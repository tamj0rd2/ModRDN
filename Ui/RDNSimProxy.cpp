/////////////////////////////////////////////////////////////////////
// File    : RDNSimProxy.cpp
// Desc    :
// Created : Monday, April 23, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNSimProxy.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "RDNText.h"

#include "../Simulation/RDNPlayer.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNQuery.h"
#include "../Simulation/RDNEBPs.h"
#include "../Simulation/GameEventDefs.h"
#include "../Simulation/CommandTypes.h"
#include "../Simulation/CommandProcessor.h"
#include "../Simulation/AttackTypes.h"

#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/Extensions/HealthExt.h"
#include "../Simulation/Extensions/ResourceExt.h"
#include "../Simulation/Extensions/MovingExt.h"
#include "../Simulation/Extensions/AttackExt.h"
#include "../Simulation/Extensions/UnitSpawnerExt.h"

#include "../Simulation/ExtInfo/SiteExtInfo.h"

#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/EntityCommand.h>
#include <SimEngine/EntityAnimator.h>

#include <EngineAPI/SelectionInterface.h>
#include <EngineAPI/CommandInterface.h>
#include <EngineAPI/CameraInterface.h>
#include <EngineAPI/UIInterface.h>
#include <EngineAPI/FXInterface.h>
#include <EngineAPI/RTSHud.h>

#include <Lua/LuaConfig.h>
#include <Lua/LuaBinding.h>
#include <Lua/GlobalLua.h>

#include <Platform/Platform.h>
#include <Platform/InputTypes.h>

#include <Assist/StlExString.h>
#include <Assist/StlExVector.h>

/////////////////////////////////////////////////////////////////////
//

namespace
{
	typedef std::map<unsigned long, long>
			ProxyUnitOrder;

	const float k_FlashTime = 0.25f;

	const char *k_FXMoveAttack = "attackdest";
	const char *k_FXMovePatrol = "patroldest";
	const char *k_FXMoveGuard = "guarddest";
	const char *k_FXMove = "movedest";

	const long BUILDUNIT_MULTIPLEUNITS = 5;
} // namespace

/////////////////////////////////////////////////////////////////////
//

namespace
{
	struct EntityFlashRec
	{
		EntityGroup entity;
		float timer;
	};

	typedef std::smallvector<EntityFlashRec, 20>
			EntityFlashList;

	struct EqualControllerType : public std::binary_function<const Entity *, int, bool>
	{
		bool operator()(const Entity *l, int r) const
		{
			return (l->GetControllerBP() && (l->GetControllerBP()->GetControllerType() == static_cast<unsigned long>(r)));
		}
	};
} // namespace

/////////////////////////////////////////////////////////////////////
// RDNSimProxy

class RDNSimProxy::Data
{
public:
	LuaConfig *m_lua;

	SelectionInterface *m_selection;
	UIInterface *m_uiinterface;
	CommandInterface *m_command;
	CameraInterface *m_camera;
	FXInterface *m_fx;
	RTSHud *m_hud;

	const RDNWorld *m_world;
	const RDNPlayer *m_player;

	bool m_initial;
	bool m_dirty;
	bool m_refresh;
	bool m_newSelection;

	std::vector<LuaBinding::Obj>
			m_exported;

	std::vector<LuaBinding::Obj>
			m_cheats;

	ProxyUnitOrder m_proxyUnit;

	int m_modalMode;

	EntityGroup m_cachedRallyTarget;
	Vec3f m_cachedRallyPosition;
	bool m_bCachedRally;

	//
	EntityFlashList m_flashentities;
};

RDNSimProxy::RDNSimProxy(
		LuaConfig *lua,
		SelectionInterface *selection,
		CommandInterface *command,
		CameraInterface *camera,
		UIInterface *uiinterface,
		FXInterface *fx,
		RTSHud *hud,
		const RDNWorld *world,
		const RDNPlayer *player)
		: m_pimpl(new Data)
{
	// init fields
	m_pimpl->m_lua = lua;

	m_pimpl->m_selection = selection;
	m_pimpl->m_command = command;
	m_pimpl->m_camera = camera;
	m_pimpl->m_uiinterface = uiinterface;
	m_pimpl->m_fx = fx;
	m_pimpl->m_hud = hud;

	m_pimpl->m_world = world;
	m_pimpl->m_player = player;

	m_pimpl->m_initial = true;
	m_pimpl->m_dirty = true;
	m_pimpl->m_refresh = false;
	m_pimpl->m_newSelection = true; // used for updating the taskbar
																	// - m_newSelection = true -> clear all taskbar menu context,
																	// - m_newSelection = fase -> retain the taskbar menu context, if possible

	m_pimpl->m_bCachedRally = false;

	// observe the selection
	m_pimpl->m_selection->Register_Observer(this);

	// observe the events
	GameEventSys::Instance()->RegisterClient(this);

	//
	Preload();

	// register to lua
	LuaSetup();

	// register cheats
	CheatSetup();

	return;
}

