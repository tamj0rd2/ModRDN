/////////////////////////////////////////////////////////////////////
// File    : RDNTaskbar.cpp
// Desc    :
// Created : Monday, April 23, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNTaskbar.h"

#include "../ModObj.h"

#include "RDNSimProxy.h"
#include "RDNMinimap.h"
#include "RDNInputBinder.h"
#include "RDNUIProxy.h"
#include "RDNText.h"

#include "../Simulation/Controllers/ControllerTypes.h"
#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/RDNPlayer.h"
#include "../Simulation/RDNEBPs.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNQuery.h"
#include "../Simulation/GameEventDefs.h"

#include "../Simulation/Extensions/HealthExt.h"
#include "../Simulation/Extensions/UnitSpawnerExt.h"
#include "../Simulation/Extensions/ResourceExt.h"
#include "../Simulation/Extensions/ModifierExt.h"

#include "../Simulation/ExtInfo/CostExtInfo.h"
#include "../Simulation/ExtInfo/SiteExtInfo.h"

#include "../Simulation/States/StateMove.h"
#include "../Simulation/States/StateGroupMove.h"

#include <EngineAPI/RTSHud.h>
#include <EngineAPI/ControllerBlueprint.h>
#include <EngineAPI/EntityFactory.h>
#include <EngineAPI/SoundInterface.h>
#include <EngineAPI/UIInterface.h>
#include <EngineAPI/FXInterface.h>
#include <EngineAPI/SelectionInterface.h>
#include <EngineAPI/CameraInterface.h>

#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/Pathfinding/ImpassMap.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/BuildingDynamics.h>

#include <Lua/LuaBinding.h>
#include <Util/Colour.h>

#include <Platform/Platform.h>
#include <Platform/InputTypes.h>

#include <Assist/FixedString.h>

/////////////////////////////////////////////////////////////////////
//

static const char *CURRENTSCREEN = "";

namespace
{
	class BindingHud
	{
	public:
		enum BindingType
		{
			BHUD_LABEL_IMAGE = 1,
			BHUD_LABEL_TEXT,
			BHUD_LABEL_TEXTTIMER,

			BHUD_LABEL_PLAYERNAME,
			BHUD_LABEL_PLAYERCASH,
			BHUD_LABEL_PLAYERPOP,
			BHUD_LABEL_PLAYERCOLOUR,

			BHUD_LABEL_ENTITYNAME,
			BHUD_LABEL_ENTITYHEALTH,

			BHUD_LABEL_BUILDQUEUE,
			BHUD_LABEL_BUILDQUEUEPROGRESS,

			BHUD_LABEL_EBPCOSTCASH,
			BHUD_LABEL_EBPNAME,
			BHUD_LABEL_EBPPREREQUISITE,
			BHUD_ICON_EBPATTRIBUTE,
			BHUD_LABEL_EBPATTRIBUTE,

			BHUD_LABEL_EBPSPEED_LAND,
			BHUD_LABEL_EBPSPEED_WATER,
			BHUD_LABEL_EBPSPEED_AIR,
			BHUD_LABEL_EBPSPEED_PUREWATER,

			BHUD_LABEL_EBPABILITY,
			BHUD_LABEL_EBPRANGEATTACK,
			BHUD_ICON_EBPRANGEATTACK,

			BHUD_LABEL_RESOURCE,

			BHUD_LABEL_REXABILITY,

			BHUD_LABEL_GAMETIME,

			BHUD_BAR_ENTITYHEALTH,
			BHUD_BAR_BUILDQUEUE,

			BHUD_TOOLTIP,
		};

	public:
		BindingType type;
		fstring<31> hud;
		fstring<31> tooltip;

		float secondDie;

		// the number of parm members has to match the number in BindingButton
		int parameter0;
		int parameter1;
		int parameter2;

	public:
		BindingHud()
				: hud(""), parameter0(0), parameter1(0), parameter2(0), type(BindingType(0)), secondDie(0.0f)
		{
		}
	};

	class BindingButton
	{
	public:
		enum BindType
		{
			BHUD_BUTTON_NORMAL = 1,
			BHUD_BUTTON_BUILDING,
			BHUD_BUTTON_UNIT,
			BHUD_BUTTON_GROUP,
			BHUD_BUTTON_CHAT,
			BHUD_BUTTON_SELECTENTITY,
			BHUD_BUTTON_STANCE,
		};

	public:
		BindType type;
		fstring<31> hud;
		fstring<31> callback_left;
		fstring<31> callback_right;
		fstring<31> tooltip;

		RDNInputBinder::BindedKeyComboName
				keyComboName;

		// the number of parm members has to match the number in BindingHud
		int parameter0; // first paramter usually used for enabled status
		int parameter1;
		int parameter2;

		bool bClear; // set to true when this button should be cleared
	};

	class RemoveAndHideClearedPredicate
	{
	public:
		RemoveAndHideClearedPredicate(RTSHud *hud) : m_pHud(hud) { ; }

	public:
		bool operator()(const BindingButton &b)
		{
			if (b.bClear)
			{
				m_pHud->Show(CURRENTSCREEN, b.hud.c_str(), false);
				return true;
			}
			else
				return false;
		}

	private:
		RTSHud *m_pHud;
	};

	class BindingHotkey
	{
	public:
		fstring<63> callback;
		int parameter0;
		RDNInputBinder::BindedKeyComboName
				keyComboName;
	};

	const char *EnemyIconName = "ui/ingame/enemycreatureicon.tga";
} // namespace

/////////////////////////////////////////////////////////////////////
//

namespace
{
	class BindingMode
	{
		// construction
	public:
		virtual ~BindingMode()
		{
		}

		// interface
	public:
		virtual void BindHud(const BindingHud &hud) = 0;
	};

	class BindingModeScreen : public BindingMode
	{
		// fields
	private:
		std::vector<BindingHud> *m_arrayHuds;
		RTSHud *m_rtsHud;

		// construction
	public:
		BindingModeScreen(RTSHud *r, std::vector<BindingHud> *v)
				: m_arrayHuds(v), m_rtsHud(r)
		{
		}

		// inherited -- BindingMode
	public:
		virtual void BindHud(const BindingHud &hud)
		{
			// check for duplicate
			std::vector<BindingHud>::iterator i = m_arrayHuds->begin();
			std::vector<BindingHud>::iterator e = m_arrayHuds->end();

			for (; i != e; ++i)
			{
				if (i->type == hud.type && i->hud == hud.hud)
					break;
			}

			// store
			if (i == e)
			{
				// add to vector
				m_arrayHuds->push_back(hud);
			}
			else
			{
				// replace
				*i = hud;
			}

			// show
			m_rtsHud->Show("", hud.hud.c_str(), true);

			return;
		}
	};

	// Private Type's
	struct FXPlaceInfo
	{
		FXInterface::Handle fxhandle;
		unsigned char placerestriction;
	};
	typedef std::smallvector<FXPlaceInfo, 32> FXPlaceList;

	struct FXPlacedBuildingInfo
	{
		FXPlaceList fxPlaceList;
		bool bReferenced;
		bool bIntersect;
	};
	typedef std::map<unsigned long, FXPlacedBuildingInfo> FXPlacedBuildingMap;

} // namespace

namespace
{
	const char *k_WayPointFXName = "waypoint";
	const char *k_RallyFXName = "rallypoint";
	const char *k_RallyFxNameLab = "rallypointlab";

	const char *k_CanPlaceOk = "canplace";
	const char *k_CanPlaceBad = "cantplace";
	const char *k_CanPlaceBadFow = "cantplacefow";

	typedef std::smallvector<Vec3f, 10>
			PointList;

	typedef std::smallvector<FXInterface::Handle, 10>
			FXPointList;
} // namespace

/////////////////////////////////////////////////////////////////////
//

static bool GetTooltipCBAndParms(
		fstring<31> &tooltipcb, // out parm
		int &parameter0,				// out parm
		int &parameter1,				// out parm
		int &parameter2,				// out parm
		const std::string &hud,
		std::vector<BindingButton> &boundButtons,
		std::vector<BindingHud> &boundHuds)
{

	//
	// look at bound buttons

	size_t i;
	for (i = 0; i != boundButtons.size(); ++i)
	{
		if (stricmp(hud.c_str(), boundButtons[i].hud.c_str()) == 0)
			break;
	}

	// couldn't find button
	if (i != boundButtons.size())
	{
		// fill out return parms
		const BindingButton &b = boundButtons[i];
		tooltipcb = b.tooltip;
		parameter0 = b.parameter0;
		parameter1 = b.parameter1;
		parameter2 = b.parameter2;
		return true;
	}

	//
	// look at bound hud
	for (i = 0; i != boundHuds.size(); ++i)
	{
		if (stricmp(hud.c_str(), boundHuds[i].hud.c_str()) == 0)
			break;
	}

	if (i != boundHuds.size())
	{
		// fill out return parms
		const BindingHud &h = boundHuds[i];
		tooltipcb = h.tooltip;
		parameter0 = h.parameter0;
		parameter1 = h.parameter1;
		parameter2 = h.parameter2;
		return true;
	}

	// couldn't find hud
	return false;
}

static bool IsEnabledBuilding(const RDNSimProxy *p, int ebpid)
{
	// find ebp
	const EntityFactory *ef = p->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		// oops!
		dbBreak();
		return false;
	}

	// enable?
	const RDNPlayer::BuildResult r = p->GetPlayer()->BlueprintCanBuild(cbp);

	const bool enabled =
			r == RDNPlayer::BR_AllowBuild ||
			r == RDNPlayer::BR_NeedResourceCash;

	return enabled;
}

static bool IsEnabledUnit(const RDNSimProxy *p, int ebpid, int building)
{
	// find ebp
	const EntityFactory *ef = p->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		// oops!
		dbBreak();
		return false;
	}

	//
	const Entity *e = ef->GetEntityFromEID(building);

	if (e == 0)
	{
		dbBreak();
		return false;
	}

	const UnitSpawnerExt *spawn = QIExt<UnitSpawnerExt>(e);

	if (spawn == 0)
	{
		dbBreak();
		return false;
	}

	// prerequisite
	const RDNPlayer::BuildResult r =
			p->GetPlayer()->BlueprintCanBuild(cbp);

	const bool enabled =
			r == RDNPlayer::BR_AllowBuild ||
			r == RDNPlayer::BR_NeedResourceCash;

	return enabled;
}

static void ClearFXPlaceList(FXPlaceList &fxlist, FXInterface *pFXInterface)
{
	FXPlaceList::iterator fxiter = fxlist.begin();
	FXPlaceList::iterator fxeiter = fxlist.end();

	for (; fxiter != fxeiter; ++fxiter)
	{
		pFXInterface->FXDestroy(fxiter->fxhandle);
	}

	fxlist.clear();
}

static void ClearFXPlacedBuildingMap(FXPlacedBuildingMap &buildings, FXInterface *pFXInterface)
{
	FXPlacedBuildingMap::iterator bi = buildings.begin();
	FXPlacedBuildingMap::iterator be = buildings.end();

	for (; bi != be; bi++)
	{
		ClearFXPlaceList(bi->second.fxPlaceList, pFXInterface);
	}

	buildings.clear();
}

static bool IsControllerTypeBuilding(unsigned long ctype)
{
	bool r = false;

	switch (ctype)
	{
	case Lab_EC:
	case ResourceRenew_EC:
	case RemoteChamber_EC:
	case WaterChamber_EC:
	case Aviary_EC:
	case ElectricGenerator_EC:
	case BrambleFence_EC:
	case Foundry_EC:
	case SoundBeamTower_EC:
		r = true;
		break;

	default:
		r = false;
	}

	return r;
}

static bool IsControllerTypeUnit(unsigned long ctype)
{
	bool r = false;

	switch (ctype)
	{
	case Henchmen_EC:
		r = true;
		break;

	default:
		r = false;
	}

	return r;
}

