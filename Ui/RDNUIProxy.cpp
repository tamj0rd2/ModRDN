/////////////////////////////////////////////////////////////////////
// File    : RDNUIProxy.cpp
// Desc    :
// Created : Wednesday, January 30, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNUIProxy.h"

#include "RDNSimProxy.h"

#include "DlgModOptions.h"

#include "Objective.h"
#include "ObjectiveFactory.h"

#include "RDNUIOptions.h"
#include "RDNEntityFilter.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "../Simulation/RDNTuning.h"
#include "../Simulation/RDNPlayer.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNQuery.h"
#include "../Simulation/GameEventDefs.h"
#include "../Simulation/CommandTypes.h"
#include "../Simulation/AttackTypes.h"

#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/Extensions/HealthExt.h"

#include "../Simulation/ExtInfo/MovingExtInfo.h"
#include "../Simulation/ExtInfo/AttackExtInfo.h"

#include "../Simulation/States/State.h"

#include <SimEngine/EntityAnimator.h>

#include <EngineAPI/SoundInterface.h>
#include <EngineAPI/FxInterface.h>
#include <EngineAPI/SelectionInterface.h>
#include <EngineAPI/CameraInterface.h>
#include <EngineAPI/UIInterface.h>
#include <EngineAPI/MessageInterface.h>

#include <Lua/LuaConfig.h>
#include <Lua/LuaBinding.h>
#include <Util/Random.h>

namespace
{
	class ControllerMatchPred : public std::unary_function<Entity *, bool>
	{
	public:
		ControllerMatchPred(ControllerType type)
				: m_type(type)
		{
		}

		bool operator()(const Entity *pEntity) const
		{
			const ModController *pModCtrlr = static_cast<const ModController *>(pEntity->GetController());
			return (pModCtrlr && pModCtrlr->GetControllerType() == m_type);
		}

	private:
		ControllerType m_type;
	};

	template <typename ExtType>
	struct HasExtensionPred : public std::unary_function<Entity *, bool>
	{
		bool operator()(const Entity *pEntity) const
		{
			return (QIExt<ExtType>(pEntity) != 0);
		}
	};

	struct IsGroundUnitPred : public std::unary_function<Entity *, bool>
	{
		bool operator()(const Entity *pEntity) const
		{
			const MovingExtInfo *pExt = QIExtInfo<MovingExtInfo>(pEntity->GetController());
			return (pExt && pExt->IsGround());
		}
	};

	struct IsWaterUnitPred : public std::unary_function<Entity *, bool>
	{
		bool operator()(const Entity *pEntity) const
		{
			const MovingExtInfo *pExt = QIExtInfo<MovingExtInfo>(pEntity->GetController());
			return (pExt && pExt->IsSwimmer());
		}
	};

	struct IsAirUnitPred : public std::unary_function<Entity *, bool>
	{
		bool operator()(const Entity *pEntity) const
		{
			const MovingExtInfo *pExt = QIExtInfo<MovingExtInfo>(pEntity->GetController());
			return (pExt && pExt->IsFlyer());
		}
	};

	class IsUnitInStatePred : public std::unary_function<Entity *, bool>
	{
	public:
		IsUnitInStatePred(State::StateIDType state)
				: m_state(state)
		{
		}

		bool operator()(const Entity *pEntity) const
		{
			const ModController *pModController = static_cast<const ModController *>(pEntity->GetController());
			if (!pModController)
				return false;
			const State *pState = pModController->QIActiveState();
			return (pState && (pState->GetStateID() == m_state));
		}

	private:
		State::StateIDType m_state;
	};

	template <typename ExtInfoType>
	struct HasExtInfoPred : public std::unary_function<Entity *, bool>
	{
		bool operator()(const Entity *pEntity) const
		{
			return (QIExtInfo<ExtInfoType>(pEntity->GetControllerBP()) != 0);
		}
	};

	class EntityFlagPred : public std::unary_function<Entity *, bool>
	{
	public:
		EntityFlagPred(ENTITY_FLAGS flag)
				: m_flag(flag)
		{
		}