RDNSimProxy::~RDNSimProxy()
{
	// unregister from events
	GameEventSys::Instance()->UnregisterClient(this);

	// unregister from selection
	m_pimpl->m_selection->Remove_Observer(this);

	// clean-up lua
	LuaReset();

	//
	CheatReset();

	DELETEZERO(m_pimpl);

	return;
}

void RDNSimProxy::LuaSetup()
{
	// constants
#define BINDCONSTANT(c) \
	m_pimpl->m_lua->SetNumber(#c, double(c))

	BINDCONSTANT(FC_NeedCash);
	BINDCONSTANT(FC_BuildQueueFull);
	BINDCONSTANT(FC_TooManyUnit);
	BINDCONSTANT(FC_Other);

	BINDCONSTANT(Lab_EC);
	BINDCONSTANT(Henchmen_EC);

	BINDCONSTANT(ATTACKTYPE_Melee);

#undef BINDCONSTANT

#define BINDINNERCONSTANT(t, c) \
	m_pimpl->m_lua->SetNumber(#c, double(t::c))

	BINDINNERCONSTANT(UIInterface, MM_None);
	BINDINNERCONSTANT(UIInterface, MM_Cursor);
	BINDINNERCONSTANT(UIInterface, MM_LockCursor);

#undef BINDINNERCONSTANT

	// functions
#define BINDFUNC(f) \
	m_pimpl->m_exported.push_back(LuaBinding::Bind(m_pimpl->m_lua, #f, this, &RDNSimProxy::f))

	BINDFUNC(LocalPlayer);

	BINDFUNC(EntityBelongsToPlayer);
	BINDFUNC(EntityType);
	BINDFUNC(EntityEBP);
	BINDFUNC(EntityOwner);
	BINDFUNC(EntityInSpawning);

	BINDFUNC(RockEBP);
	BINDFUNC(PaperEBP);
	BINDFUNC(ScissorEBP);

	BINDFUNC(UnitCanBeBuiltHere);
	BINDFUNC(BuildQueueLength);
	BINDFUNC(BuildingEBPFromType);
	BINDFUNC(TypeFromEBP);

	BINDFUNC(LocalPlayerLabId);

	BINDFUNC(DoBuildUnit);
	BINDFUNC(DoCancelBuildUnit);
	BINDFUNC(DoCommandStop);
	BINDFUNC(DoDestroy);
	BINDFUNC(DoModalCommand);

#undef BINDFUNC

	return;
}

void RDNSimProxy::LuaReset()
{
	m_pimpl->m_exported.clear();
}

void RDNSimProxy::CheatSetup()
{
#define BINDFUNC(n, f) \
	m_pimpl->m_cheats.push_back(LuaBinding::Bind(GlobalLua::GetState(), #n, this, &RDNSimProxy::f))

	BINDFUNC(cheat_cash, CheatCash);
	BINDFUNC(cheat_killself, CheatKillSelf);

#undef BINDFUNC

	return;
}

void RDNSimProxy::CheatReset()
{
	m_pimpl->m_cheats.clear();
}

void RDNSimProxy::Notify_Insertion(Entity *e)
{
	UNREF_P(e);

	// flag
	m_pimpl->m_dirty = true;
	m_pimpl->m_newSelection = true;

	return;
}

void RDNSimProxy::Notify_Removal(Entity *e)
{
	UNREF_P(e);

	// flag
	m_pimpl->m_dirty = true;
	m_pimpl->m_newSelection = false;

	return;
}

bool RDNSimProxy::Input(const Plat::InputEvent &ie)
{
	// force a refresh before each keyboard command/hotkey
	if (ie.filter == Plat::IFT_KeyBoard)
	{
		// NOTE: we need to refresh the taskbar BEFORE processing hotkeys 'cuz it could
		// change the selection, and then the taskbar would keep on showing stuff
		// related to the previous selection until next frame
		Refresh();
	}

	// we should NEVER process any key in here
	return false;
}

void RDNSimProxy::Refresh()
{
	if (m_pimpl->m_refresh)
	{
		// flag
		m_pimpl->m_refresh = false;

		// call into lua
		LuaBinding::Call<void> c;
		c.Execute(m_pimpl->m_lua, "on_refresh");
	}

	// selection
	if (m_pimpl->m_dirty)
	{
		// flag
		m_pimpl->m_dirty = false;

		// call into lua
		LuaBinding::Call<void> c;

		if (m_pimpl->m_newSelection)
		{
			c.Execute(m_pimpl->m_lua, "on_selection");
		}
		else
		{
			// we probably lost one (or more) of the selected entities; we'll try to
			// refresh the taskbar so we don't lose any submenu context
			c.Execute(m_pimpl->m_lua, "on_refresh");
			m_pimpl->m_newSelection = true;
		}
	}

	return;
}

void RDNSimProxy::Update()
{
	// initial update
	if (m_pimpl->m_initial)
	{
		// flag
		m_pimpl->m_initial = false;

		// call into lua
		LuaBinding::Call<void> c;
		c.Execute(m_pimpl->m_lua, "on_initial");
	}

	// refresh
	Refresh();

	//
	EntityFlashList::iterator iter = m_pimpl->m_flashentities.begin();

	const float curTime = Plat::Time::GetSeconds();
	const float flashexpire = curTime;

	for (; iter != m_pimpl->m_flashentities.end();)
	{
		bool bDelete = false;

		if ((*iter).entity.front())
		{
			EntityFlashRec &flashrec = (*iter);

			if (flashexpire >= flashrec.timer)
			{
				bDelete = true;
			}
		}
		else
		{
			bDelete = true;
		}

		if (bDelete)
		{
			// turn off the flash
			if ((*iter).entity.front())
			{
				EntityFlashRec &flashrec = (*iter);

				EntityAnimator *pAnimator = flashrec.entity.front()->GetAnimator();

				pAnimator->SetFullbright(false);
			}

			iter = std::vector_eraseback(m_pimpl->m_flashentities, iter);
		}
		else
		{
			++iter;
		}
	}

	return;
}

int RDNSimProxy::EntityType(int id)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(id);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	return e->GetControllerBP()->GetControllerType();
}

int RDNSimProxy::EntityOwner(int id)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(id);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	//
	const Player *p = e->GetOwner();

	return (p != 0) ? p->GetID() : 0;
}

bool RDNSimProxy::EntityInSpawning(int id)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(id);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	// get extension
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
		return false;

	if (spawner->UnitInProgress().first != 0)
		return true;

	// check build order cache
	ProxyUnitOrder::const_iterator found =
			m_pimpl->m_proxyUnit.find(id);

	return found != m_pimpl->m_proxyUnit.end();
}

const ControllerBlueprint *RDNSimProxy::BuildQueueAt(int building, int index) const
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(building);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	// get extension
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
	{
		dbBreak();
		return 0;
	}

	// check entity
	const ControllerBlueprint *cbp = 0;

	if ((cbp = spawner->BuildQueueAt(index)) != 0)
		return cbp;

	// check build order cache
	ProxyUnitOrder::const_iterator found = m_pimpl->m_proxyUnit.find(building);

	if (found == m_pimpl->m_proxyUnit.end())
	{
		dbBreak();
		return 0;
	}

	//
	int cacheIndex = index - spawner->BuildQueueSize();

	if (cacheIndex > 1)
	{
		dbBreak();
		return 0;
	}

	const ControllerBlueprint *cbpCache = ef->GetControllerBP(found->second);

	return cbpCache;
}

float RDNSimProxy::BuildQueueBar(int building) const
{
	//
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(building);

	if (e == 0)
	{
		dbBreak();
		return 0.0f;
	}

	//
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
	{
		dbBreak();
		return 0.0f;
	}

	//
	if (spawner->UnitInProgress().first != 0)
		return spawner->UnitInProgress().second;

	// check build order cache
	ProxyUnitOrder::const_iterator found = m_pimpl->m_proxyUnit.find(building);

	if (found == m_pimpl->m_proxyUnit.end())
	{
		dbBreak();
		return 0.0f;
	}

	return 0.0f;
}

int RDNSimProxy::BuildQueueLength(int id)
{
	// entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(id);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	// get extension
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
	{
		dbBreak();
		return 0;
	}

	int size = spawner->BuildQueueSize();

	// check build order cache
	ProxyUnitOrder::const_iterator found = m_pimpl->m_proxyUnit.find(id);

	if (found != m_pimpl->m_proxyUnit.end())
	{
		size += 1;
	}

	return size;
}

bool RDNSimProxy::UnitCanBeBuiltHere(int building, int ebpid)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *b = ef->GetEntityFromEID(building);

	if (b == 0)
	{
		dbBreak();
		return false;
	}

	//
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(b);

	if (spawner == 0)
	{
		dbBreak();
		return false;
	}

	//
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		dbBreak();
		return false;
	}

	return spawner->UnitListFilter(cbp);
}