static void DetermineModalCursor(char *cursor, size_t len, int &ttStrId, unsigned long mode, const RDNPlayer *player, const Entity *mouseOverEntity, const EntityGroup &selection)
{
	dbTracef("RDNTaskbar::DetermineModalCursor is probably not implemented properly");

	// init out parms
	strcpy(cursor, "default");
	ttStrId = 0;

	switch (mode)
	{
	case RDNTaskbar::MC_AttackMove:
	{
		strcpy(cursor, "modal_attack");
	}
	break;
	case RDNTaskbar::MC_Attack:
	{
		strcpy(cursor, "modal_attack_ground");
	}
	break;
	case RDNTaskbar::MC_Move:
	{
		strcpy(cursor, "modal_waypoints");
	}
	break;
	case RDNTaskbar::MC_Unload:
	{
		strcpy(cursor, "modal_unload");
	}
	break;
	case RDNTaskbar::MC_SetRallyPoint:
	{
		if (RDNQuery::CanRallyTo(mouseOverEntity, player))
		{
			strcpy(cursor, "modal_rallypoint");
		}
		else
		{
			strcpy(cursor, "modal_rallypoint_cancel");
		}
	}
	break;

	default:
	{
		// not handled
	}
	break;
	}

	return;
}

static const StateMove *GetStateMove(const Entity *pEntity)
{
	const StateMove *pMove = QIState<StateMove>(pEntity);
	if (!pMove)
	{
		// check for StateGroupMove
		const StateGroupMove *pGroupMove = QIState<StateGroupMove>(pEntity);
		if (pGroupMove)
		{
			pMove = static_cast<const StateMove *>(const_cast<StateGroupMove *>(pGroupMove)->GetSubState(State::SID_Move));
		}
	}

	return pMove;
}

static bool GetGroupWayPointPath(const EntityGroup &egroup, PointList &path)
{
	// init out parm
	path.clear();

	// check for empty group
	if (egroup.empty())
	{
		dbBreak();
		return false;
	}

	// make sure these things move
	const StateMove *pMove = GetStateMove(egroup.front());

	if (pMove == 0)
		return false;

	// retrieve the path
	const size_t numpoints = std::min(25U, pMove->GetNumWayPoints());

	path.reserve(numpoints + 2);
	path.assign(pMove->GetWayPoints(), pMove->GetWayPoints() + numpoints);

	//
	EntityGroup::const_iterator iter = egroup.begin();
	EntityGroup::const_iterator eiter = egroup.end();
	++iter;

	for (; iter != eiter; ++iter)
	{
		const StateMove *pMove = GetStateMove(*iter);

		if (pMove == NULL)
			break;

		size_t numpoints1 = path.size();
		size_t numpoints2 = std::min(25U, pMove->GetNumWayPoints());

		// Do a Reverse compare, i.e. start with the last waypoint and go to the first
		// we do this because an entity can pass through a waypoint before other
		// entities that share the same path, so we check for a common ending
		const Vec3f *pPoints1 = &path[0];
		size_t cmpPtIter1 = numpoints1;

		const Vec3f *pPoints2 = pMove->GetWayPoints();
		size_t cmpPtIter2 = numpoints2;

		bool bMismatch = false;
		while (cmpPtIter1 != 0 && cmpPtIter2 != 0)
		{
			cmpPtIter2--;
			cmpPtIter1--;

			if (pPoints1[cmpPtIter1] != pPoints2[cmpPtIter2])
			{
				bMismatch = true;
				break;
			}
		}

		if (bMismatch)
			break;

		// have we reached the start of the waypoint list ?
		if (cmpPtIter1 < cmpPtIter2)
		{
			while (cmpPtIter2 > 0)
			{
				cmpPtIter2--;
				path.insert(path.begin(), pPoints2[cmpPtIter2]);
			}
		}
	}

	if (iter == eiter)
	{
		return true;
	}

	path.clear();

	return false;
}

static void DestroyFXHandleList(FXInterface *pFX, FXPointList &fxlist)
{
	FXPointList::iterator iter = fxlist.begin();
	FXPointList::iterator eiter = fxlist.end();

	for (; iter != eiter; ++iter)
	{
		pFX->FXDestroy(*iter);
	}

	fxlist.clear();
}

/////////////////////////////////////////////////////////////////////
// RDNTaskbar

class RDNTaskbar::Data
{
public:
	LuaConfig *m_lua;

	RTSHud *m_hud;

	CameraInterface *m_camera;
	SelectionInterface *m_selection;
	UIInterface *m_ui;
	FXInterface *m_fx;

	RDNSimProxy *m_proxy;
	RDNUIProxy *m_uiproxy;
	const RDNGhost *m_pGhost;

	fstring<63> m_curtt;

	RDNInputBinder *m_pInputBinder;

	// lua
	std::vector<LuaBinding::Obj>
			m_exported;

	// modal UI
	std::string m_modalCBOk;
	std::string m_modalCBAbort;
	int m_modalParm;
	bool m_cursorOverride;
	char m_modalCursor[128];
	int m_modalTTStrId; // tool tip string id

	// rally stuff
	FXInterface::Handle m_rallyFxHandle;
	EntityGroup m_rallyGroup;

	Plat::InputKey m_commandQueueKey;
	fstring<63> m_commandQueueReleaseCB;
	bool m_commandQueueEnable;
	bool m_commandQueueModifier;
	int m_commandQueueCount; // the number of commands that can been queued since the shift key was pressed

	bool m_bBlockContextMouseTooltips;

	// at what time the timer was started should start at zero
	float m_startTimer;

	// the label bindings are needed because the text might actually
	// change every frame
	std::vector<BindingHud>
			m_boundHuds;

	// the button bindings are needed to dispatch the callback
	std::vector<BindingButton>
			m_boundButtons;

	//
	std::vector<BindingHotkey>
			m_boundKeys;

	// minimaps
	std::vector<RDNMiniMap *>
			m_minimaps;

	// binding
	BindingMode *m_binding;
	BindingModeScreen *m_bindingScreen;
	//
	bool m_dirtyButtons;

	// List of FX used to give feedback about placing a structure down
	FXPlaceList m_PlaceFXHandles;

	// List of FX used to give feedback about the footprint of existing structures that are close to where
	// a structure is to be placed
	FXPlacedBuildingMap m_PlacedBuildings;

	// button enable/disable support
	std::vector<int> m_disabledButtons;

	FXPointList m_WayPointFX;
	bool m_bUpdateWayPoint;

	// cached hotkey group states so that entities may be reinserted into hotkey groups
	// when certain events occur
	EntityGroup m_cachedHotkeyGroup[10];

	// use this to do certain updates once per simstep
	long m_LastGameTickUpdate;
};

/////////////////////////////////////////////////////////////////////
// RDNTaskbar

RDNTaskbar::RDNTaskbar(
		LuaConfig *lua,
		RTSHud *hud,
		CameraInterface *camera,
		SelectionInterface *selection,
		UIInterface *ui,
		FXInterface *fx,
		RDNSimProxy *proxy,
		RDNInputBinder *inputBinder,
		RDNUIProxy *uiproxy,
		const RDNGhost *pGhost)
		: m_pimpl(new Data)
{
	// init fields
	m_pimpl->m_lua = lua;

	m_pimpl->m_hud = hud;

	m_pimpl->m_camera = camera;
	m_pimpl->m_selection = selection;
	m_pimpl->m_ui = ui;
	m_pimpl->m_fx = fx;
	m_pimpl->m_proxy = proxy;
	m_pimpl->m_uiproxy = uiproxy;
	m_pimpl->m_pGhost = pGhost;

	// add tooltip system callback
	m_pimpl->m_hud->SetChildTooltipCB("RDNtaskbar", RTSHud::ToolTipCallback::Bind(this, &RDNTaskbar::OnChildToolTipCB));

	m_pimpl->m_pInputBinder = inputBinder;

	// binding
	m_pimpl->m_bindingScreen = new BindingModeScreen(m_pimpl->m_hud, &m_pimpl->m_boundHuds);
	m_pimpl->m_binding = m_pimpl->m_bindingScreen;

	m_pimpl->m_cursorOverride = false;

	m_pimpl->m_dirtyButtons = true;

	strcpy(m_pimpl->m_modalCursor, "default");
	m_pimpl->m_modalTTStrId = 0;

	m_pimpl->m_modalParm = 0;

	// command queueing
	m_pimpl->m_commandQueueKey = Plat::KEY_Shift;

	m_pimpl->m_commandQueueReleaseCB = "";
	m_pimpl->m_commandQueueEnable = false;
	m_pimpl->m_commandQueueModifier = false;
	m_pimpl->m_commandQueueCount = 0;

	m_pimpl->m_bBlockContextMouseTooltips = false;

	m_pimpl->m_bUpdateWayPoint = false;

	m_pimpl->m_startTimer = 0.0f;

	// preload
	Preload();

	// rally point
	m_pimpl->m_rallyFxHandle = m_pimpl->m_fx->FXCreate(k_RallyFXName);
	m_pimpl->m_fx->FXSetVisible(m_pimpl->m_rallyFxHandle, false);
	m_pimpl->m_fx->FXSetScale(m_pimpl->m_rallyFxHandle, 1.0f);
	m_pimpl->m_fx->FXSetLength(m_pimpl->m_rallyFxHandle, 1.0f);

	// init lua
	LuaSetup();

	// observe the events
	GameEventSys::Instance()->RegisterClient(this);

	// cached hotkey groups only care if the entities are deleted
	for (int gi = 0; gi < 10; gi++)
	{
		m_pimpl->m_cachedHotkeyGroup[gi].ClearFlag(EF_IsSpawned);
	}

	// init game tick
	m_pimpl->m_LastGameTickUpdate = 0;

	return;
}

RDNTaskbar::~RDNTaskbar()
{
	// unregister from events
	GameEventSys::Instance()->UnregisterClient(this);

	// shutdown lua
	LuaReset();

	DELETEZERO(m_pimpl->m_bindingScreen);

	std::vector<RDNMiniMap *>::iterator mmi = m_pimpl->m_minimaps.begin();
	std::vector<RDNMiniMap *>::iterator mme = m_pimpl->m_minimaps.end();
	for (; mmi != mme; ++mmi)
	{
		DELETEZERO(*mmi);
	}

	DELETEZERO(m_pimpl);

	return;
}

void RDNTaskbar::ButtonDispatch(const char *callback, int parm)
{
	if (strlen(callback) == 0)
		return;

	// callback into lua
	LuaBinding::Call<void> c;
	c.Execute(m_pimpl->m_lua, callback, parm);

	return;
}

void RDNTaskbar::ButtonCB(const std::string &str, Plat::InputKey mouseButton)
{
	// find bound button
	size_t i = 0;
	size_t e = m_pimpl->m_boundButtons.size();

	for (; i != e; ++i)
	{
		if (stricmp(str.c_str(), m_pimpl->m_boundButtons[i].hud.c_str()) == 0)
		{
			// skip buttons that have the clear flag set
			if (m_pimpl->m_boundButtons[i].bClear)
				return;

			break;
		}
	}

	// handle error
	if (i == e)
	{
		// oops!
		dbBreak();
		return;
	}

	// dispatch
	if (mouseButton == Plat::KEY_MouseLeft)
	{
		ButtonDispatch(
				m_pimpl->m_boundButtons[i].callback_left.c_str(),
				m_pimpl->m_boundButtons[i].parameter1);
	}
	else if (mouseButton == Plat::KEY_MouseRight)
	{
		ButtonDispatch(
				m_pimpl->m_boundButtons[i].callback_right.c_str(),
				m_pimpl->m_boundButtons[i].parameter1);
	}

	return;
}