		bool operator()(const Entity *pEntity) const
		{
			return pEntity->GetEntityFlag(m_flag);
		}

	private:
		ENTITY_FLAGS m_flag;
	};

	template <typename Pred>
	void CollectAll(const Player *pPlayer, Pred pred, EntityGroup &group)
	{
		group.clear();

		if (pPlayer == NULL)
			return;

		EntityGroup::const_iterator iEntity = pPlayer->GetEntities().begin();
		EntityGroup::const_iterator eEntity = pPlayer->GetEntities().end();
		for (; iEntity != eEntity; ++iEntity)
		{
			if ((*iEntity)->GetEntityFlag(EF_IsSpawned) &&
					(*iEntity)->GetEntityFlag(EF_Selectable) &&
					pred(*iEntity))
				group.push_back(*iEntity);
		}
	}

	template <typename Pred>
	void SelectNext(const Player *pPlayer, Pred pred, EntityGroup &group)
	{
		Entity *pCurrent = group.front();
		if (pCurrent && !pred(pCurrent))
			pCurrent = NULL;

		CollectAll(pPlayer, pred, group);

		if (group.size() > 1)
		{
			//	Filter out all that have IDs <= to the
			EntityIDNumber currentID = 0;
			if (pCurrent)
				currentID = pCurrent->GetID();

			EntityIDNumber nextID = 0xffffffff;
			Entity *pNext = NULL;

			Entity *pFirst = group.front();

			EntityGroup::const_iterator iEntity = group.begin();
			EntityGroup::const_iterator eEntity = group.end();
			for (; iEntity != eEntity; ++iEntity)
			{
				EntityIDNumber id = (*iEntity)->GetID();
				if ((id > currentID) &&
						(id < nextID))
				{
					pNext = (*iEntity);
					nextID = id;
				}
				if (id < pFirst->GetID())
				{
					pFirst = *iEntity;
				}
			}

			group.clear();
			if (pNext)
				group.push_back(pNext);
			else
				group.push_back(pFirst);
		}
	}

	const float PLAYBACK_FAST = 1000.0f;
	const float PLAYBACK_NORMAL = 8.0f;
} // namespace

/////////////////////////////////////////////////////////////////////
// RDNUIProxy

class RDNUIProxy::Data
{
public:
	LuaConfig *m_lua;

	RDNSimProxy *m_sim;

	RTSHud *m_hud;
	SelectionInterface *m_selection;
	CameraInterface *m_camera;
	UIInterface *m_ui;
	SoundInterface *m_sound;
	FXInterface *m_fx;
	MessageInterface *m_message;
	RDNInputBinder *m_pInputBinder;
	RDNUIOptions *m_pRDNUIOptions;

	std::vector<LuaBinding::Obj>
			m_exported;

	DlgModOptions *m_dlgModOptions;

	bool m_gameStartFlag;
	bool m_playerLoseFlag;
	bool m_playerWinFlag;

	EntityGroup m_idleHenchman;
	EntityGroup m_nextGroundCombiner, m_nextWaterCombiner, m_nextAirCombiner;

	bool m_bCinematic;
};

RDNUIProxy::RDNUIProxy(
		LuaConfig *lua,
		RTSHud *rts,
		SelectionInterface *selection,
		CameraInterface *camera,
		UIInterface *ui,
		FXInterface *fx,
		SoundInterface *sound,
		MessageInterface *message,
		RDNSimProxy *sim,
		RDNInputBinder *pInputBinder,
		RDNUIOptions *uiOptions)
		: m_pimpl(new Data)
{
	// init fields
	m_pimpl->m_lua = lua;

	m_pimpl->m_sim = sim;

	m_pimpl->m_hud = rts;
	m_pimpl->m_selection = selection;
	m_pimpl->m_camera = camera;
	m_pimpl->m_ui = ui;
	m_pimpl->m_fx = fx;
	m_pimpl->m_message = message;
	m_pimpl->m_sound = sound;
	m_pimpl->m_pInputBinder = pInputBinder;
	m_pimpl->m_pRDNUIOptions = uiOptions;

	m_pimpl->m_gameStartFlag = false;
	m_pimpl->m_playerLoseFlag = false;
	m_pimpl->m_playerWinFlag = false;

	m_pimpl->m_bCinematic = false;

	//
	Preload();

	// register to lua
	LuaSetup();

	// observe the events
	GameEventSys::Instance()->RegisterClient(this);

	return;
}