int RDNSimProxy::RockEBP() const
{
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const ControllerBlueprint *cbp = const_cast<EntityFactory *>(ef)->GetControllerBP(RDNEBP::Rock.folder, RDNEBP::Rock.file);

	return cbp->GetEBPNetworkID();
}

int RDNSimProxy::PaperEBP() const
{
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const ControllerBlueprint *cbp = const_cast<EntityFactory *>(ef)->GetControllerBP(RDNEBP::Paper.folder, RDNEBP::Paper.file);

	return cbp->GetEBPNetworkID();
}

int RDNSimProxy::ScissorEBP() const
{
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const ControllerBlueprint *cbp = const_cast<EntityFactory *>(ef)->GetControllerBP(RDNEBP::Scissor.folder, RDNEBP::Scissor.file);

	return cbp->GetEBPNetworkID();
}

int RDNSimProxy::BuildingEBPFromType(int type)
{
	dbTracef(">>>DEBUG trying to get building EBP from type %d", type);
	const RDNEBP::EBPName *ebp = 0;

	switch (type)
	{
	case Lab_EC:
		ebp = &RDNEBP::Lab;
		break;

	default:
		// oops!
		dbBreak();
		return 0;
	}

	//
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const ControllerBlueprint *cbp = const_cast<EntityFactory *>(ef)->GetControllerBP(ebp->folder, ebp->file);

	return cbp->GetEBPNetworkID();
}