void RDNTaskbar::ButtonLeftCB(const std::string &str)
{
	ButtonCB(str, Plat::KEY_MouseLeft);
}

void RDNTaskbar::ButtonRightCB(const std::string &str)
{
	ButtonCB(str, Plat::KEY_MouseRight);
}

void RDNTaskbar::RemoveClearedButtons(void)
{
	// remove and hide buttons marked for clearing
	std::vector<BindingButton>::iterator bi = m_pimpl->m_boundButtons.begin();
	std::vector<BindingButton>::iterator be = m_pimpl->m_boundButtons.end();

	m_pimpl->m_boundButtons.erase(std::remove_if(bi, be, RemoveAndHideClearedPredicate(m_pimpl->m_hud)), be);
}

void RDNTaskbar::Clear()
{
	// hide every visible control
	// labels
	std::vector<BindingHud>::iterator li = m_pimpl->m_boundHuds.begin();
	std::vector<BindingHud>::iterator le = m_pimpl->m_boundHuds.end();

	for (; li != le; ++li)
	{
		m_pimpl->m_hud->Show(CURRENTSCREEN, li->hud.c_str(), false);
	}

	m_pimpl->m_boundHuds.resize(0);

	// mark buttons for clearing, but don't actually clear them until
	// a call to RemoveClearedButtons()
	std::vector<BindingButton>::iterator bi = m_pimpl->m_boundButtons.begin();
	std::vector<BindingButton>::iterator be = m_pimpl->m_boundButtons.end();

	for (; bi != be; ++bi)
	{
		bi->bClear = true;
	}

	// hotkeyLuaNames
	m_pimpl->m_boundKeys.resize(0);

	return;
}

// when "hotkeyLuaName" is pressed it calls the callback for the left mouse button
// the params correspond to the parameters that the LUA callback function take
void RDNTaskbar::ButtonInternal(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback_left,
		const char *callback_right,
		const char *tooltipcb,
		const char *texture,
		bool enabled,
		int parm1,
		int parm2,
		int type)
{

	BindingButton bb;
	BindingButton *entry = &bb;
	bool alreadyExisted = false;

	// if button already bound then ignore
	for (size_t i = 0; i != m_pimpl->m_boundButtons.size(); i++)
	{
		if ((m_pimpl->m_boundButtons[i].hud == button))
		{
			if (!m_pimpl->m_boundButtons[i].bClear)
				return;
			alreadyExisted = true;
			entry = &m_pimpl->m_boundButtons[i];
			break;
		}
	}

	// fill out button
	entry->hud = button;
	entry->callback_left = callback_left;
	entry->callback_right = callback_right;
	entry->tooltip = tooltipcb;
	entry->parameter0 = enabled;
	entry->parameter1 = parm1;
	entry->parameter2 = parm2;
	entry->bClear = false;
	entry->type = BindingButton::BindType(type);
	entry->keyComboName = hotkeyLuaName;

	if (!alreadyExisted)
		m_pimpl->m_boundButtons.push_back(*entry);

	// set button texture
	if (strlen(texture) > 0)
	{
		m_pimpl->m_hud->SetTextureName(CURRENTSCREEN, button, texture);
	}

	// set button callback
	m_pimpl->m_hud->SetButtonCB(CURRENTSCREEN, button, RTSHud::ButtonCallback::Bind(this, &RDNTaskbar::ButtonLeftCB));
	m_pimpl->m_hud->SetButtonRightCB(CURRENTSCREEN, button, RTSHud::ButtonCallback::Bind(this, &RDNTaskbar::ButtonRightCB));

	// enable
	m_pimpl->m_hud->Enable(CURRENTSCREEN, button, enabled);

	// show hud
	m_pimpl->m_hud->Show(CURRENTSCREEN, button, true);

	//
	m_pimpl->m_dirtyButtons = true;

	return;
}

void RDNTaskbar::BindButton(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		const char *texture,
		int parm)
{
	if (!button)
		dbFatalf("RDNTaskbar::BindButton No button provided");

	if (!hotkeyLuaName)
		dbFatalf("RDNTaskbar::BindButton No hotkeyLuaName provided");

	if (!callback)
		dbFatalf("RDNTaskbar::BindButton No callback provided");

	if (!tooltipcb)
		dbFatalf("RDNTaskbar::BindButton No tooltipcb provided");

	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			texture,
			true,
			parm,
			0,
			BindingButton::BHUD_BUTTON_NORMAL);

	return;
}

void RDNTaskbar::BindButtonDisabled(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		const char *texture,
		int parm)
{
	// delegate
	ButtonInternal(button, hotkeyLuaName, callback, "", tooltipcb, texture, false, parm, 0, BindingButton::BHUD_BUTTON_NORMAL);

	return;
}

void RDNTaskbar::BindButtonToEntity(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		int entityId)
{
	// find ebp
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	if (e->GetControllerBP()->GetControllerType() != Henchmen_EC)
	{
		// units only
		dbBreak();
		return;
	}

	// texture
	const char *texture = e->GetControllerBP()->GetIconName();

	// enable?
	const bool enabled = true;

	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			texture,
			enabled,
			entityId,
			0,
			BindingButton::BHUD_BUTTON_SELECTENTITY);

	return;
}

void RDNTaskbar::BindButtonToBuildQueue(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		int building,
		int index,
		bool enabled)
{
	//
	const ControllerBlueprint *cbp = m_pimpl->m_proxy->BuildQueueAt(building, index);

	if (cbp == 0)
		// lag!
		return;

	// texture
	const char *texture = 0;

	if (m_pimpl->m_proxy->GetPlayer() == NULL)
	{
		texture = EnemyIconName;
	}
	else
	{
		texture = cbp->GetIconName();
	}

	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			texture,
			enabled,
			index,
			0,
			BindingButton::BHUD_BUTTON_NORMAL);

	return;
}

void RDNTaskbar::BindHudToTooltip(const char *hud, const char *tooltipcb, int parm0, int parm1)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_TOOLTIP;
	entry.hud = hud;
	entry.parameter0 = parm0;
	entry.parameter1 = parm1;
	entry.tooltip = tooltipcb;

	// store
	m_pimpl->m_binding->BindHud(entry);
}

void RDNTaskbar::CommandQueueEnable(const char *hotkeyLuaName, const char *releaseCallback)
{
	dbTracef("RDNTaskbar::CommandQueueEnable is probably not implemented properly");

	m_pimpl->m_commandQueueEnable = true;
	m_pimpl->m_commandQueueReleaseCB = releaseCallback;
	m_pimpl->m_commandQueueCount = 0;

	dbTracef("RDNTaskbar::CommandQueueEnable exiting");
}

void RDNTaskbar::EnableHud(EnableType type, bool enable)
{
	if (!enable)
	{
		// check for duplicate
		if (std::find(m_pimpl->m_disabledButtons.begin(), m_pimpl->m_disabledButtons.end(), type) == m_pimpl->m_disabledButtons.end())
		{
			m_pimpl->m_disabledButtons.push_back(type);
		}
	}
	else
	{
		// remove
		m_pimpl->m_disabledButtons.erase(std::remove(m_pimpl->m_disabledButtons.begin(), m_pimpl->m_disabledButtons.end(), type),
																		 m_pimpl->m_disabledButtons.end());
	}
}

bool RDNTaskbar::IsHudEnabled(unsigned long type)
{
	EnableType etype = static_cast<EnableType>(type);

	return std::find(m_pimpl->m_disabledButtons.begin(), m_pimpl->m_disabledButtons.end(), etype) == m_pimpl->m_disabledButtons.end();
}

void RDNTaskbar::Update(float elapsedSeconds)
{
	// remove buttons that have been hidden
	RemoveClearedButtons();

#pragma FIXME(find a better solution - for refreshing tooltips)
	m_pimpl->m_hud->RefreshTooltip(CURRENTSCREEN);

	// update all bound label text
	std::vector<BindingHud>::const_iterator i = m_pimpl->m_boundHuds.begin();
	std::vector<BindingHud>::const_iterator e = m_pimpl->m_boundHuds.end();

	const size_t hn = m_pimpl->m_boundHuds.size();

	for (; i != e; ++i)
	{
		BindingHud::BindingType type = i->type;

		switch (type)
		{
		case BindingHud::BHUD_LABEL_BUILDQUEUEPROGRESS:
			LabelUpdateBuildQueuePrg(i->hud.c_str(), i->parameter0);
			break;
		case BindingHud::BHUD_LABEL_ENTITYNAME:
			LabelUpdateEntityName(i->hud.c_str(), i->parameter1);
			break;
		case BindingHud::BHUD_LABEL_PLAYERCASH:
			LabelUpdatePlayerCash(i->hud.c_str(), i->parameter0);
			break;
		case BindingHud::BHUD_LABEL_PLAYERPOP:
			LabelUpdatePlayerPop(i->hud.c_str(), i->parameter0);
			break;
		case BindingHud::BHUD_LABEL_ENTITYHEALTH:
			LabelUpdateEntityHealth(i->hud.c_str(), i->parameter1);
			break;

		case BindingHud::BHUD_LABEL_RESOURCE:
			LabelUpdateResource(i->hud.c_str(), i->parameter0);
			break;

		case BindingHud::BHUD_BAR_ENTITYHEALTH:
			BarUpdateEntityHealth(i->hud.c_str(), i->parameter1);
			break;
		case BindingHud::BHUD_BAR_BUILDQUEUE:
			BarUpdateBuildQueue(i->hud.c_str(), i->parameter0);
			break;

		case BindingHud::BHUD_LABEL_GAMETIME:
			LabelUpdateGameTime(i->hud.c_str());

		case BindingHud::BHUD_LABEL_TEXTTIMER:
			break; // ignore for now
		case BindingHud::BHUD_TOOLTIP:
			break; // ignore for now

		case BindingHud::BHUD_LABEL_IMAGE: // fall-through
		case BindingHud::BHUD_LABEL_TEXT:
		case BindingHud::BHUD_LABEL_BUILDQUEUE:
		case BindingHud::BHUD_LABEL_EBPNAME:
		case BindingHud::BHUD_LABEL_EBPCOSTCASH:
		case BindingHud::BHUD_LABEL_EBPPREREQUISITE:
		case BindingHud::BHUD_ICON_EBPATTRIBUTE:
		case BindingHud::BHUD_LABEL_EBPATTRIBUTE:
		case BindingHud::BHUD_LABEL_EBPSPEED_LAND:
		case BindingHud::BHUD_LABEL_EBPSPEED_WATER:
		case BindingHud::BHUD_LABEL_EBPSPEED_AIR:
		case BindingHud::BHUD_LABEL_EBPSPEED_PUREWATER:
		case BindingHud::BHUD_LABEL_EBPABILITY:
		case BindingHud::BHUD_LABEL_EBPRANGEATTACK:
		case BindingHud::BHUD_ICON_EBPRANGEATTACK:

		case BindingHud::BHUD_LABEL_PLAYERNAME:
		case BindingHud::BHUD_LABEL_PLAYERCOLOUR:
			break;

		default:
			dbBreak();
		}

	} // next

	dbAssert(hn == m_pimpl->m_boundHuds.size());

	// update the timed labels
	// we do it separately 'cuz they modify the vector
	std::vector<BindingHud>::iterator ti = m_pimpl->m_boundHuds.begin();

	for (; ti != m_pimpl->m_boundHuds.end();)
	{
		if (ti->type == BindingHud::BHUD_LABEL_TEXTTIMER)
		{
			ti->secondDie -= elapsedSeconds;
			if (LabelUpdateTextTimer(ti->hud.c_str(), ti->secondDie))
			{
				ti->secondDie = 0;
				ti = m_pimpl->m_boundHuds.erase(ti);
			}
			else
			{
				ti++;
			}
		}
		else
		{
			ti++;
		}
	}

	// update the minimaps
	std::vector<RDNMiniMap *>::iterator mmi = m_pimpl->m_minimaps.begin();
	std::vector<RDNMiniMap *>::iterator mme = m_pimpl->m_minimaps.end();

	for (; mmi != mme; ++mmi)
	{
		(*mmi)->Update(elapsedSeconds);
	}

	// update the bound buttons
	std::vector<BindingButton>::iterator bi = m_pimpl->m_boundButtons.begin();
	std::vector<BindingButton>::iterator be = m_pimpl->m_boundButtons.end();

	const size_t bn = m_pimpl->m_boundButtons.size();

	for (; bi != be; ++bi)
	{
		switch (bi->type)
		{
		case BindingButton::BHUD_BUTTON_BUILDING:
			if (m_pimpl->m_dirtyButtons)
				ButtonUpdateBuilding(bi->hud.c_str(), bi->parameter1, bi->parameter2);
			break;
		case BindingButton::BHUD_BUTTON_UNIT:
			if (m_pimpl->m_dirtyButtons)
				ButtonUpdateUnit(bi->hud.c_str(), bi->parameter1, bi->parameter2);
			break;
		case BindingButton::BHUD_BUTTON_GROUP:
			ButtonUpdateGroup(bi->hud.c_str(), bi->parameter1, bi->parameter2);
			break;
		case BindingButton::BHUD_BUTTON_CHAT:
			ButtonUpdateChat(bi->hud.c_str(), bi->parameter1, bi->parameter2);
			break;
		case BindingButton::BHUD_BUTTON_SELECTENTITY:
			ButtonUpdateSelectEntity(bi->hud.c_str());
			break;

		case BindingButton::BHUD_BUTTON_NORMAL: // fall-through
			break;

		default:
			dbBreak();
		}
	}

	dbAssert(bn == m_pimpl->m_boundButtons.size());

	// dirty flag
	m_pimpl->m_dirtyButtons = false;

	if (m_pimpl->m_bUpdateWayPoint)
	{
		WayPointUpdate();
	}

	const World *w = m_pimpl->m_proxy->GetWorld();

	if (m_pimpl->m_LastGameTickUpdate != w->GetGameTicks())
	{
		// per simtick updates go in here

		// remember the last gametick
		m_pimpl->m_LastGameTickUpdate = w->GetGameTicks();

		// unselect hidden entities
		UnselectHiddenEntities();
	}

	return;
}