RDNUIProxy::~RDNUIProxy()
{
	// unregister from events
	GameEventSys::Instance()->UnregisterClient(this);

	// clean-up lua
	LuaReset();

	DELETEZERO(m_pimpl);

	return;
}

void RDNUIProxy::LuaSetup()
{
#define BINDINNERCONSTANT(t, c) \
	m_pimpl->m_lua->SetNumber(#c, double(t::c))

	BINDINNERCONSTANT(RDNUIProxy, CHATALLOW_Ok);
	BINDINNERCONSTANT(RDNUIProxy, CHATALLOW_NotMP);
	BINDINNERCONSTANT(RDNUIProxy, CHATALLOW_Dead);
	BINDINNERCONSTANT(RDNUIProxy, CHATALLOW_NoLocal);
	BINDINNERCONSTANT(RDNUIProxy, CHATALLOW_COPPA);

#undef BINDINNERCONSTANT

#define BINDFUNC(f) \
	m_pimpl->m_exported.push_back(LuaBinding::Bind(m_pimpl->m_lua, #f, this, &RDNUIProxy::f))

	BINDFUNC(PlaySound);

	BINDFUNC(SelectionCount);
	BINDFUNC(SelectionId);
	BINDFUNC(GetRangeAttackCount);
	BINDFUNC(SelectionBelongsToPlayer);
	BINDFUNC(SelectionIsEnemy);
	BINDFUNC(SelectionIsAlly);
	BINDFUNC(SelectionAllSameType);
	BINDFUNC(SelectionHasAttackType);

	BINDFUNC(SelectEntity);
	BINDFUNC(DeSelectEntity);
	BINDFUNC(DeSelectAll);

	BINDFUNC(SelectHotkeyGroup);
	BINDFUNC(AssignHotkeyGroup);
	BINDFUNC(UnassignFromAllHotkeyGroups);

	BINDFUNC(SelectAllUnitsOnScreen);
	BINDFUNC(SelectAllUnitsInWorld);
	BINDFUNC(SelectHQ);
	BINDFUNC(SelectNextSubSelect);

	BINDFUNC(PauseMenuShow);

	BINDFUNC(ChatAllowed);
	BINDFUNC(ChatShow);

	BINDFUNC(FastSpeedAllowed);
	BINDFUNC(IsFastSpeed);
	BINDFUNC(SetFastSpeed);

	BINDFUNC(BuildButtonPressed);
	BINDFUNC(BuildEBPButtonPressed);

	BINDFUNC(FocusOnEntity);
	BINDFUNC(FocusOnSelection);
	BINDFUNC(ZoomCameraMouse);

	BINDFUNC(LoadUIOptions);

#undef BINDFUNC

	return;
}

void RDNUIProxy::PlaySound(const char *sound)
{
	//
	if (strlen(sound) == 0)
		return;

	char soundfile[_MAX_PATH];
	strcpy(soundfile, "data:");
	strcat(soundfile, sound);

	m_pimpl->m_sound->PlaySound(soundfile);

	return;
}

int RDNUIProxy::SelectionCount() const
{
	return m_pimpl->m_selection->GetSelection().size();
}

int RDNUIProxy::SelectionId(int selndx) const
{
	// validate index
	if (selndx < 0 || selndx >= SelectionCount())
	{
		dbBreak();
		return Invalid_Entity;
	}

	// get selection
	EntityGroup::const_iterator it = m_pimpl->m_selection->GetSelection().begin();
	std::advance(it, selndx);

	return (*it)->GetID();
}

int RDNUIProxy::GetRangeAttackCount(int ebpid)
{
	// find ebp
	const EntityFactory *ef = m_pimpl->m_sim->GetWorld()->GetEntityFactory();
	const ControllerBlueprint *cbp = ef->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		// oops!
		dbBreak();
		return 0;
	}
	return int(cbp->GetRangeAttackCount());
}

bool RDNUIProxy::SelectionIsEnemy() const
{
	// check player
	if (m_pimpl->m_sim->GetPlayer() == 0 ||
			m_pimpl->m_sim->GetPlayer()->IsPlayerDead())
		return false;

	// check 1st entity in selection
	const Entity *e = m_pimpl->m_selection->GetSelection().front();

	if (e == 0)
	{
		dbBreak();
		return false;
	}

	if (e->GetOwner() == 0)
		return false;

	if (m_pimpl->m_sim->GetPlayer() == e->GetOwner())
		return false;

	return true;
}

bool RDNUIProxy::SelectionIsAlly() const
{
	// validate object state
	if (SelectionCount() == 0)
	{
		dbBreak();
		return false;
	}

	// check player
	if (m_pimpl->m_sim->GetPlayer() == 0 ||
			m_pimpl->m_sim->GetPlayer()->IsPlayerDead())
		return false;

	// check 1st entity in selection
	const Entity *e = m_pimpl->m_selection->GetSelection().front();

	if (e->GetOwner() == 0)
		return false;

	if (m_pimpl->m_sim->GetPlayer() != e->GetOwner())
		return false;

	return true;
}

void RDNUIProxy::BuildButtonPressed()
{
	GameEventSys::Instance()->PublishEvent(GameEvent_UIHenchmanBuild());
}

void RDNUIProxy::BuildEBPButtonPressed(long ebpid)
{
	GameEventSys::Instance()->PublishEvent(GameEvent_UIStartBuildUnit(ebpid));
}

void RDNUIProxy::FocusOnEntity(int entityId, bool focusOnEntity, bool jump)
{
	// quick-out
	if (entityId == 0)
		return;

	// locate entity
	const EntityFactory *ef = m_pimpl->m_sim->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	if (focusOnEntity)
	{
		//
		EntityGroup g;
		g.push_back(const_cast<Entity *>(e));

		m_pimpl->m_camera->FocusOnEntityGroup(g);
	}
	else
	{
		m_pimpl->m_camera->FocusOnTerrain(e->GetPosition());
	}

	//
	if (jump)
	{
		m_pimpl->m_camera->ForceCamera();
	}

	return;
}

void RDNUIProxy::FocusOnSelection()
{
	// check if selection is empty
	if (m_pimpl->m_selection->GetSelection().empty())
		return;

	//
	m_pimpl->m_camera->FocusOnEntityGroup(m_pimpl->m_selection->GetSelection());

	return;
}

void RDNUIProxy::ZoomCameraMouse(float ammount)
{
	//
	m_pimpl->m_camera->ZoomCameraMouse(ammount);
}

void RDNUIProxy::SelectEntity(int entityId, int actOnSimilar)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_sim->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	if (actOnSimilar)
	{
		long eid = e->GetControllerBP()->GetEBPNetworkID();

		// copy group
		EntityGroup newgroup(m_pimpl->m_selection->GetSelection());

		// only remove entities of the same type
		EntityGroup::iterator iter = newgroup.begin();
		while (iter != newgroup.end())
		{
			const Entity *pEntity = *iter;

			if (pEntity->GetControllerBP()->GetEBPNetworkID() != eid)
			{
				iter = newgroup.erase(iter);
				continue;
			}

			++iter;
		}

		m_pimpl->m_selection->SetSelection(newgroup);
	}
	else
	{
		//
		EntityGroup g;
		g.push_back(const_cast<Entity *>(e));

		m_pimpl->m_selection->SetSelection(g);
	}

	return;
}

void RDNUIProxy::DeSelectEntity(int entityId, int actOnSimilar)
{
	// locate entity
	const EntityFactory *ef = m_pimpl->m_sim->GetWorld()->GetEntityFactory();
	const Entity *e = ef->GetEntityFromEID(entityId);

	if (e == 0)
	{
		dbBreak();
		return;
	}

	// copy group
	EntityGroup newgroup(m_pimpl->m_selection->GetSelection());

	// only remove entities of the same type
	if (actOnSimilar)
	{
		long eid = e->GetControllerBP()->GetEBPNetworkID();

		// remove entities of the same ebp id
		EntityGroup::iterator iter = newgroup.begin();
		while (iter != newgroup.end())
		{
			const Entity *pEntity = *iter;

			if (pEntity->GetControllerBP()->GetEBPNetworkID() == eid)
			{
				iter = newgroup.erase(iter);
				continue;
			}

			++iter;
		}
	}
	else
	{
		// remove just this entity
		newgroup.remove(e);
	}

	// set the selection
	m_pimpl->m_selection->SetSelection(newgroup);

	return;
}

void RDNUIProxy::DeSelectAll(void)
{
	// clear selection
	m_pimpl->m_selection->SetSelection(EntityGroup());
}

void RDNUIProxy::PauseMenuShow(void)
{
	m_pimpl->m_ui->ShowPauseMenu();
}

void RDNUIProxy::ChatShow()
{
	m_pimpl->m_ui->ShowChat(UIInterface::CHAT_All);
}

int RDNUIProxy::ChatAllowed() const
{
	const UIInterface::ChatAllowedResult r =
			m_pimpl->m_ui->ChatAllowed();

	if (r != UIInterface::CHATALLOW_Ok)
	{
		switch (r)
		{
		case UIInterface::CHATALLOW_NotMP:
			return RDNUIProxy::CHATALLOW_NotMP;

		case UIInterface::CHATALLOW_NoLocal:
			return RDNUIProxy::CHATALLOW_NoLocal;

		case UIInterface::CHATALLOW_COPPA:
			return RDNUIProxy::CHATALLOW_COPPA;
		}
	}

	// check if player is dead
	if (m_pimpl->m_sim->GetPlayer() == 0 ||
			m_pimpl->m_sim->GetPlayer()->IsPlayerDead())
		return RDNUIProxy::CHATALLOW_Dead;

	return RDNUIProxy::CHATALLOW_Ok;
}

bool RDNUIProxy::SelectionAllSameType() const
{
	//
	const EntityGroup &g = m_pimpl->m_selection->GetSelection();

	// validate selection
	if (g.size() == 0)
	{
		dbBreak();
		return false;
	}
	else
			// check for single selection
			if (g.size() == 1)
	{
		return true;
	}
	else
	{
		const unsigned long ctFront = g.front()->GetControllerBP()->GetControllerType();

		EntityGroup::const_iterator i = g.begin();
		++i;
		EntityGroup::const_iterator e = g.end();

		for (; i != e; ++i)
		{
			if ((*i)->GetControllerBP()->GetControllerType() != ctFront)
				break;
		}

		return i == e;
	}
}

bool RDNUIProxy::SelectionHasAttackType(int attacktype) const
{
	// validate parm
	if (attacktype < 0 || attacktype >= ATTACKTYPE_COUNT)
	{
		dbBreak();
		return false;
	}

	// validate object state
	if (SelectionCount() == 0)
	{
		dbBreak();
		return false;
	}

	//
	const EntityGroup &g = m_pimpl->m_selection->GetSelection();

	EntityGroup::const_iterator i = g.begin();
	EntityGroup::const_iterator e = g.end();

	for (; i != e; ++i)
	{
		//
		const AttackExtInfo *attack = QIExtInfo<AttackExtInfo>((*i)->GetController());

		if (attack != 0)
		{
			if (attacktype == ATTACKTYPE_Melee && !attack->attackInfo.meleeList.empty())
				break;
		}
	}

	if (i != e)
		return true;

	if (attacktype == ATTACKTYPE_NonRetaliate)
		return true;

	return false;
}

bool RDNUIProxy::FastSpeedAllowed() const
{
	return m_pimpl->m_ui->FastSpeedAllowed();
}

bool RDNUIProxy::IsFastSpeed() const
{
	return (m_pimpl->m_ui->GetSimulationRate() == PLAYBACK_FAST);
}

void RDNUIProxy::SetFastSpeed(bool bFast)
{
	if (bFast)
	{
		m_pimpl->m_ui->SetSimulationRate(PLAYBACK_FAST);
	}
	else
	{
		m_pimpl->m_ui->SetSimulationRate(PLAYBACK_NORMAL);
	}
}

void RDNUIProxy::ModOptionsShow(void)
{
	dbAssert(m_pimpl->m_pRDNUIOptions);

	m_pimpl->m_dlgModOptions = DlgModOptions::Create(m_pimpl->m_hud, m_pimpl->m_sim, m_pimpl->m_pInputBinder, m_pimpl->m_pRDNUIOptions, DlgModOptions::CloseCB::Bind(this, &RDNUIProxy::DlgModOptionsClose));

	return;
}

void RDNUIProxy::DlgModOptionsClose()
{
	DELETEZERO(m_pimpl->m_dlgModOptions);
}

void RDNUIProxy::SelectHotkeyGroup(int groupNb)
{
	if (groupNb < 0 || groupNb > 9)
		return;

	m_pimpl->m_selection->SetSelectionToHotkeyGroup(groupNb);
}

void RDNUIProxy::AssignHotkeyGroup(int groupNb)
{
	if (groupNb < 0 || groupNb > 9)
		return;

	m_pimpl->m_selection->AssignHotkeyGroupFromSelection(groupNb, RDNEntityFilter::Instance());
}

void RDNUIProxy::UnassignFromAllHotkeyGroups()
{
	m_pimpl->m_selection->ClearSelectionFromAllHotkeyGroups();
}

bool RDNUIProxy::SelectionBelongsToPlayer() const
{
	// validate object state
	if (SelectionCount() == 0)
	{
		dbBreak();
		return false;
	}

	return m_pimpl->m_sim->EntityBelongsToPlayer(SelectionId(0));
}

void RDNUIProxy::SelectAllUnitsOnScreen()
{
	EntityGroup group;
	CollectAll(m_pimpl->m_sim->GetPlayer(), std::not1(EntityFlagPred(EF_SingleSelectOnly)), group);
	m_pimpl->m_selection->SetSelectionOnScreen(group);
}

void RDNUIProxy::SelectAllUnitsInWorld()
{
	EntityGroup group;
	CollectAll(m_pimpl->m_sim->GetPlayer(), std::not1(EntityFlagPred(EF_SingleSelectOnly)), group);
	m_pimpl->m_selection->SetSelection(group);
}

void RDNUIProxy::SelectHQ()
{
	EntityGroup group;
	CollectAll(m_pimpl->m_sim->GetPlayer(),
						 ControllerMatchPred(HQ_EC),
						 group);
	if (group.empty())
		return;
	if (m_pimpl->m_selection->GetSelection() == group)
	{
		m_pimpl->m_camera->FocusOnTerrain(group.front()->GetPosition());
	}
	else
	{
		m_pimpl->m_selection->SetSelection(group);
	}
}

void RDNUIProxy::SelectNextSubSelect()
{
	m_pimpl->m_selection->NextSubSelection();
}

void RDNUIProxy::Update()
{
	// messages
	if (m_pimpl->m_message)
	{
		// static vector to avoid memory alloc
		static std::vector<unsigned char> msg;
		unsigned long sender = 0;

		if (m_pimpl->m_message->MessageRetrieve(msg, sender))
		{
			DispatchMessage(msg, sender);
		}
	}

	//
	if (m_pimpl->m_gameStartFlag)
	{
		LuaBinding::Call<void> c;
		c.Execute(m_pimpl->m_lua, "on_gamestart");

		m_pimpl->m_gameStartFlag = false;
	}

	//
	if (m_pimpl->m_playerLoseFlag)
	{
		LuaBinding::Call<void> c;
		c.Execute(m_pimpl->m_lua, "on_playerlose");

		m_pimpl->m_playerLoseFlag = false;
	}

	//
	if (m_pimpl->m_playerWinFlag)
	{
		LuaBinding::Call<void> c;
		c.Execute(m_pimpl->m_lua, "on_playerwin");

		m_pimpl->m_playerWinFlag = false;
	}

	return;
}

void RDNUIProxy::OnEvent(const GameEventSys::Event &ev)
{
	if (ev.GetType() == GE_GameStart)
	{
		// defer processing of this event
		m_pimpl->m_gameStartFlag = true;
	}
	else if (ev.GetType() == GE_PlayerKilled)
	{
		// check to see if the local player is killed; if so, we lost
		const GameEvent_PlayerKilled &pk = static_cast<const GameEvent_PlayerKilled &>(ev);

		if (pk.m_killed == m_pimpl->m_sim->GetPlayer())
		{
			// defer processing of this event
			m_pimpl->m_playerLoseFlag = true;
		}
	}
	else if (ev.GetType() == GE_GameOver)
	{
		// the local player wins if he is still alive
		if (m_pimpl->m_sim->GetPlayer() &&
				m_pimpl->m_sim->GetPlayer()->IsPlayerDead() == 0)
		{
			// defer processing of this event
			m_pimpl->m_playerWinFlag = true;
		}
	}

	return;
}

void RDNUIProxy::LuaReset()
{
	m_pimpl->m_exported.clear();
}

void RDNUIProxy::SendMessageToAll(const unsigned char *msg, unsigned int msgLen) const
{
	// fast out
	if (!m_pimpl->m_message)
		return;

	const RDNWorld *pWorld = m_pimpl->m_sim->GetWorld();
	const RDNPlayer *pLocalPlayer = m_pimpl->m_sim->GetPlayer();

	// get the other players
	int numPlayers = pWorld->GetPlayerCount();
	unsigned long *players = new unsigned long[numPlayers - 1];
	unsigned long localPlayerID = pLocalPlayer->GetID();

	int pIndex = 0;
	for (int i = 0; i < numPlayers; i++)
	{
		const Player *pPlayer = pWorld->GetPlayerAt(i);
		if (pPlayer->GetID() != localPlayerID)
		{
			players[pIndex] = pPlayer->GetID();
			pIndex++;
		}
	}

	// send message
	m_pimpl->m_message->MessageSend(msg,
																	msgLen,
																	players,
																	numPlayers - 1);

	delete[] players;
}

void RDNUIProxy::DispatchMessage(const std::vector<unsigned char> &msg, unsigned long sender)
{
	UNREF_P(msg);

	const RDNWorld *pWorld = m_pimpl->m_sim->GetWorld();
	const RDNPlayer *pLocalPlayer = m_pimpl->m_sim->GetPlayer();
	const Player *senderPlayer = pWorld->GetPlayerFromID(sender);

	// ignore messages from enemies
	if (senderPlayer != pLocalPlayer)
	{
		return;
	}

	/***
	const unsigned long magic = *reinterpret_cast<const unsigned long*>( &msg[0] );				
	switch (magic)
	{
	default:
		dbPrintf("Unknown message received (%d)", magic);
		break;
	}
***/
}

void RDNUIProxy::LoadUIOptions()
{
	//
	m_pimpl->m_pRDNUIOptions->Load();
	m_pimpl->m_pRDNUIOptions->ApplyOptions();
	m_pimpl->m_pRDNUIOptions->Save();
}

void RDNUIProxy::Preload()
{
	// preload all fx used by the game
	const RDNTuning::EffectInfo &inf = RDNTuning::Instance()->GetEffectInfo();

#define PRELOAD(t) \
	m_pimpl->m_fx->FXPreload(inf.t.fx)

	PRELOAD(impact);

#undef PRELOAD

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNUIProxy::OnCinematicMode(bool bCinematic)
{
	m_pimpl->m_bCinematic = bCinematic;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNUIProxy::OnCharacterTalk(unsigned long entityID, bool bTalk)
{
	// get entity
	const EntityFactory *ef = m_pimpl->m_sim->GetWorld()->GetEntityFactory();
	const Entity *pEntity = ef->GetEntityFromEID(entityID);

	if (pEntity)
	{
		// the talk motion variable controls the talking animation;
		// 0-0.5 = no talking; 0.5-1.0 = talking
		float talkVal = bTalk ? 0.75f : 0;
		pEntity->GetAnimator()->SetMotionVariable("Talk", talkVal);
	}
}