int RDNSimProxy::TypeFromEBP(int ebpid)
{
	dbTracef("Getting a type from ebp with id %s", ebpid);
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp)
	{
		return cbp->GetControllerType();
	}

	return NULL_EC;
}

int RDNSimProxy::LocalPlayer() const
{
	// check player
	if (m_pimpl->m_player == 0)
		return 0;

	return m_pimpl->m_player->GetID();
}

int RDNSimProxy::LocalPlayerLabId() const
{
	// check player
	if (m_pimpl->m_player == 0 ||
			m_pimpl->m_player->IsPlayerDead())
		return 0;

	// check lab
	const Entity *lab = m_pimpl->m_player->GetLabEntity();

	if (lab == 0)
		return 0;

	//
	return lab->GetID();
}

void RDNSimProxy::GetCursorInfo(char *cursor, size_t len, int &ttStrId, const Entity *mouseOverEntity)
{
	// init out parms
	relicstring_copyN(cursor, "default", len);
	ttStrId = 0;

	// check for modalModes
	const UIInterface::ModalModes mode = m_pimpl->m_uiinterface->GetModalMode();

	if (mode != UIInterface::MM_None)
	{
		// modal cursors are handled in the taskbar
		dbBreak();
	}
	// non-modal cursors
	else
	{
		//
		if (mouseOverEntity == 0)
		{
			relicstring_copyN(cursor, "default", len);
		}
		else
				// check if over something in the selection
				if (m_pimpl->m_selection->GetSelection().find(mouseOverEntity) != m_pimpl->m_selection->GetSelection().end())
		{
			relicstring_copyN(cursor, "default", len);
		}
		else
				// nothing is currently selected, or an entity not owned by the player is selected
				if (m_pimpl->m_selection->GetSelection().empty() ||
						m_pimpl->m_selection->GetSelection().front()->GetOwner() != m_pimpl->m_player)
		{
			if (mouseOverEntity->GetEntityFlag(EF_Selectable))
			{
				relicstring_copyN(cursor, "eye", len);
			}
			else
			{
				relicstring_copyN(cursor, "default", len);
			}
		}
		else if (RDNQuery::CanAttack(m_pimpl->m_selection->GetSelection(), mouseOverEntity))
		{
			// cursor over an enemy, soldiers in the selection
			relicstring_copyN(cursor, "attack", len);
		}
		else
				// check for guard: do this after heal
				if (RDNQuery::CanGuard(m_pimpl->m_selection->GetSelection(), mouseOverEntity, m_pimpl->m_player))
		{
			relicstring_copyN(cursor, "modal_guard", len);
		}
		else
				// this should be last.  If no other context-sensitive cursors applies the selection "eye" and tooltip should be shown
				if (mouseOverEntity->GetEntityFlag(EF_Selectable))
		{
			relicstring_copyN(cursor, "eye", len);
		}
	}

	return;
}