void RDNTaskbar::OnEvent(const GameEventSys::Event &ev)
{
	dbTracef("RDNTaskbar::OnEvent");
	/***
	// these should force an update of the taskbar
	if( ev.GetType() == GE_ConstructionComplete  )
	{
		m_pimpl->m_dirtyButtons = true;
	}
	else
***/
	if (ev.GetType() == GE_RallyPointSet)
	{
		const GameEvent_RallyPointSet &rps = static_cast<const GameEvent_RallyPointSet &>(ev);

		// only if the local player owns this structure, and
		// the structure is currently selected
		if (m_pimpl->m_proxy->GetPlayer() &&
				m_pimpl->m_proxy->GetPlayer()->CanControlEntity(rps.m_pBuilding))
		{
			const EntityGroup &currentSelection =
					m_pimpl->m_proxy->GetSelectionInterface()->GetSelection();

			if (currentSelection.find(rps.m_pBuilding) != currentSelection.end())
			{
				// update rally point fx
				RallyPointShow(rps.m_pBuilding->GetID());
			}
		}
	}

	return;
}

void RDNTaskbar::LuaSetup()
{
#define BIND(f) \
	m_pimpl->m_exported.push_back(LuaBinding::Bind(m_pimpl->m_lua, #f, this, &RDNTaskbar::f))

	// these are the bindings that actually get used by taskbar.lua
	BIND(Clear);

	BIND(PreloadTexture);

	BIND(CreateMinimap);

	BIND(BindLabelToPlayerName);
	BIND(BindLabelToPlayerCash);
	BIND(BindLabelToPlayerPop);
	BIND(BindLabelToPlayerColour);
	BIND(BindLabelToEntityName);
	BIND(BindLabelToEntityHealth);
	BIND(BindLabelToText);
	BIND(BindLabelToTooltip);
	BIND(BindLabelToHotkey);
	BIND(BindLabelToTextTimer);
	BIND(BindLabelToGameTime);

	BIND(BindLabelToBuildQueue);
	BIND(BindLabelToBuildProgress);
	BIND(BindLabelToEBPName);
	BIND(BindLabelToEBPCostCash);
	BIND(BindLabelToResource);

	BIND(BindImageToTexture);
	BIND(BindImageToEntityIcon);

	BIND(BindButton);
	BIND(BindButtonDisabled);
	BIND(BindButtonToBuildingEBP);
	BIND(BindButtonToUnitEBP);
	BIND(BindButtonToBuildQueue);
	BIND(BindButtonToEntity);
	BIND(BindButtonToGroup);
	BIND(BindButtonToChat);
	BIND(BindHudToTooltip);

	BIND(BindBarToEntityHealth);
	BIND(BindBarToBuildQueue);

	BIND(BindHotkey);

	BIND(ShowBitmapLabel);
	BIND(ShowHud);

	BIND(BuildUIBegin);
	BIND(BuildUIEnd);

	BIND(ModalUIBegin);
	BIND(ModalUIEnd);

	BIND(RallyPointShow);
	BIND(RallyPointHide);

	BIND(WayPointPathShow);
	BIND(WayPointPathHide);

	BIND(CommandQueueEnable);
	BIND(ModalCommandQueueRequest);

	BIND(IsSelectSimilarPressed);
	BIND(IsSelectSinglePressed);

	BIND(IsHudEnabled);

	BIND(GetModalCommandMode);

	BIND(HelpTextTitle);
	BIND(HelpTextShortcut);
	BIND(HelpTextTextWithoutRequirements);
	BIND(HelpTextChat);

	BIND(TypeFromEBP);
#undef BIND

#define BINDINNERCONSTANT(t, c) \
	m_pimpl->m_lua->SetNumber(#c, double(t::c))

	BINDINNERCONSTANT(RTSHud, UATTR_Health);
	BINDINNERCONSTANT(RTSHud, UATTR_Armor);
	BINDINNERCONSTANT(RTSHud, UATTR_Speed);
	BINDINNERCONSTANT(RTSHud, UATTR_Sight);
	BINDINNERCONSTANT(RTSHud, UATTR_Size);
	BINDINNERCONSTANT(RTSHud, UATTR_Melee);

	BINDINNERCONSTANT(RDNTaskbar, MC_Move);
	BINDINNERCONSTANT(RDNTaskbar, MC_Attack);
	BINDINNERCONSTANT(RDNTaskbar, MC_AttackMove);
	BINDINNERCONSTANT(RDNTaskbar, MC_SetRallyPoint);
	BINDINNERCONSTANT(RDNTaskbar, MC_Unload);

	BINDINNERCONSTANT(RDNTaskbar, ENABLE_HenchmanKill);
	BINDINNERCONSTANT(RDNTaskbar, ENABLE_HenchmanBuild);
	BINDINNERCONSTANT(RDNTaskbar, ENABLE_HenchmanAdvancedBuild);

#undef BINDINNERCONSTANT
	return;
}

void RDNTaskbar::LuaReset()
{
	m_pimpl->m_exported.clear();
}

void RDNTaskbar::CreateMinimap(const char *label)
{
	// create new minimap
	MiniMap *mm = m_pimpl->m_hud->CreateMiniMap(CURRENTSCREEN, label);

	if (mm == 0)
		return;

	// create the new minimap controller and pass it the interface it should use
	RDNMiniMap *smm = new RDNMiniMap(mm, m_pimpl->m_proxy->GetPlayer(), m_pimpl->m_camera, m_pimpl->m_selection, m_pimpl->m_pGhost);

	m_pimpl->m_minimaps.push_back(smm);

	return;
}

void RDNTaskbar::LabelUpdateBuildQueuePrg(const char *label, int parm)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = const_cast<EntityFactory *>(ef)->GetEntityFromEID(parm);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
	{
		dbBreak();
		return;
	}

	const int percent =
			int(spawner->UnitInProgress().second * 100.0f);

	// find owner
	const RDNPlayer *owner =
			static_cast<const RDNPlayer *>(e->GetOwner());

	//
	wchar_t wchbuf[256] = L"";

	// check pop cap
	if (percent == 0 && owner && owner->PopulationTotal() >= owner->PopulationMax())
	{
		Localizer::GetString(wchbuf, LENGTHOF(wchbuf), TBAR_POPCAPREACHED);
	}
	else if (spawner->IsBlocked())
	{
		// the spawner is blocked

		Localizer::GetString(wchbuf, LENGTHOF(wchbuf), TBAR_SPAWNLOCATIONOCCUPIED);
	}
	else
	{
		// display progress %

		wchar_t n[256];
		Localizer::ConvertNumber2Localized(n, LENGTHOF(n), percent);

		Localizer::FormatText(wchbuf, LENGTHOF(wchbuf), TBAR_BUILDQUEUEPROGRESS, n);
	}

	// update text
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wchbuf);

	return;
}

void RDNTaskbar::LabelUpdatePlayerCash(const char *label, int parm)
{
	//
	const RDNPlayer *player =
			static_cast<const RDNPlayer *>(m_pimpl->m_proxy->GetWorld()->GetPlayerFromID(parm));

	if (player == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	// text
	const int cur = int(floorf(player->GetResourceCash()));
	wchar_t curW[16];
	Localizer::ConvertNumber2Localized(curW, LENGTHOF(curW), cur);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, curW);

	return;
}

void RDNTaskbar::LabelUpdatePlayerPop(const char *label, int parm)
{
	//
	const RDNPlayer *player =
			static_cast<const RDNPlayer *>(m_pimpl->m_proxy->GetWorld()->GetPlayerFromID(parm));

	if (player == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	//
	const int cur = player->PopulationCurrent();
	const int max = player->PopulationMax();

	wchar_t curW[16];
	Localizer::ConvertNumber2Localized(curW, LENGTHOF(curW), cur);

	wchar_t maxW[16];
	Localizer::ConvertNumber2Localized(maxW, LENGTHOF(maxW), max);

	// text
	wchar_t wchbuf[256];
	Localizer::FormatText(wchbuf, LENGTHOF(wchbuf), TBAR_POPULATION, curW, maxW);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wchbuf);

	return;
}

void RDNTaskbar::LabelUpdateGameTime(const char *label)
{
	int seconds = static_cast<int>(m_pimpl->m_proxy->GetWorld()->GetGameTime() - m_pimpl->m_startTimer);

	const int nhrs = seconds / (60 * 60);
	seconds -= nhrs * 60 * 60;
	const int nmins = seconds / 60;
	seconds -= nmins * 60;
	const int nsecs = seconds;

	wchar_t hrs[8];
	swprintf(hrs, L"%02d", nhrs);

	wchar_t min[8];
	swprintf(min, L"%02d", nmins);

	wchar_t sec[8];
	swprintf(sec, L"%02d", nsecs);

	wchar_t timebuf[256];
	Localizer::FormatText(timebuf, LENGTHOF(timebuf), TBAR_TIME, hrs, min, sec);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, timebuf);

	return;
}

void RDNTaskbar::LabelUpdateEntityHealth(const char *label, int entityId)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	//
	const HealthExt *h = QIExt<HealthExt>(e);

	//
	wchar_t wchbuf[256] = L"";

	if (h != 0)
	{
		const int healthCurrent = int(__max(1.0f, h->GetHealth()));
		const int healthMax = int(__max(1.0f, h->GetHealthMax()));

		// text
		wchar_t hc[12];
		Localizer::ConvertNumber2Localized(hc, LENGTHOF(hc), healthCurrent);

		wchar_t hm[12];
		Localizer::ConvertNumber2Localized(hm, LENGTHOF(hm), healthMax);

		Localizer::FormatText(wchbuf, LENGTHOF(wchbuf), TBAR_HEALTH, hc, hm);
	}

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wchbuf);

	return;
}

bool RDNTaskbar::IsCommandQueueKeyPressed()
{
	return Plat::Input::IsKeyPressed(m_pimpl->m_commandQueueKey);
}

bool RDNTaskbar::ModalCommandQueueRequest()
{
	if (m_pimpl->m_commandQueueEnable)
	{
		bool bQueue = IsCommandQueueKeyPressed();
		if (bQueue)
		{
			m_pimpl->m_commandQueueCount++;
		}
		return (bQueue);
	}
	else
	{
		return (false);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//

void RDNTaskbar::OnChildToolTipCB(const std::string &hud, bool show)
{
	// tooltip is about to be shown
	if (show)
	{
		// block context mouse from clearing the tooltips
		// when any HUD is displaying a tooltip
		m_pimpl->m_bBlockContextMouseTooltips = true;

		// find tooltip callback associated with the hud
		fstring<31> tooltipcb;
		int parm0, parm1, parm2;

		const bool ttOk = GetTooltipCBAndParms(
				tooltipcb, // out parm
				parm0,		 // out parm
				parm1,		 // out parm
				parm2,		 // out parm
				hud,
				m_pimpl->m_boundButtons,
				m_pimpl->m_boundHuds);

		if (ttOk == 0 || tooltipcb.size() == 0)
			return;

		// call tooltip function
		LuaBinding::Call<void> c;
		if (c.Execute(m_pimpl->m_lua, tooltipcb.c_str(), parm0, parm1, parm2, 0))
		{
		}
	}
	// tooltip is being hidden
	else
	{
		// allow context mouse to set tooltips
		m_pimpl->m_bBlockContextMouseTooltips = false;
	}

	return;
}

bool RDNTaskbar::IsSelectSimilarPressed()
{
	return Plat::Input::IsKeyPressed(Plat::KEY_Shift);
}

bool RDNTaskbar::IsSelectSinglePressed()
{
	return Plat::Input::IsKeyPressed(Plat::KEY_Control);
}

bool RDNTaskbar::Input(const Plat::InputEvent &ie)
{
	if (m_pimpl->m_commandQueueEnable &&
			(ie.type == Plat::IET_KeyRelease) &&
			(ie.key == m_pimpl->m_commandQueueKey))
	{
		// call back into lua to indicate the queue combo was released
		if (m_pimpl->m_commandQueueReleaseCB.size() > 0 && (m_pimpl->m_commandQueueCount > 0))
		{
			LuaBinding::Call<void> c;
			c.Execute(m_pimpl->m_lua, m_pimpl->m_commandQueueReleaseCB.c_str());

			m_pimpl->m_commandQueueEnable = false;
		}
	}

	// check the button hotkeyLuaNames
	if (ie.type == Plat::IET_KeyPress)
	{
		// NOTE: we reverse iterate over the containers so later code
		// can override older one

		size_t bestMatchPriority = 0;

		// check buttons
		std::vector<BindingButton>::reverse_iterator bi = m_pimpl->m_boundButtons.rbegin();
		std::vector<BindingButton>::reverse_iterator be = m_pimpl->m_boundButtons.rend();
		std::vector<BindingButton>::reverse_iterator bbest = m_pimpl->m_boundButtons.rend();

		for (; bi != be; ++bi)
		{
			// skip buttons that have the clear flag set
			if ((*bi).bClear)
				continue;

			// skip buttons that are disabled
			if (!(m_pimpl->m_hud->IsEnabled(CURRENTSCREEN, (*bi).hud.c_str())))
				continue;

			size_t matchPriority = m_pimpl->m_pInputBinder->IsComboKeyPressed((*bi).keyComboName.c_str(), &ie);
			if (matchPriority > bestMatchPriority)
			{
				bestMatchPriority = matchPriority;
				bbest = bi;
			}
		}

		// check hotkeyLuaNames
		std::vector<BindingHotkey>::reverse_iterator ki = m_pimpl->m_boundKeys.rbegin();
		std::vector<BindingHotkey>::reverse_iterator ke = m_pimpl->m_boundKeys.rend();
		std::vector<BindingHotkey>::reverse_iterator kbest = m_pimpl->m_boundKeys.rend();

		for (; ki != ke; ++ki)
		{
			size_t matchPriority = m_pimpl->m_pInputBinder->IsComboKeyPressed((*ki).keyComboName.c_str(), &ie);
			if (matchPriority > bestMatchPriority)
			{
				bestMatchPriority = matchPriority;
				kbest = ki;
				bbest = be; //	since we beat the button priority
			}
		}

		if (bbest != be)
		{
			ButtonDispatch(
					(*bbest).callback_left.c_str(),
					(*bbest).parameter1);
			return true;
		}

		if (kbest != ke)
		{
			ButtonDispatch(
					(*kbest).callback.c_str(),
					(*kbest).parameter0);
			return true;
		}
	}

	return false;
}

int RDNTaskbar::BuildUIBegin(const char *callbackOk, const char *callbackAbort, int ebpid)
{
	// validate player
	if (m_pimpl->m_proxy->GetPlayer() == 0 ||
			m_pimpl->m_proxy->GetPlayer()->IsPlayerDead())
	{
		dbFatalf("RDNTaskbar::BuildUIBegin the player is dead or does not exist");
		return RDNSimProxy::FC_Other;
	}

	// validate parm
	if (callbackOk == 0 || strlen(callbackOk) == 0 ||
			callbackAbort == 0 || strlen(callbackAbort) == 0)
	{
		dbFatalf("RDNTaskbar::BuildUIBegin callbackOk or callbackAbort missing");
		return RDNSimProxy::FC_Other;
	}

	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		dbFatalf("RDNTaskbar::BuildUIBegin could not find an ebp for net id %d", ebpid);
		return RDNSimProxy::FC_Other;
	}

	// quick validation
	const int ok = m_pimpl->m_proxy->ValidateBuildUI(cbp);

	if (ok != RDNSimProxy::FC_Ok)
	{
		dbFatalf("RDNTaskbar::BuildUIBegin could not validate build ui. result: %d", ok);
		return ok;
	}

	// store
	m_pimpl->m_modalCBOk = callbackOk;
	m_pimpl->m_modalCBAbort = callbackAbort;
	m_pimpl->m_modalParm = ebpid;
	m_pimpl->m_cursorOverride = true;
	strcpy(m_pimpl->m_modalCursor, "default");
	m_pimpl->m_modalTTStrId = 0;

	// use fence ui?
	bool bFence = false;

	// change sm
	if (bFence)
	{
		m_pimpl->m_ui->ModalUIStart(
				cbp,
				UIInterface::ModalCBAbort::Bind(this, &RDNTaskbar::ModalUiCBAbort),
				UIInterface::ModalCBTwoClick::Bind(this, &RDNTaskbar::ModalUiCBTwoClick),
				UIInterface::ModalCBPlaceFenceUpdate::Bind(this, &RDNTaskbar::ModalUiCBPlaceFence));
	}
	else
	{
		m_pimpl->m_ui->ModalUIStart(
				cbp,
				UIInterface::ModalCBAbort::Bind(this, &RDNTaskbar::ModalUiCBAbort),
				UIInterface::ModalCBClick::Bind(this, &RDNTaskbar::ModalUiCBClick),
				UIInterface::ModalCBPlaceEntityUpdate::Bind(this, &RDNTaskbar::ModalUiCBPlaceEntity));
	}

	return RDNSimProxy::FC_Ok;
}

void RDNTaskbar::BuildUIEnd()
{
	// reset fields
	m_pimpl->m_modalCBOk = "";
	m_pimpl->m_modalCBAbort = "";
	m_pimpl->m_modalParm = 0;
	m_pimpl->m_cursorOverride = false;
	m_pimpl->m_commandQueueEnable = false;

	// change sm
	m_pimpl->m_ui->ModalUIStop();

	// clear any can't place fx's
	ClearFXPlaceList(m_pimpl->m_PlaceFXHandles, m_pimpl->m_fx);

	// clear any placed-building fx's
	ClearFXPlacedBuildingMap(m_pimpl->m_PlacedBuildings, m_pimpl->m_fx);

	return;
}

//zzz - add ebpid to the callback click button instead of just xyz

int RDNTaskbar::ModalUIBegin(const char *callbackOk, const char *callbackAbort, int mode, int command)
{
	dbTracef("RDNTaskbar::ModalUIBegin");

	// validate player
	if (m_pimpl->m_proxy->GetPlayer() == 0 ||
			m_pimpl->m_proxy->GetPlayer()->IsPlayerDead())
	{
		dbBreak();
		return RDNSimProxy::FC_Other;
	}

	// validate parm
	if (callbackOk == 0 || strlen(callbackOk) == 0 ||
			callbackAbort == 0 || strlen(callbackAbort) == 0)
	{
		dbBreak();
		return RDNSimProxy::FC_Other;
	}

	// store
	m_pimpl->m_modalCBOk = callbackOk;
	m_pimpl->m_modalCBAbort = callbackAbort;
	m_pimpl->m_modalParm = command;
	m_pimpl->m_cursorOverride = true; // cursor will come from m_pimpl->m_modalCursor

	// Capture clicks in the MiniMap
	if (!m_pimpl->m_minimaps.empty())
	{
		m_pimpl->m_minimaps.front()->SetModalClickCapture(true);
	}

	// zzz set proxy modal mode so it shows the proper cursor

	// change sm
	m_pimpl->m_ui->ModalUIStart(
			mode,		 // MM_Cursor, or MM_LockCursor
			command, // MC_Attack, etc.
			UIInterface::ModalCBAbort::Bind(this, &RDNTaskbar::ModalUiCBAbort),
			UIInterface::ModalCBClick::Bind(this, &RDNTaskbar::ModalUiCBClick),
			UIInterface::ModalCBCursorUpdate::Bind(this, &RDNTaskbar::ModalUiCBCursorUpdate));

	return RDNSimProxy::FC_Ok;
}

void RDNTaskbar::ModalUIReset()
{
	// reset fields
	m_pimpl->m_modalCBOk = "";
	m_pimpl->m_modalCBAbort = "";
	m_pimpl->m_modalParm = 0;
	m_pimpl->m_cursorOverride = false;
	m_pimpl->m_commandQueueEnable = false;

	//
	m_pimpl->m_ui->ModalUIStop();

	// Capture clicks in the MiniMap
	if (!m_pimpl->m_minimaps.empty())
	{
		m_pimpl->m_minimaps.front()->SetModalClickCapture(false);
	}

	// clear any can't place fx's
	ClearFXPlaceList(m_pimpl->m_PlaceFXHandles, m_pimpl->m_fx);

	// clear any placed-building fx's
	ClearFXPlacedBuildingMap(m_pimpl->m_PlacedBuildings, m_pimpl->m_fx);

	return;
}

void RDNTaskbar::ModalUIEnd()
{
	// validate object state
	if (m_pimpl->m_modalCBOk.empty())
		// NOTE: this is an error
		return;

	// reset fields
	m_pimpl->m_modalCBOk = "";
	m_pimpl->m_modalCBAbort = "";
	m_pimpl->m_modalParm = 0;
	m_pimpl->m_cursorOverride = false;
	m_pimpl->m_commandQueueEnable = false;

	// change sm
	m_pimpl->m_ui->ModalUIStop();

	// Capture clicks in the MiniMap
	if (!m_pimpl->m_minimaps.empty())
	{
		m_pimpl->m_minimaps.front()->SetModalClickCapture(false);
	}

	// clear any can't place fx's
	ClearFXPlaceList(m_pimpl->m_PlaceFXHandles, m_pimpl->m_fx);

	// clear any placed-building fx's
	ClearFXPlacedBuildingMap(m_pimpl->m_PlacedBuildings, m_pimpl->m_fx);

	return;
}

void RDNTaskbar::ModalUiCBAbort()
{
	// validate object state
	dbAssert(!m_pimpl->m_modalCBAbort.empty());

	// callback into lua
	LuaBinding::Call<void> c;
	c.Execute(m_pimpl->m_lua, m_pimpl->m_modalCBAbort.c_str(), m_pimpl->m_modalParm);

	return;
}

void RDNTaskbar::ModalUiCBClick(Vec3f v, int ebpid)
{
	// validate object state
	dbAssert(!m_pimpl->m_modalCBOk.empty());

	// callback into lua
	LuaBinding::Call<void> c;
	c.Execute(
			m_pimpl->m_lua,
			m_pimpl->m_modalCBOk.c_str(),
			m_pimpl->m_modalParm,
			v.x,
			v.y,
			v.z,
			ebpid);

	return;
}

void RDNTaskbar::ModalUiCBTwoClick(Vec3f v1, Vec3f v2, int ebpid)
{

	// validate object state
	dbAssert(!m_pimpl->m_modalCBOk.empty());

	// callback into lua
	LuaBinding::Call<void> c;
	c.Execute(
			m_pimpl->m_lua,
			m_pimpl->m_modalCBOk.c_str(),
			m_pimpl->m_modalParm,
			v1.x,
			v1.y,
			v1.z,

			v2.x,
			v2.y,
			v2.z,

			ebpid);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: callback used to determine if we can place an entity/building
//	Result	:
//	Param.	: position - the original position, which will be snapped to the grid
//			  bCanPlace - bool indicating if the position is valid
//			  cbp - the entity we are placing
//	Author	: dswinerd
//
static int RoundToNearestNum(float num, int multiple)
{
	long result = abs(num) + multiple / 2;
	result -= result % multiple;
	result *= num > 0 ? 1 : -1;
	return result;
}

void RDNTaskbar::ModalUiCBPlaceEntity(Matrix43f &position, bool &bCanPlace, const ControllerBlueprint *cbp, bool bRender) const
{
	// TODO: this snap to grid implementation could use some work. The snaps in real ic are bigger.
	position.T.x = RoundToNearestNum(position.T.x, 3);
	position.T.z = RoundToNearestNum(position.T.z, 3);
	dbTracef("Adjusted cursor position: x: %g, z: %g", position.T.x, position.T.z);

	const ECStaticInfo *si = ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);
	if (!si)
		dbFatalf("RDNTaskbar::ModalUiCBPlaceEntity no static info for cbp %s", cbp->GetFileName());

	const SiteExtInfo *siteExtInfo = QIExtInfo<SiteExtInfo>(si);
	if (!siteExtInfo)
		dbFatalf("RDNTaskbar::ModalUiCBPlaceEntity no site info for cbp type %s", cbp->GetFileName());

	dbTracef("RDNTaskbar::ModalUiCBPlaceEntity location type for cbp %s is %d",
					 cbp->GetFileName(),
					 siteExtInfo->canPlaceType);

	TerrainHMBase *terrain = ModObj::i()->GetWorld()->GetTerrain();
	TerrainHMBase::TerrainType terrainType = terrain->GetTerrainCellType(position.T.x, position.T.z);
	dbTracef("RDNTaskbar::ModalUiCBPlaceEntity got terrain type %d", terrainType);

	if (!siteExtInfo->CanPlaceOnTerrain(terrainType))
	{
		bCanPlace = false;
		return;
	}

	// TODO: check if the place is impassable
	const TerrainCellMap *tcmap = ModObj::i()->GetWorld()->GetPathfinder()->GetTerrainCellMap();
	long cellSize = (long)tcmap->GetCellSize();
	long width, height;
	BuildingDynamics::GetEntityWidthHeight(cbp, &position, width, height);
	dbTracef("Width %d, height: %d", width, height);

	switch (terrainType)
	{
	case TerrainHMBase::eLand:
	{
		bool isLandImpassable = tcmap->TestCells(position.T.x, position.T.z, width, height, eLandImpassible);
		if (isLandImpassable)
		{
			dbTracef("Land is impassable");
			bCanPlace = false;
			return;
		}

		dbTracef("Land is not impassable");
		break;
	}
	case TerrainHMBase::eWater:
	{
		bool isWaterImpassable = tcmap->TestCells(position.T.x, position.T.z, width, height, eWaterImpassible);
		if (isWaterImpassable)
		{
			dbTracef("Water is impassable");
			bCanPlace = false;
			return;
		}

		dbTracef("Water is not impassable");
		break;
	}
	default:
		bCanPlace = false;
		dbFatalf("RDNTaskbar::ModalUiCBPlaceEntity unhandled terrain type");
		return;
	}

	bCanPlace = true;
	// const TerrainCell terrainCell = tcmap->GetCell(position.T.x, position.T.z);
	// bool isLand = TCIsLand(terrainCell);

	// if (isLand)
	// {
	// 	dbTracef("It is land impassable");
	// 	bCanPlace = false;
	// 	return;
	// }

	// else
	// {
	// 	dbTracef("It is not land impassable");
	// }

	// dbTracef("RDNTaskbar::ModalUiCBPlaceEntity is position land impassable? %s", TCIsLand(terrainCell) ? 'yes' : 'no');

	// TCMask test = eLandImpassible | eWaterImpassible | eAmphibianImpassible;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: callback used to determine positions of fence segments
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void RDNTaskbar::ModalUiCBPlaceFence(const Vec3f &pos1,
																		 const Vec3f &pos2,
																		 std::vector<Matrix43f> &transList,
																		 std::vector<bool> &canPlaceList,
																		 int &canAfford,
																		 const ControllerBlueprint *cbp) const
{
	dbFatalf("RDNTaskbar::ModalUiCBPlaceFence not yet implemented");
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void RDNTaskbar::ModalUiCBCursorUpdate(unsigned long userData, Vec3f &position, bool &bValidIntersection, Entity *mouseOverEntity)
{
	ModalCommands mode = static_cast<ModalCommands>(userData);

	// get the current selection
	const EntityGroup &currentSelection = m_pimpl->m_proxy->GetSelection();
	dbAssert(!currentSelection.empty());

	if (mode == MC_SetRallyPoint)
	{
		m_pimpl->m_rallyGroup.clear();
		if (mouseOverEntity)
		{ // set the position of the effect on top of the mouseover entity
			m_pimpl->m_rallyGroup.push_back(mouseOverEntity);
		}
		else if (bValidIntersection)
		{ // just set the position of the effect at the cursor
			RallyPointSetPosition(position, true);
		}
		else
		{ // mouse was over the sky or something, don't move the rally point there!
			return;
		}
	}

	DetermineModalCursor(
			m_pimpl->m_modalCursor,
			LENGTHOF(m_pimpl->m_modalCursor),
			m_pimpl->m_modalTTStrId,
			mode,
			m_pimpl->m_proxy->GetPlayer(),
			mouseOverEntity,
			currentSelection);

	return;
}

void RDNTaskbar::BindImageToEntityIcon(const char *label, int entityId, const char *tooltipcb, int parm)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	BindingHud entry;
	entry.hud = label;
	entry.tooltip = tooltipcb;
	entry.parameter0 = parm;
	entry.type = entry.BHUD_LABEL_IMAGE;

	//
	const char *texture = 0;

	texture = e->GetControllerBP()->GetIconName();

	// set texture
	m_pimpl->m_hud->SetTextureName(CURRENTSCREEN, label, texture);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindBarToEntityHealth(const char *bar, int entityId, const char *tooltipcb, int parm)
{
	// validate
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	//
	const HealthExt *h = QIExt<HealthExt>(e);

	if (h == 0)
	{
		// the flyers don't have health -- ignore
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_BAR_ENTITYHEALTH;
	entry.hud = bar;
	entry.tooltip = tooltipcb;
	entry.parameter0 = parm;
	entry.parameter1 = entityId;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BarUpdateEntityHealth(const char *bar, int parm)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(parm);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	const HealthExt *h = QIExt<HealthExt>(e);

	if (h == 0)
	{
		dbBreak();
		return;
	}

	//
	float len =
			__max(0.1f, h->GetHealth() / h->GetHealthMax());

	Colour colour;

	if (len < 0.25f)
		colour = Colour::Red;
	else if (len < 0.50f)
		colour = unsigned long(Colour::Red | Colour::Green);
	else
		colour = Colour::Green;

	m_pimpl->m_hud->SetBarLength(CURRENTSCREEN, bar, len);
	m_pimpl->m_hud->SetBarColour(CURRENTSCREEN, bar, colour);

	return;
}

void RDNTaskbar::BindBarToBuildQueue(const char *bar, int buildingId)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_BAR_BUILDQUEUE;
	entry.hud = bar;
	entry.parameter0 = buildingId;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BarUpdateBuildQueue(const char *bar, int parm)
{
	//
	const float l = m_pimpl->m_proxy->BuildQueueBar(parm);

	//
	m_pimpl->m_hud->SetBarLength(CURRENTSCREEN, bar, l);

	return;
}

void RDNTaskbar::BindHotkey(const char *hotkeyLuaName, const char *callback, int parm)
{
	BindingHotkey entry;
	entry.callback = callback;
	entry.parameter0 = parm;

	entry.keyComboName = hotkeyLuaName;

	m_pimpl->m_boundKeys.push_back(entry);

	return;
}

void RDNTaskbar::BindLabelToPlayerName(const char *label, int playerId)
{
	// locate player
	const Player *p = m_pimpl->m_proxy->GetWorld()->GetPlayerFromID(playerId);

	// quick-out
	if (p == 0)
		return;

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_PLAYERNAME;
	entry.hud = label;
	entry.parameter0 = playerId;

	// set text
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, p->GetName());

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToPlayerColour(const char *label, int playerId)
{
	// locate player
	const Player *p = m_pimpl->m_proxy->GetWorld()->GetPlayerFromID(playerId);

	// quick-out
	if (p == 0)
		return;

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_PLAYERCOLOUR;
	entry.hud = label;
	entry.parameter0 = playerId;

	// set colour
	m_pimpl->m_hud->SetLabelPlayerColour(CURRENTSCREEN, label, p);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToEntityName(const char *label, int entityId, const char *tooltipcb, int parm)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_ENTITYNAME;
	entry.hud = label;
	entry.tooltip = tooltipcb;
	entry.parameter0 = parm;
	entry.parameter1 = entityId;

	// not tagged
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, e->GetControllerBP()->GetScreenName());

	// set text colour
	m_pimpl->m_hud->SetLabelTextPlayerColour(CURRENTSCREEN, label, e->GetOwner());

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::LabelUpdateEntityName(const char *label, int entityId)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	// not tagged
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, e->GetControllerBP()->GetScreenName());

	return;
}

void RDNTaskbar::BindLabelToTextTimer(const char *label, int textId, float seconds)
{
	// validate parm
	if (seconds <= 0.0f)
	{
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_TEXTTIMER;
	entry.hud = label;
	entry.secondDie = seconds;

	// set text
	wchar_t wch[512];
	Localizer::GetString(wch, LENGTHOF(wch), textId);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wch);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToText(const char *label, int textId)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_TEXT;
	entry.hud = label;

	// set text
	wchar_t wch[512];
	Localizer::GetString(wch, LENGTHOF(wch), textId);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wch);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToTooltip(const char *label, const char *tooltipcb)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_TEXT;
	entry.hud = label;
	entry.tooltip = tooltipcb;

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, L"");
	m_pimpl->m_hud->Show(CURRENTSCREEN, label, true);

	// store
	m_pimpl->m_binding->BindHud(entry);
}