void RDNSimProxy::DoModalCommand(int mode, float x, float y, float z, int entityID, bool bQueueCommand)
{
	// validate selection first
	if (!SelectionCanReceiveCommand(0))
		return;

	struct Modal2Command
	{
		unsigned char command;
		unsigned long param;
	};

	const Modal2Command mode2command[] =
			{
					0, 0,							 // MC_None
					CMD_Move, 0,			 // MC_Move
					CMD_Attack, 0,		 // MC_Attack
					0, 0,							 // MC_BuildStructure
					CMD_AttackMove, 0, // MC_AttackMove
					CMD_RallyPoint, 0, // MC_SetRallyPoint
			};

	if (mode < 0 || mode >= (sizeof(mode2command) / sizeof(mode2command[0])))
	{
		dbBreak();
		return;
	}

	// if this is an attackmove command on an entity from a creature, convert to point command
	if ((mode2command[mode].command == CMD_AttackMove) && entityID)
	{
		const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
		const Entity *e = ef->GetEntityFromEID(entityID);

		if (e == 0)
		{
			dbBreak();
			return;
		}

		x = e->GetPosition().x;
		y = e->GetPosition().y;
		z = e->GetPosition().z;

		entityID = 0;
	}

	if (entityID)
	{
		// locate entity
		const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
		const Entity *e = ef->GetEntityFromEID(entityID);

		if (e == 0)
		{
			dbBreak();
			return;
		}

		//
		EntityGroup eg;
		eg.push_back(const_cast<Entity *>(e));

		// validate the command
		// NOTE: RDNQuery::CanDoCommand() is incomplete and defaults to true
		if (RDNQuery::CanDoCommand(m_pimpl->m_selection->GetSelection(), eg, mode2command[mode].command, mode2command[mode].param))
		{ // command is valid

			if (mode2command[mode].command == CMD_RallyPoint)
			{ // cache the rally target until the command is processed
				m_pimpl->m_cachedRallyTarget = eg;
				m_pimpl->m_bCachedRally = true;
			}

			// send the appropriate network message
			m_pimpl->m_command->DoEntityEntity(
					mode2command[mode].command,
					mode2command[mode].param,
					bQueueCommand ? CMDF_Queue : 0,
					m_pimpl->m_player,
					m_pimpl->m_selection->GetSelection(),
					eg);

			OnEntityEntityCmd(eg.front());
		}
	}
	else
	{
		Vec3f temp(x, y, z);

		// clamp the point to the world
		GetWorld()->ClampPointToWorld(temp);

		if (mode2command[mode].command == CMD_RallyPoint)
		{ // cache the rally target until the command is processed
			m_pimpl->m_cachedRallyPosition = temp;
			m_pimpl->m_cachedRallyTarget.clear();
			m_pimpl->m_bCachedRally = true;
		}

		// send the appropriate network message
		m_pimpl->m_command->DoEntityPoint(
				mode2command[mode].command,
				mode2command[mode].param,
				bQueueCommand ? CMDF_Queue : 0,
				m_pimpl->m_player,
				m_pimpl->m_selection->GetSelection(),
				&temp,
				1);

		OnEntityPointCmd(temp, mode2command[mode].command);
	}

	if (!bQueueCommand)
	{
		// assume the selected object has changed
		m_pimpl->m_dirty = true;
	}

	return;
}