void RDNTaskbar::BindLabelToHotkey(const char *label, const char *hotkeyLuaName)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_TEXT;
	entry.hud = label;

	// set text
	wchar_t wkey[256];
	Localizer::String2LocString(wkey, LENGTHOF(wkey), hotkeyLuaName);

	wchar_t wch[512];
	Localizer::FormatText(wch, LENGTHOF(wch), TBAR_HOTKEY, wkey);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, wch);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToBuildQueue(const char *label, int building, int index)
{
	//
	const ControllerBlueprint *cbp = m_pimpl->m_proxy->BuildQueueAt(building, index);

	if (cbp == 0)
		// lag!
		return;

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_BUILDQUEUE;
	entry.hud = label;

	// set text
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, cbp->GetScreenName());

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindImageToTexture(const char *label, const char *texture)
{
	//
	BindingHud entry;
	entry.hud = label;
	entry.type = entry.BHUD_LABEL_IMAGE;

	// set texture
	m_pimpl->m_hud->SetTextureName(CURRENTSCREEN, label, texture);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToPlayerCash(const char *label, const char *tooltipcb, int index, int playerId)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_PLAYERCASH;
	entry.hud = label;
	entry.parameter0 = playerId;
	entry.parameter1 = index;
	entry.tooltip = tooltipcb;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToBuildProgress(const char *label, int building)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = const_cast<EntityFactory *>(ef)->GetEntityFromEID(building);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	const UnitSpawnerExt *spawner = QIExt<UnitSpawnerExt>(e);

	if (spawner == 0)
	{
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_BUILDQUEUEPROGRESS;
	entry.hud = label;
	entry.parameter0 = building;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToPlayerPop(const char *label, const char *tooltipcb, int index, int playerId)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_PLAYERPOP;
	entry.hud = label;
	entry.parameter0 = playerId;
	entry.parameter1 = index;
	entry.tooltip = tooltipcb;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToGameTime(const char *label)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_GAMETIME;
	entry.hud = label;
	entry.parameter0 = 0;
	entry.parameter1 = 0;
	entry.tooltip = "";

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToEntityHealth(const char *label, int entityId, const char *tooltipcb, int parm)
{
	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_ENTITYHEALTH;
	entry.hud = label;
	entry.tooltip = tooltipcb;
	entry.parameter0 = parm;
	entry.parameter1 = entityId;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToEBPName(const char *label, int ebpid)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = const_cast<EntityFactory *>(ef)->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_EBPNAME;
	entry.hud = label;

	// set text
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, cbp->GetScreenName());

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::BindLabelToEBPCostCash(const char *label, int ebpid)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const ECStaticInfo *si = const_cast<EntityFactory *>(ef)->GetECStaticInfo(ebpid);
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (si == 0)
	{
		dbBreak();
		return;
	}

	//
	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

	if (si == 0)
	{
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_EBPCOSTCASH;
	entry.hud = label;

	// get the cost
	float costCash = cost->costCash;

	// set text
	wchar_t chbuf[32];
	Localizer::ConvertNumber2Localized(chbuf, 32, int(floorf(costCash + 0.5f)));

	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, chbuf);

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::ShowBitmapLabel(const char *label)
{
	//
	BindingHud entry;
	entry.hud = label;
	entry.type = entry.BHUD_LABEL_IMAGE;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::ShowHud(const char *hud)
{
	//
	BindingHud entry;
	entry.hud = hud;
	entry.type = entry.BHUD_LABEL_IMAGE;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::GetCursorInfoOverride(char *cursor, size_t len, int &ttStrId, const Entity *mouseOverEntity)
{
	// TODO: RDNTaskbar::GetCursorInfoOverride is probably not implemented properly

	// check if we are in modal mode - false means 'no we are not'
	if (m_pimpl->m_cursorOverride == false)
	{
		strcpy(cursor, "");
		ttStrId = 0;
		return;
	}

	strcpy(cursor, m_pimpl->m_modalCursor);
	ttStrId = m_pimpl->m_modalTTStrId;
	return;
}

void RDNTaskbar::OnIngameTooltip(int ttStrId)
{
	// TODO: RDNTaskbar::OnIngameTooltip not implemented
}

void RDNTaskbar::BindLabelToResource(const char *label, int resource)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = const_cast<EntityFactory *>(ef)->GetEntityFromEID(resource);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	const ResourceExt *res = QIExt<ResourceExt>(e);

	if (res == 0)
	{
		dbBreak();
		return;
	}

	// bind hud
	BindingHud entry;
	entry.type = entry.BHUD_LABEL_RESOURCE;
	entry.hud = label;
	entry.parameter0 = resource;

	// store
	m_pimpl->m_binding->BindHud(entry);

	return;
}

void RDNTaskbar::LabelUpdateResource(const char *label, int parm)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *e = const_cast<EntityFactory *>(ef)->GetEntityFromEID(parm);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	//
	const ResourceExt *res = QIExt<ResourceExt>(e);

	if (res == 0)
	{
		dbBreak();
		return;
	}

	//
	wchar_t chbuf[32];
	Localizer::ConvertNumber2Localized(chbuf, 32, int(floorf(res->GetResources())));

	// set text
	m_pimpl->m_hud->SetText(CURRENTSCREEN, label, chbuf);

	return;
}

bool RDNTaskbar::LabelUpdateTextTimer(const char *label, float secondDie)
{
	// check if the time of its death has arrived

	if (secondDie > 0)
		// not ready to die yet
		return false;

	// hide it
	m_pimpl->m_hud->Show(CURRENTSCREEN, label, false);

	return true;
}

void RDNTaskbar::PreloadTexture(const char *texture)
{
	m_pimpl->m_hud->PreloadTexture(texture);
}

void RDNTaskbar::BindButtonToBuildingEBP(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		int ebpId)
{
	// validate player
	if (m_pimpl->m_proxy->GetPlayer() == 0 ||
			m_pimpl->m_proxy->GetPlayer()->IsPlayerDead())
		return;

	// find ebp
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpId);

	if (cbp == 0)
	{
		// oops!
		dbFatalf("RDNTaskbar::BindButtonToBuildingEBP No controller blueprint for %d", ebpId);
		return;
	}

	if (IsControllerTypeBuilding(cbp->GetControllerType()) == 0)
	{
		// buildings only
		dbFatalf("RDNTaskbar::BindButtonToBuildingEBP The given controller was not for a building %d", ebpId);
		return;
	}

	// cache player
	const RDNPlayer *player = m_pimpl->m_proxy->GetPlayer();

	// check restriction
	if (player->BlueprintCanBuild(cbp) == m_pimpl->m_proxy->GetPlayer()->BR_Restricted)
		// don't show
		return;

	// texture
	const char *texture = cbp->GetIconName();

	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			texture,
			IsEnabledBuilding(m_pimpl->m_proxy, ebpId),
			ebpId,
			0,
			BindingButton::BHUD_BUTTON_BUILDING);

	return;
}

void RDNTaskbar::BindButtonToUnitEBP(
		const char *button,
		const char *hotkeyLuaName,
		const char *callback,
		const char *tooltipcb,
		int building,
		int ebpId)
{
	// validate player
	if (m_pimpl->m_proxy->GetPlayer() == 0 ||
			m_pimpl->m_proxy->GetPlayer()->IsPlayerDead())
		return;

	// find ebp
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpId);

	if (cbp == 0)
	{
		// oops!
		dbBreak();
		return;
	}

	if (IsControllerTypeUnit(cbp->GetControllerType()) == 0)
	{
		// units only
		dbBreak();
		return;
	}

	const RDNPlayer *player = m_pimpl->m_proxy->GetPlayer();

	// check restriction
	if (player->BlueprintCanBuild(cbp) == m_pimpl->m_proxy->GetPlayer()->BR_Restricted)
		// don't show
		return;

	//
	const Entity *e = ef->GetEntityFromEID(building);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	const UnitSpawnerExt *spawn = QIExt<UnitSpawnerExt>(e);

	if (spawn == 0)
	{
		dbBreak();
		return;
	}

	// type
	if (spawn->UnitListFilter(ebpId) == 0)
		// don't even show the button
		return;

	// texture
	const char *texture = cbp->GetIconName();

	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			texture,
			IsEnabledUnit(m_pimpl->m_proxy, ebpId, building),
			ebpId,
			building,
			BindingButton::BHUD_BUTTON_UNIT);

	return;
}

void RDNTaskbar::BindButtonToGroup(const char *button, const char *callback_left, const char *callback_right, const char *tooltipcb, int groupNb)
{
	ButtonInternal(button, "", callback_left, callback_right, tooltipcb, "", true, groupNb, 0,
								 BindingButton::BHUD_BUTTON_GROUP);
}

void RDNTaskbar::ButtonUpdateBuilding(const char *button, int parm0, int parm1)
{
	dbTracef("RDNTaskbar::ButtonUpdateBuilding probably not implemented properly");

	//
	const bool enabled =
			IsEnabledBuilding(m_pimpl->m_proxy, parm0);

	m_pimpl->m_hud->Enable(CURRENTSCREEN, button, enabled);

	return;
}

void RDNTaskbar::ButtonUpdateUnit(const char *button, int parm0, int parm1)
{
	//
	const bool enabled =
			IsEnabledUnit(m_pimpl->m_proxy, parm0, parm1);

	m_pimpl->m_hud->Enable(CURRENTSCREEN, button, enabled);

	return;
}