void RDNSimProxy::DoCommand(const Vec3f *v, unsigned long num, bool bQueueCommand)
{
	// validate selection first
	if (!SelectionCanReceiveCommand(0))
		return;

	// clamp the v's to the world

	// OPTIMIZE: this could be better
	std::vector<Vec3f> vClamped;
	vClamped.reserve(num);
	for (unsigned long i = 0; i < num; i++)
	{
		Vec3f vc = v[i];
		GetWorld()->ClampPointToWorld(vc);
		vClamped.push_back(vc);
	}

	// if a building that can spawn units is selected,
	// then cache the rally point
	if (QIExt<UnitSpawnerExt>(m_pimpl->m_selection->GetSelection().front()) != NULL)
	{
		m_pimpl->m_cachedRallyPosition = vClamped[0];
		m_pimpl->m_cachedRallyTarget.clear();
		m_pimpl->m_bCachedRally = true;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntityPoint(
			CMD_DefaultAction,
			0,
			bQueueCommand ? CMDF_Queue : 0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection(),
			&vClamped[0],
			num);

	// display movement arrow (if the selection constains something that can move)
	EntityGroup::const_iterator si = m_pimpl->m_selection->GetSelection().begin();
	EntityGroup::const_iterator se = m_pimpl->m_selection->GetSelection().end();

	for (; si != se; ++si)
	{
		if (QIExt<MovingExt>(*si) != NULL)
			break;
	}

	if (si != se)
	{
		OnEntityPointCmd(vClamped[0], CMD_DefaultAction);
	}

	return;
}

void RDNSimProxy::DoCommand(const EntityGroup &eg, bool bQueueCommand)
{
	// validate selection first
	if (!SelectionCanReceiveCommand(0))
		return;

	// if a building that can spawn units is selected,
	// and the target can be the rally point,
	// then cache the rally point
	if (eg.front() &&
			QIExt<UnitSpawnerExt>(m_pimpl->m_selection->GetSelection().front()) != NULL &&
			RDNQuery::CanRallyTo(eg.front(), m_pimpl->m_player))
	{
		// cache the rally target until the command is processed
		m_pimpl->m_cachedRallyTarget = eg;
		m_pimpl->m_bCachedRally = true;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntityEntity(
			CMD_DefaultAction,
			0,
			bQueueCommand ? CMDF_Queue : 0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection(),
			eg);

	if (eg.front())
	{
		// Flash the target entity, only if someone will do something to it
		EntityGroup::const_iterator si = m_pimpl->m_selection->GetSelection().begin();
		EntityGroup::const_iterator se = m_pimpl->m_selection->GetSelection().end();

		for (; si != se; ++si)
		{
			if (CommandProcessor::GetDefaultEntityEntityCommand(*si, eg.front()) != CMD_DefaultAction)
				break;
		}

		if (si != se)
		{
			OnEntityEntityCmd(eg.front());
		}
	}
}

int RDNSimProxy::DoBuildUnit(int ebpid)
{
	// validate parms
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();

	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);
	if (cbp == 0 || (cbp->GetControllerType() != Henchmen_EC))
	{
		dbBreak();
		return FC_Other;
	}

	// validate object state
	if (!SelectionCanReceiveCommand(1))
	{
		dbBreak();
		return FC_Other;
	}

	// quick check player
	const RDNPlayer::BuildResult br = m_pimpl->m_player->BlueprintCanBuild(cbp);

	if (br != RDNPlayer::BR_AllowBuild)
	{
		// ignore command
		FailedCommand fc;

		if (br == RDNPlayer::BR_NeedResourceCash)
			fc = FC_NeedCash;
		else
			fc = FC_Other;

		return fc;
	}

	// check if there is room in the build queue
	int queueLengthLeft = UnitSpawnerExt::MAXQUEUELENGTH - BuildQueueLength(m_pimpl->m_selection->GetSelection().front()->GetID());
	if (queueLengthLeft <= 0)
	{
		// ignore command
		return FC_BuildQueueFull;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntity(
			CMD_BuildUnit,
			ebpid,
			0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection());

	// store the build order
	const Entity *building =
			m_pimpl->m_selection->GetSelection().front();
	m_pimpl->m_proxyUnit[building->GetID()] = ebpid;

	// assume the selected object has changed
	m_pimpl->m_dirty = true;

	return 0;
}

void RDNSimProxy::DoCancelBuildUnit(int unitIndex)
{
	// validate object state
	if (!SelectionCanReceiveCommand(1))
	{
		dbBreak();
		return;
	}

	// locate entity
	const ControllerBlueprint *cbp = BuildQueueAt(GetSelection().front()->GetID(), unitIndex);

	if (cbp == 0)
	{
		dbBreak();
		return;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntity(
			CMD_CancelBuildUnit,
			unitIndex,
			0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection());

	// assume the selected object has changed
	m_pimpl->m_dirty = true;

	return;
}

void RDNSimProxy::DoCommandStop()
{
	// validate object state
	if (!SelectionCanReceiveCommand(0))
	{
		dbBreak();
		return;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntity(
			CMD_Stop,
			0,
			0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection());

	return;
}

void RDNSimProxy::DoDestroy()
{
	// validate object state
	if (!SelectionCanReceiveCommand(0))
	{
		dbBreak();
		return;
	}

	// send the appropriate network message
	m_pimpl->m_command->DoEntity(
			CMD_Destroy,
			0,
			0,
			m_pimpl->m_player,
			m_pimpl->m_selection->GetSelection());

	return;
}

const RDNWorld *RDNSimProxy::GetWorld() const
{
	return m_pimpl->m_world;
}

const RDNPlayer *RDNSimProxy::GetPlayer() const
{
	return m_pimpl->m_player;
}

const EntityGroup &RDNSimProxy::GetSelection() const
{
	return m_pimpl->m_selection->GetSelection();
}

void RDNSimProxy::OnEvent(const GameEventSys::Event &event)
{
	// handle special events
	if (event.GetType() == GE_PlayerKilled)
	{
		if (static_cast<const GameEvent_PlayerKilled &>(event).m_killed == m_pimpl->m_player)
		{
			m_pimpl->m_dirty = true;
			return;
		}
	}

	// ignore all those events not addressed to me
	if (m_pimpl->m_player != 0 && event.GetPlayer() != m_pimpl->m_player)
		return;

	// dirty flag if the event is related to the selection
	const Entity *e = 0;

	switch (event.GetType())
	{
#define EVENTENTITY(type) static_cast<const type &>(event).m_pSpawner;
	case GE_BuildUnitStart:
		e = EVENTENTITY(GameEvent_BuildUnitStart);
		break;
	case GE_BuildUnitComplete:
		e = EVENTENTITY(GameEvent_BuildUnitComplete);
		break;
	case GE_BuildUnitCancel:
		e = EVENTENTITY(GameEvent_BuildUnitCancel);
		break;
#undef EVENTENTITY
	}

	if (e != 0)
	{
		if (m_pimpl->m_selection->GetSelection().find(e) != m_pimpl->m_selection->GetSelection().end())
		{
			m_pimpl->m_dirty = true;
		}
	}

	// special handling these event
	if (event.GetType() == GE_BuildUnitCommand)
	{
		const GameEvent_BuildUnitCommand &evbu = static_cast<const GameEvent_BuildUnitCommand &>(event);

		ProxyUnitOrder::iterator found =
				m_pimpl->m_proxyUnit.find(evbu.m_pEntity->GetID());

		if (found != m_pimpl->m_proxyUnit.end())
		{
			m_pimpl->m_proxyUnit.erase(found);
		}
	}
	else if (event.GetType() == GE_RallyPointSet)
	{
		// not caching the rally point anymore, the command completed
		m_pimpl->m_bCachedRally = false;
	}

	// refresh taskbar on some events
	int etype = event.GetType();
	switch (etype)
	{
	case GE_EntityKilled:
	{
		// check to see if the entity is owned by the local player
		const GameEvent_EntityKilled &killev = static_cast<const GameEvent_EntityKilled &>(event);
		if (killev.m_victim->GetOwner() == m_pimpl->m_player)
		{
			SetRefreshFlag();
		}
		break;
	}
	}

	return;
}

int RDNSimProxy::ValidateBuildUI(const ControllerBlueprint *cbp) const
{
	// validate parm
	dbAssert(cbp != 0);

	// validate object state
	dbAssert(m_pimpl->m_player != 0);

	// quick validation
	const RDNPlayer::BuildResult br = m_pimpl->m_player->BlueprintCanBuild(cbp);

	if (br != RDNPlayer::BR_AllowBuild)
	{
		if (br == RDNPlayer::BR_NeedResourceCash)
			return FC_NeedCash;
		else
			return FC_Other;
	}

	return 0;
}

int RDNSimProxy::EntityEBP(int entityId)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return 0;
	}

	return e->GetControllerBP()->GetEBPNetworkID();
}

bool RDNSimProxy::EntityBelongsToPlayer(int entityid) const
{
	// check player
	if (m_pimpl->m_player == 0 ||
			m_pimpl->m_player->IsPlayerDead())
		return false;

	// check 1st entity in selection
	const EntityFactory *ef = m_pimpl->m_world->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityid);

	return m_pimpl->m_player->CanControlEntity(e);
}

void RDNSimProxy::CheatCash(int n)
{
	// check player
	if (m_pimpl->m_player == 0 ||
			m_pimpl->m_player->IsPlayerDead())
		return;

	// send command
	m_pimpl->m_command->DoPlayerPlayer(
			PCMD_CheatCash,
			n,
			false,
			m_pimpl->m_player,
			m_pimpl->m_player);

	return;
}

void RDNSimProxy::CheatKillSelf()
{
	// check player
	if (m_pimpl->m_player == 0 ||
			m_pimpl->m_player->IsPlayerDead())
		return;

	// send command
	m_pimpl->m_command->DoPlayerPlayer(
			PCMD_CheatKillSelf,
			0,
			false,
			m_pimpl->m_player,
			m_pimpl->m_player);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns true if there is a cached rally target or position
//	Result	:
//	Param.	: position - pointer to a position pointer
//			  target - pointer to a EntityGroup pointer
//	Author	: dswinerd
//
bool RDNSimProxy::GetCachedRally(const Vec3f **const position, const EntityGroup **const target) const
{
	if (!m_pimpl->m_bCachedRally)
	{
		return false;
	}

	// using pointers to avoid unnecessary copies.  looks a bit nasty though.
	*position = &m_pimpl->m_cachedRallyPosition;
	*target = &m_pimpl->m_cachedRallyTarget;

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNSimProxy::SelectionCanReceiveCommand(int max)
{
	if (m_pimpl->m_player == 0 ||
			m_pimpl->m_player->IsPlayerDead())
		return false;

	if (m_pimpl->m_selection->GetSelection().front() == 0 ||
			m_pimpl->m_selection->GetSelection().front()->GetOwner() != m_pimpl->m_player)
		return false;

	if (m_pimpl->m_selection->GetSelection().size() > size_t(max) && max != 0)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNSimProxy::OnEntityPointCmd(const Vec3f &target, int command)
{
	// validate parm
	dbAssert(command < CMD_COUNT);

	// find out which type of command this is
	const char *fxName = 0;
	switch (command)
	{
	case CMD_RallyPoint:
		//	We already have an effect there.
		return;

	case CMD_Attack:
	case CMD_AttackMove:
		fxName = k_FXMoveAttack;
		break;

	default:
		fxName = k_FXMove;
		break;
	}

	// spawn appropriate fx
	FXInterface::Handle h = m_pimpl->m_fx->FXCreate(fxName);

	Matrix43f m;
	m.IdentitySelf();
	m.T = target;

	m_pimpl->m_fx->FXSetTransform(h, m);
	m_pimpl->m_fx->FXSetScale(h, 1.0f);
	m_pimpl->m_fx->FXSetLength(h, 1.0f);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNSimProxy::OnEntityEntityCmd(const Entity *target)
{
	EntityAnimator *pAnimator = target->GetAnimator();

	if (pAnimator)
	{
		EntityFlashList::iterator iter = m_pimpl->m_flashentities.begin();
		EntityFlashList::iterator eiter = m_pimpl->m_flashentities.end();

		for (; iter != eiter; ++iter)
		{
			if ((*iter).entity.front() == target)
			{
				break;
			}
		}

		if (iter != eiter)
		{
			(*iter).timer = Plat::Time::GetSeconds() + k_FlashTime;
		}
		else
		{
			EntityFlashRec newRecord;

			newRecord.entity.push_back(const_cast<Entity *>(target));
			newRecord.timer = Plat::Time::GetSeconds() + k_FlashTime;

			pAnimator->SetFullbright(true);

			m_pimpl->m_flashentities.push_back(newRecord);
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNSimProxy::SetDirtyFlag()
{
	m_pimpl->m_dirty = true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNSimProxy::SetRefreshFlag()
{
	m_pimpl->m_refresh = true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
CommandInterface *RDNSimProxy::GetCommand()
{
	return m_pimpl->m_command;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
SelectionInterface *RDNSimProxy::GetSelectionInterface()
{
	return m_pimpl->m_selection;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
CameraInterface *RDNSimProxy::GetCameraInterface()
{
	return m_pimpl->m_camera;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNSimProxy::Preload()
{
	m_pimpl->m_fx->FXPreload(k_FXMoveAttack);
	m_pimpl->m_fx->FXPreload(k_FXMovePatrol);
	m_pimpl->m_fx->FXPreload(k_FXMoveGuard);
	m_pimpl->m_fx->FXPreload(k_FXMove);
}