void RDNTaskbar::ButtonUpdateGroup(const char *, int groupNum, int)
{
	// check if group has been assigned
	const EntityGroup &egroup = m_pimpl->m_proxy->GetSelectionInterface()->GetHotkeyGroup(groupNum);

	char hudName[64];
	_snprintf(hudName, LENGTHOF(hudName), "hotkey_%d_assigned", (groupNum == 9) ? 0 : (groupNum + 1));
	if (egroup.size())
	{
		m_pimpl->m_hud->Show(CURRENTSCREEN, hudName, true);
	}
	else
	{
		m_pimpl->m_hud->Show(CURRENTSCREEN, hudName, false);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Displays the rally point for a building.
//				Called by lua, on selection.
//				Called by the GameEvent system when a rally point is set
//	Result	:
//	Param.	: the entityId of the building
//	Author	: dswinerd, modified for lab specific stuff by bcode
//
void RDNTaskbar::RallyPointShow(int buildingId)
{
	//
	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();
	const Entity *pEntity = ef->GetEntityFromEID(buildingId);

	if (!pEntity)
	{ // need an Entity
		return;
	}

	// hide the effect here
	RallyPointHide();

	Vec3f position;

	const UnitSpawnerExt *pExt = QIExt<UnitSpawnerExt>(pEntity);
	if (!pExt || !pExt->GetRallyPosition(position))
	{ // need an extension and rally position
		return;
	}

	const EntityGroup *cachedRallyTarget;
	const Vec3f *cachedRallyPosition;

	bool bUseCached = m_pimpl->m_proxy->GetCachedRally(&cachedRallyPosition, &cachedRallyTarget);

	if (bUseCached)
	{ // using cached version of the rally target/position

		if (!cachedRallyTarget->empty())
		{ // we are caching a rally target, until the command is executed

			// positioning of the rally point will be taken care of in RallyPointUpdate() as it gets the interpolated position of the rally entity
			m_pimpl->m_rallyGroup = *cachedRallyTarget;
		}
		else
		{
			// visualize the position of the rally point
			RallyPointSetPosition(*cachedRallyPosition, true);
		}
	}
	else
	{
		m_pimpl->m_rallyGroup.clear();

		const Entity *pRallyTarget = pExt->GetRallyTarget();

		if (pRallyTarget)
		{ // positioning of the rally point will be taken care of in RallyPointUpdate() as it gets the interpolated position of the rally entity
			m_pimpl->m_rallyGroup.push_back(const_cast<Entity *>(pRallyTarget));
		}
		else
		{
			// move the position of the rally point
			RallyPointSetPosition(position, true);
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Stops showing the rally point
//	Result	:
//	Param.	:
//	Author	: dswinerd, modified for lab specific stuff by bcode
//
void RDNTaskbar::RallyPointHide()
{
	// hide the generic rally point effect
	m_pimpl->m_fx->FXSetVisible(m_pimpl->m_rallyFxHandle, false);
	m_pimpl->m_rallyGroup.clear();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Used to update the position of a rally point set on an Entity
//	Result	:
//	Param.	:
//	Author	: dswinerd, modified for lab specific stuff by bcode
//
void RDNTaskbar::RallyPointUpdate(const Entity *pEntity, const Vec3f &interpPos)
{
	RallyPointUpdateHelper(pEntity, interpPos);
}

void RDNTaskbar::RallyPointUpdateHelper(const Entity *pEntity, const Vec3f &interpPos)
{
	if (m_pimpl->m_rallyGroup.empty())
	{ // no rally point to update
		return;
	}

	dbAssert(m_pimpl->m_rallyGroup.size() == 1);

	Entity *pRally = m_pimpl->m_rallyGroup.front();
	if (pEntity == pRally)
	{ // this is the rally point -> update
		Vec3f adjustedPosition = interpPos;

		adjustedPosition.y += pRally->GetOBB().GetYScale() * 2.0f;

		RallyPointSetPosition(adjustedPosition, true);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: sets the position and visibility of the rally point
//	Result	: changes position and visibility of the rally point
//	Param.	: position - the new position of the point
//			  bVisible - true if the rally point is to be visible
//	Author	: dswinerd
//
void RDNTaskbar::RallyPointSetPosition(const Vec3f &position, bool bVisible)
{
	Matrix43f m;
	m.IdentitySelf();
	m.T = position;

	FXInterface::Handle *pHandle = &m_pimpl->m_rallyFxHandle;

	m_pimpl->m_fx->FXSetTransform(*pHandle, m);
	m_pimpl->m_fx->FXSetVisible(*pHandle, bVisible);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::WayPointPathShow()
{
	dbAssert(m_pimpl->m_WayPointFX.size() == 0);

	// check if group has been assigned
	const EntityGroup &egroup = m_pimpl->m_proxy->GetSelectionInterface()->GetSelection();
	dbAssert(egroup.size() > 0);

	PointList waypointpath;

	if (GetGroupWayPointPath(egroup, waypointpath))
	{
		PointList::iterator iter = waypointpath.begin();
		PointList::iterator eiter = waypointpath.end();

		Matrix43f transform;
		transform.IdentitySelf();

		for (size_t i = 0; iter != eiter; ++iter, ++i)
		{
			FXInterface::Handle newfx = m_pimpl->m_fx->FXCreate(k_WayPointFXName);

			transform.T = waypointpath[i];

			m_pimpl->m_fx->FXSetTransform(newfx, transform);

			m_pimpl->m_WayPointFX.push_back(newfx);
		}
	}

	m_pimpl->m_bUpdateWayPoint = true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::WayPointPathHide()
{
	DestroyFXHandleList(m_pimpl->m_fx, m_pimpl->m_WayPointFX);

	m_pimpl->m_bUpdateWayPoint = false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::WayPointUpdate()
{
	// check if group has been assigned
	const EntityGroup &egroup = m_pimpl->m_proxy->GetSelectionInterface()->GetSelection();
	dbAssert(egroup.size() > 0);

	PointList waypointpath;

	if (GetGroupWayPointPath(egroup, waypointpath))
	{
		PointList::iterator iter = waypointpath.begin();
		PointList::iterator eiter = waypointpath.end();

		FXPointList templist = m_pimpl->m_WayPointFX;
		m_pimpl->m_WayPointFX.clear();

		Matrix43f transform;
		transform.IdentitySelf();

		for (size_t i = 0; iter != eiter; ++iter, ++i)
		{
			transform.T = waypointpath[i];

			FXInterface::Handle fxhandle;

			if (!templist.empty())
			{
				fxhandle = templist.front();
				templist.erase(templist.begin());
			}
			else
			{
				fxhandle = m_pimpl->m_fx->FXCreate(k_WayPointFXName);
			}

			m_pimpl->m_fx->FXSetTransform(fxhandle, transform);

			m_pimpl->m_WayPointFX.push_back(fxhandle);
		}

		DestroyFXHandleList(m_pimpl->m_fx, templist);
	}
	else
	{
		if (m_pimpl->m_WayPointFX.size() > 0)
		{
			DestroyFXHandleList(m_pimpl->m_fx, m_pimpl->m_WayPointFX);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::OnCinematicMode(bool bCinematic)
{
	if (bCinematic)
	{
		// clear all selections to hide any waypoint fx
		EntityGroup empty;
		m_pimpl->m_proxy->GetSelectionInterface()->SetSelection(empty);
	}

	// otherwise, do nothing
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::BindButtonToChat(const char *button, const char *hotkeyLuaName, const char *callback, const char *tooltipcb)
{
	// delegate
	ButtonInternal(
			button,
			hotkeyLuaName,
			callback,
			"",
			tooltipcb,
			"",
			false,
			0,
			0,
			BindingButton::BHUD_BUTTON_CHAT);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::ButtonUpdateChat(const char *button, int parm0, int parm1)
{
	dbTracef("RDNTaskbar::ButtonUpdateChat possibly not implemented properly");

	//
	const int r =
			m_pimpl->m_uiproxy->ChatAllowed();

	m_pimpl->m_hud->Enable(CURRENTSCREEN, button, (r == RDNUIProxy::CHATALLOW_Ok));

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::ButtonUpdateSelectEntity(const char *button)
{
	// entity-selection is disabled during modal-UI mode
	bool bModalUIMode = m_pimpl->m_modalParm ? true : false;
	m_pimpl->m_hud->Enable(CURRENTSCREEN, button, !bModalUIMode);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::OnSaveControlGroup(const Entity *pEntity)
{
	for (int groupNum = 0; groupNum < 10; groupNum++)
	{
		const EntityGroup &egroup = m_pimpl->m_proxy->GetSelectionInterface()->GetHotkeyGroup(groupNum);
		if (egroup.find(pEntity) != egroup.end())
		{
			// remember entity
			m_pimpl->m_cachedHotkeyGroup[groupNum].push_back(const_cast<Entity *>(pEntity));
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::OnRestoreControlGroup(const Entity *pEntity)
{
	for (int groupNum = 0; groupNum < 10; groupNum++)
	{
		if (m_pimpl->m_cachedHotkeyGroup[groupNum].find(pEntity) != m_pimpl->m_cachedHotkeyGroup[groupNum].end())
		{
			EntityGroup &egroup =
					const_cast<EntityGroup &>(m_pimpl->m_proxy->GetSelectionInterface()->GetHotkeyGroup(groupNum));

			// remember entity
			egroup.push_back(const_cast<Entity *>(pEntity));
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::UpdateEntityFow(const Entity *e)
{
	if (m_pimpl->m_minimaps.empty() == 0)
		m_pimpl->m_minimaps.front()->UpdateEntityFow(e);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNTaskbar::IsEntityVisible(const Entity *e)
{
	if (m_pimpl->m_minimaps.empty() == 0)
		return m_pimpl->m_minimaps.front()->IsEntityVisible(e);
	else
		return m_pimpl->m_proxy->GetWorld()->IsEntityVisible(m_pimpl->m_proxy->GetPlayer(), e);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::SetRevealAll(bool b)
{
	if (m_pimpl->m_minimaps.empty() == 0)
		m_pimpl->m_minimaps.front()->SetRevealAll(b);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNTaskbar::GetRevealAll() const
{
	if (m_pimpl->m_minimaps.empty() == 0)
		return m_pimpl->m_minimaps.front()->GetRevealAll();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::ResetGameTimer()
{
	m_pimpl->m_startTimer = ModObj::i()->GetWorld()->GetGameTime();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::SetGameStartTime(float startTime)
{
	m_pimpl->m_startTimer = startTime;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float RDNTaskbar::GetGameStartTime() const
{
	return m_pimpl->m_startTimer;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::Preload()
{
	m_pimpl->m_fx->FXPreload(k_WayPointFXName);
	m_pimpl->m_fx->FXPreload(k_RallyFXName);

	m_pimpl->m_fx->FXPreload(k_CanPlaceOk);
	m_pimpl->m_fx->FXPreload(k_CanPlaceBad);
	m_pimpl->m_fx->FXPreload(k_CanPlaceBadFow);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTaskbar::UnselectHiddenEntities()
{
	// don't care if there's no local player
	const RDNPlayer *pPlayer = m_pimpl->m_proxy->GetPlayer();
	if (!pPlayer)
		return;

	Player::PlayerID localPlayerID = pPlayer->GetID();

	const EntityGroup &selGroup = m_pimpl->m_proxy->GetSelection();
	EntityGroup::const_iterator ei = selGroup.begin();
	EntityGroup::const_iterator ee = selGroup.end();

	// look for non-local player owned entities amongst the selected entities
	for (; ei != ee; ei++)
	{
		const Entity *pEntity = *ei;

		// non-local player owned entity?
		if (pEntity->GetOwner() && (pEntity->GetOwner()->GetID() != localPlayerID))
		{
			// check to see if the entity is visible
			if (!IsEntityVisible(pEntity))
			{
				// clear selection if the entity is no longer visible
				EntityGroup emptyGroup;
				m_pimpl->m_proxy->GetSelectionInterface()->SetSelection(emptyGroup);

				// remove entity from focus groups
				m_pimpl->m_proxy->GetCameraInterface()->RemoveFocusFromEntity(pEntity);

				break;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    : ehuang
//
int RDNTaskbar::GetModalCommandMode()
{
	return m_pimpl->m_modalParm;
}

void RDNTaskbar::HelpTextTitle(int modtextId)
{
	std::wstring helpText(Localizer::GetString(modtextId));
	// dbTracef("RDNTaskbar::HelpTextTitle %S", helpText);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, "ingame_helptext_title", helpText.c_str());
}

void RDNTaskbar::HelpTextShortcut(const char *hotkeyLuaName)
{
	const RDNInputBinder::HotKey *p_hk = m_pimpl->m_pInputBinder->GetHotKeyByTableName(hotkeyLuaName);
	if (p_hk)
	{
		// dbTracef("RDNTaskbar::HelpTextShortcut Hotkey for %s is: %s", hotkeyLuaName, p_hk->keyCombo);
	}
	else
	{
		// dbTracef("RDNTaskbar::HelpTextShortcut Could not find hot key string for %s", hotkeyLuaName);
		std::wstring fallbackText(std::wstring(L"FallbackText"));
		m_pimpl->m_hud->SetText(CURRENTSCREEN, "ingame_helptext_shortcut", fallbackText.c_str());
	}
}

void RDNTaskbar::HelpTextTextWithoutRequirements(int modtextId)
{
	std::wstring helpText(Localizer::GetString(modtextId));
	// dbTracef("RDNTaskbar::HelpTextTextWithoutRequirements %S", helpText);

	m_pimpl->m_hud->SetText(CURRENTSCREEN, "ingame_helptext_without_requirements", helpText.c_str());
}

void RDNTaskbar::HelpTextChat()
{
	// dbTracef("RDNTaskbar::HelpTextChat");
}

int RDNTaskbar::TypeFromEBP(long ebpid)
{
	dbTracef("RDNTaskbar::TypeFromEBP id %d", ebpid);

	const EntityFactory *ef = m_pimpl->m_proxy->GetWorld()->GetEntityFactory();

	dbTracef("got entity factory");
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);
	dbTracef("tried to get a blueprint");

	if (cbp)
	{
		dbTracef("got a blueprint");
		return cbp->GetControllerType();
	}

	dbTracef("got nada");
	return NULL_EC;
}
