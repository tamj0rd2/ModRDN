/////////////////////////////////////////////////////////////////////
// File    : CommandProcessor.cpp
// Desc    :
// Created : Monday, January 28, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"

#include "CommandProcessor.h"

#include "../ModObj.h"
#include "RDNWorld.h"
#include "RDNPlayer.h"

#include "Controllers/ModController.h"

#include "Extensions/HealthExt.h"
#include "Extensions/ModifierExt.h"
#include "Extensions/UnitSpawnerExt.h"

#include "ExtInfo/MovingExtInfo.h"
#include "ExtInfo/HealthExtInfo.h"

#include "States/StateDead.h"
#include "States/StateGather.h"
#include "States/StateMove.h"
#include "States/StateIdle.h"
#include "States/StateAttack.h"
#include "States/StateAttackMove.h"
#include "States/StateGroupMove.h"
#include "States/StatePause.h"

#include "RDNQuery.h"

#include "CommandTypes.h"

#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityCommand.h>

#include <Util/Iff.h>

#include <ModInterface/ECStaticInfo.h>

/////////////////////////////////////////////////////////////////////
// Local functions

#define GETSTATEENTITY(entity, state) static_cast<const state *>(static_cast<const ModController *>(entity->GetController())->QIStateAll(state::StateID))

#define GETSTATE(state) static_cast<state *>(m_pMC->QIStateAll(state::StateID))
#define GETEXT(ext) static_cast<ext *>(m_pMC->QIAll(ext::ExtensionID))

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
CommandProcessor::CommandProcessor() : m_bIsDead(false),
																			 m_pMC(NULL)
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
CommandProcessor::~CommandProcessor()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::Init(ModController *pMC)
{
	dbAssert(m_pMC == NULL);
	m_pMC = pMC;
	dbAssert(m_pMC);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::MakeDead()
{
	// If we are already dead then just return
	if (m_bIsDead)
		return;

	m_bIsDead = true;

	State *pCurState = m_pMC->QIStateAll(State::SID_Current);

	if (pCurState)
	{
		pCurState->ForceExit();

		StateDead *pStateDead = GETSTATE(StateDead);
		dbAssert(pStateDead);
		pStateDead->Enter();
		m_pMC->SetActiveState(State::SID_Dead);
	}
	else
	{
		// state should only ever be null if the entity isnt' spawned
		dbAssert(!m_pMC->GetEntity()->GetEntityFlag(EF_IsSpawned));
	}

	// Flush any active modifiers
	ModifierExt *pModExt = GETEXT(ModifierExt);
	if (pModExt)
		pModExt->FlushModifiers();

	// update flags
	m_pMC->GetEntity()->ClearEntityFlag(EF_Selectable);

	// check to see if we should remove the entity from pathfinding when it dies
	bool bRemPathfinding = true;
	const HealthExtInfo *healthExtInfo = QIExtInfo<HealthExtInfo>(m_pMC);
	if (healthExtInfo && !healthExtInfo->fadeAndDeleteWhenDead && healthExtInfo->stayInPathfindingAfterDead)
	{
		bRemPathfinding = false;
	}

	if (bRemPathfinding)
	{
		// yuck : this is dubious, watch out for bugs caused by this

		// remove entity from pathfinding when it dies
		ModObj::i()->GetWorld()->RemEntityPathfinding(m_pMC->GetEntity());

		// entity cant collide after it is dead
		m_pMC->GetEntity()->ClearEntityFlag(EF_CanCollide);
	}

	//	Inform all the extensions we care about that they are now dead
#define ONDEAD(Ext)          \
	Ext *p##Ext = GETEXT(Ext); \
	if (p##Ext)                \
		p##Ext->OnDead();

	ONDEAD(UnitSpawnerExt);

#undef ONDEAD
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::Save(IFF &iff) const
{
	iff.PushChunk(Type_NormalVers, 'CMDP', 1);

	IFFWrite(iff, m_bIsDead);

	iff.PopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::Load(IFF &iff)
{
	iff.AddParseHandler(HandleCMDP, Type_NormalVers, 'CMDP', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long CommandProcessor::HandleCMDP(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	CommandProcessor *pCommandProcessor = static_cast<CommandProcessor *>(pContext1);
	dbAssert(pCommandProcessor);

	IFFRead(iff, pCommandProcessor->m_bIsDead);

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::OnSpawnEntity()
{
	// state
	State *s = m_pMC->QIActiveState();

	if (s)
		s->ForceExit();

	// the state has been force exited therefore we have no current state.
	m_pMC->SetActiveState(State::SID_NULLState);
	dbAssert(m_pMC->QIActiveState() == NULL);

	ToStateIdle();

	dbAssert(m_pMC->QIActiveState());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::OnDeSpawnEntity()
{
	// state
	State *s = m_pMC->QIActiveState();

	if (s)
	{
		s->ForceExit();

		m_pMC->SetActiveState(State::SID_NULLState);
		dbAssert(m_pMC->QIActiveState() == NULL);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Validates the given command
//	Result	: returns true if the command is valid
//	Param.	: pEntCmd - the command
//
bool CommandProcessor::ValidateCommand(const EntityCommand *pEntCmd)
{
	switch (pEntCmd->GetCommandType())
	{
	case EntityCommand::CT_EntityEntity:
	{
		// An Entity-Entity Command.
		const EntityCommand_EntityEntity *pEntCmd_EE = static_cast<const EntityCommand_EntityEntity *>(pEntCmd);

		if (pEntCmd_EE->GetTargets().empty())
		{
			return false;
		}
		else if (!pEntCmd_EE->GetTargets().front()->GetEntityFlag(EF_CanCollide))
		{
			// target is non-collidable
			return false;
		}
		break;
	}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    : return true if the command was processed and should not be stored
// Param.    :
// Author    :
//
bool CommandProcessor::CommandDoProcessNow(const EntityCommand *pEntCmd)
{
	dbTracef("CommandProcessor::CommandDoProcessNow");

	// grab the current state
	State *pCurState = m_pMC->QIActiveState();
	dbAssert(pCurState);

	// make sure the given command is valid
	if (pEntCmd && !ValidateCommand(pEntCmd))
	{
		// command is invalid
		return true;
	}

	// If the current state rejects these commands, then we eat the command
	if (pEntCmd && pCurState->AcceptCommand(pEntCmd->GetCommand()) == false)
		return true;

	// early out for queued commands
	if (pEntCmd->GetFlags() & CMDF_Queue)
	{
		// if we process the queued command now then don't store it.
		if (ProcessQueuedCommandNow(pEntCmd))
		{
			return true;
		}

		// Let the state know that a command is queued, if it wants to exit it can
		pCurState->SoftExit();

		return false;
	}

	// set to true by default
	bool processed = true;

	// By default all commands that aren't processed will ask the current state to exit
	bool bRequestExit = true;

	switch (pEntCmd->GetCommandType())
	{
	case EntityCommand::CT_Entity:
	{
		unsigned long command = pEntCmd->GetCommand();
		// An Entity Command.
		switch (command)
		{
		case CMD_Destroy:
			SelfDestruct();
			break;
		case CMD_Stop:
			// Request that the current state exits.
			pCurState->RequestExit();
			// let the command fall through so that it clears the queue
			processed = false;
			break;
		case CMD_Pause:
		{
			if (HasState(State::SID_Pause))
			{
				if (pCurState->GetStateID() == State::SID_Pause)
				{
					StatePause *pPauseState = static_cast<StatePause *>(pCurState);
					pPauseState->IncrementPauseCount();
				}
				else
				{
					processed = false;
				}
			}
		}
		break;
		case CMD_UnPause:
		{
			if (pCurState->GetStateID() == State::SID_Pause)
			{
				// exit the pause state now
				pCurState->RequestExit();
			}
		}
		break;
		}
	}
	break;

	case EntityCommand::CT_EntityPoint:
	{
		const EntityCommand_EntityPoint *pEntCmd_EP = static_cast<const EntityCommand_EntityPoint *>(pEntCmd);
		unsigned long command = pEntCmd->GetCommand();
		// An Entity-Point Command.
		switch (command)
		{
		case CMD_DefaultAction:
		case CMD_Move:
			if (HasState(State::SID_Move))
			{
				processed = false;
			}
			break;
		case CMD_Attack:
		case CMD_AttackMove:
			if (HasState(State::SID_AttackMove))
			{
				processed = false;
			}
			else if (HasState(State::SID_Move))
			{
				processed = false;
			}
			break;
		case CMD_RallyPoint:
		{
			UnitSpawnerExt *pUnitSpawnerExt = QIExt<UnitSpawnerExt>(m_pMC);
			if (pUnitSpawnerExt && pEntCmd_EP->GetPosCount() > 0)
			{
				pUnitSpawnerExt->SetRallyTarget(pEntCmd_EP->GetPosAt(0));
			}

			processed = true;
		}
		break;
		}
	}
	break;

	case EntityCommand::CT_EntityEntity:
	{
		unsigned long command = pEntCmd->GetCommand();
		// An Entity-Entity Command.
		const EntityCommand_EntityEntity *pEntCmd_EE = static_cast<const EntityCommand_EntityEntity *>(pEntCmd);

		if (command == CMD_DefaultAction)
		{
			// translate command into an actual command from the default
			command = GetDefaultEntityEntityCommand(m_pMC->GetEntity(), pEntCmd_EE->GetTargets().front());
		}

		dbTracef("CommandProcessor::CommandDoProcessNow EntityEntity %d", command);

		// An Entity-Entity Command.
		switch (command)
		{
		case CMD_Move:
			if (HasState(State::SID_Move))
			{
				// were we told to move to ourself?
				if (pEntCmd_EE->GetTargets().front() == m_pMC->GetEntity())
				{
					// discard this command
					processed = true;
				}
				else
				{
					processed = false;
				}
			}
			break;
		case CMD_Attack:
			if ((HasState(State::SID_Attack)) &&
					RDNQuery::CanAttack(m_pMC->GetEntity(), pEntCmd_EE->GetTargets().front(), false))
			{
				processed = false;
			}
			break;
		case CMD_AttackMove:
			if (HasState(State::SID_AttackMove))
			{
				processed = false;
			}
			break;
		case CMD_Gather:
		{
			if (HasState(State::SID_Gather))
			{
				processed = false;
			}
			break;
		}
		case CMD_RallyPoint:
		{
			UnitSpawnerExt *pUnitSpawnerExt = QIExt<UnitSpawnerExt>(m_pMC);
			if (pUnitSpawnerExt && !pEntCmd_EE->GetTargets().empty())
			{
				pUnitSpawnerExt->SetRallyTarget(pEntCmd_EE->GetTargets().front());
			}
		}
		break;
		}
	}
	break;

	default:
		// unhandled case
		dbFatalf("CommandProcessor::CommandDoProcessNow unhandled case");
	}

	// if we don't want to process the command now
	// then the current state should exit
	if (processed == false && bRequestExit)
	{
		pCurState->RequestExit();
	}

	// if we get here the command was processed and shouldn't be
	// added to the queue
	dbTracef("CommandProcessor::CommandDoProcessNow end");
	return processed;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::CommandIsClearQueue(const EntityCommand *pEntCmd) const
{
	dbTracef("CommandProcessor::CommandIsClearQueue");

	// Un-Pause commands never clear the queue
	if (pEntCmd->GetCommandType() == EntityCommand::CT_Entity)
	{
		if (pEntCmd->GetCommand() == CMD_UnPause)
		{
			return false;
		}
	}

	if (pEntCmd->GetFlags() & CMDF_Queue)
	{
		// don't clear the queue
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::Update(const EntityCommand *pEntCmd)
{
	if (pEntCmd)
	{
		dbTracef("CommandProcessor::Update");
	}

	// grab the current state
	State *pCurState = m_pMC->QIActiveState();
	dbAssert(pCurState);

	// make sure the given command is valid
	if (pEntCmd && !ValidateCommand(pEntCmd))
	{
		// command is invalid
		return true;
	}

	if (pEntCmd && pCurState->AcceptCommand(pEntCmd->GetCommand()) == false)
		return true;

	//
	ModObj::i()->GetWorld()->CumulateStateTimeBegin();

	bool bStateDone = pCurState->Update();

	//
	ModObj::i()->GetWorld()->CumulateStateTimeEnd();

	if (!bStateDone)
	{
		// If the state is still processing check if the command passed in should
		// be processed now
		if (pEntCmd && EatUpdateCommand(pEntCmd))
		{
			return true;
		}

		return false;
	}

	m_pMC->SetActiveState(State::SID_NULLState);
	dbAssert(m_pMC->QIActiveState() == NULL);

	if (pEntCmd)
	{
		switch (pEntCmd->GetCommandType())
		{
		case EntityCommand::CT_Entity:
		{
			unsigned long command = pEntCmd->GetCommand();
			switch (command)
			{
			case CMD_Pause:
			{
				if (HasState(State::SID_Pause))
				{
					// if we aren't set to accept commands then ignore them.
					ToStatePause(pEntCmd->GetParam() == 0);
				}
			}
			break;
			}
		}
		break;
		case EntityCommand::CT_EntityPoint:
		{
			unsigned long command = pEntCmd->GetCommand();
			// An Entity-Point Command.
			const EntityCommand_EntityPoint *pEntCmd_EP = static_cast<const EntityCommand_EntityPoint *>(pEntCmd);

			switch (command)
			{
			// Default Point Action for anything with a move State is to move to the location specified
			case CMD_DefaultAction:
			case CMD_Move:
				if (HasState(State::SID_Move))
				{
					ToStateMove(*pEntCmd_EP->GetPos(), pEntCmd->GetEntities(), pEntCmd->GetProcessedEntities(), pEntCmd_EP->GetParam());
				}
				break;
			case CMD_Attack:
			case CMD_AttackMove:
				if (HasState(State::SID_AttackMove))
				{
					ToStateAttackMove(*pEntCmd_EP->GetPos());
				}
				else if (HasState(State::SID_Move))
				{
					ToStateMove(*pEntCmd_EP->GetPos(), pEntCmd->GetEntities(), pEntCmd->GetProcessedEntities(), pEntCmd_EP->GetParam());
				}
				break;
			}
		}
		break;

		case EntityCommand::CT_EntityEntity:
		{
			// An Entity-Entity Command.
			const EntityCommand_EntityEntity *pEntCmd_EE = static_cast<const EntityCommand_EntityEntity *>(pEntCmd);

			// If there are now targets then we can't process it, now can we
			if (pEntCmd_EE->GetTargets().front() == NULL)
				break;

			unsigned long command = pEntCmd->GetCommand();
			if (command == CMD_DefaultAction)
			{
				// An Entity-Entity Command.
				const EntityCommand_EntityEntity *pEntCmd_EE = static_cast<const EntityCommand_EntityEntity *>(pEntCmd);

				command = GetDefaultEntityEntityCommand(m_pMC->GetEntity(), pEntCmd_EE->GetTargets().front());
			}

			switch (command)
			{
			case CMD_Move:
				if (HasState(State::SID_Move))
				{
					ToStateMove(const_cast<Entity *>(pEntCmd_EE->GetTargets().front()), pEntCmd_EE->GetParam());
				}
				break;
			case CMD_Attack:
				if (HasState(State::SID_Attack))
				{
					ToStateAttack(const_cast<Entity *>(pEntCmd_EE->GetTargets().front()));
				}
				break;
			case CMD_AttackMove:
				if (HasState(State::SID_AttackMove))
				{
					ToStateAttackMove(const_cast<Entity *>(pEntCmd_EE->GetTargets().front()));
				}
				break;
			case CMD_Gather:
				if (HasState(State::SID_Gather))
				{
					ToStateGather(const_cast<Entity *>(pEntCmd_EE->GetTargets().front()));
				}
				break;
			}
		}
		break;
		}

		dbTracef("CommandProcessor::Update finished");
	}

	if (m_pMC->QIActiveState() == NULL)
	{
		ToStateIdle();
	}

	dbAssert(m_pMC->QIActiveState());
	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::SelfDestruct()
{
	HealthExt *pHealth = GETEXT(HealthExt);
	if (pHealth)
	{
		// controllers with health ext should call the MakeDead in their
		// health gone notification callback
		pHealth->SelfDestroy();
	}
	else
	{
		MakeDead();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::HasState(unsigned char StateID)
{
	return (m_pMC->QIStateAll(StateID) != NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::ProcessQueuedCommandNow(const EntityCommand *pEntCmd)
{
	dbTracef("CommandProcessor::ProcessQueuedCommandNow");

	// grab the current state
	State *pCurState = m_pMC->QIActiveState();
	dbAssert(pCurState);

	switch (pEntCmd->GetCommandType())
	{
	case EntityCommand::CT_Entity:
	{
		unsigned long command = pEntCmd->GetCommand();

		if (command == CMD_Stop)
		{
			// Request that the current state exits.
			m_pMC->QIActiveState()->RequestExit();
			return true;
		}
		else if (command == CMD_UnPause)
		{
			if (pCurState->GetStateID() == State::SID_Pause)
			{
				// exit the pause state now
				pCurState->RequestExit();
			}
			return true;
		}
	}
	break;

	case EntityCommand::CT_EntityPoint:
	{
	}
	break;

	case EntityCommand::CT_EntityEntity:
	{
	}
	break;

	default:
		// unhandled case
		dbFatalf("ComandProcessor::ProcessQueuedCommandNow unhandled case");
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : This give us the chance to eat an Update Command
//			   We do this for queued patrol and move commands
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::EatUpdateCommand(const EntityCommand *pEntCmd)
{
	dbTracef("CommandProcessor::EatUpdateCommand");

	unsigned long command = pEntCmd->GetCommand();

	switch (pEntCmd->GetCommandType())
	{
	case EntityCommand::CT_Entity:
	{
	}
	break;

	case EntityCommand::CT_EntityPoint:
	{
		if ((command == CMD_Move || command == CMD_DefaultAction) && QIState<StateMove>(m_pMC))
		{
			// creature is patrolling and we received a queue patrolling command -> add the patrol point to the current command
			const EntityCommand_EntityPoint *pEntCmd_EP = static_cast<const EntityCommand_EntityPoint *>(pEntCmd);

			// add the new position to the patrol state
			dbAssert(pEntCmd_EP->GetPosCount() == 1);
			QIState<StateMove>(m_pMC)->AddWayPoint(pEntCmd_EP->GetPosAt(0));

			return true;
		}
		else if ((command == CMD_Move || command == CMD_DefaultAction) && QIState<StateGroupMove>(m_pMC))
		{
			// creature is patrolling and we received a queue patrolling command -> add the patrol point to the current command
			const EntityCommand_EntityPoint *pEntCmd_EP = static_cast<const EntityCommand_EntityPoint *>(pEntCmd);

			// add the new position to the patrol state
			dbAssert(pEntCmd_EP->GetPosCount() == 1);
			QIState<StateGroupMove>(m_pMC)->AddWayPoint(pEntCmd_EP->GetPosAt(0));

			return true;
		}
	}
	break;

	case EntityCommand::CT_EntityEntity:
	{
	}
	break;

	default:
		// unhandled case
		dbTracef("CommandProessor::EatUpdateCommand unhandled case");
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::ToStateIdle()
{
	// Transition to state Idle, we process a few different idle state's here
	// in order of priority
	if (HasState(State::SID_Idle))
	{
		StateIdle *pIdle = GETSTATE(StateIdle);
		dbAssert(pIdle);
		pIdle->Enter();

		m_pMC->SetActiveState(StateIdle::SID_Idle);
	}
	else
	{
		// if anyting is using the generic state update transition system
		// it must have an appropriate idle state
		dbFatalf("MOD -- Failed to transition to State Idle CType %i", m_pMC->GetControllerType());
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::ToStateMove(const Vec3f &position, const EntityGroup &commandGroup, const EntityGroup &processedGroup, unsigned long flags)
{
	StateGroupMove *pStateGroupMove = GETSTATE(StateGroupMove);

	size_t commandSize = commandGroup.size() + processedGroup.size();
	if (pStateGroupMove && commandSize > 1)
	{
		pStateGroupMove->Enter(position, 0, flags);
		m_pMC->SetActiveState(StateGroupMove::StateID);
	}
	else
	{
		StateMove *pStateMove = GETSTATE(StateMove);
		dbAssert(pStateMove);

		pStateMove->Enter(position, 0.0f, flags, StateMove::RT_BlockedOnEntityAllowSpace, 4);
		m_pMC->SetActiveState(StateMove::StateID);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::ToStateMove(Entity *target, unsigned long flags)
{
	if (target == NULL)
		return;

	StateMove *pStateMove = GETSTATE(StateMove);
	dbAssert(pStateMove);

	float AP = 0.0f;
	const MovingExtInfo *pMEI = QIExtInfo<MovingExtInfo>(m_pMC);
	if (pMEI)
	{
		AP = pMEI->entityMoveAP;
	}

	pStateMove->Enter(target, AP, true, flags, StateMove::RT_BlockedOnEntity, 4);
	m_pMC->SetActiveState(StateMove::StateID);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: clee
//
void CommandProcessor::ToStateAttack(Entity *entity)
{
	if (entity == NULL)
		return;

	if (HasState(State::SID_Attack))
	{
		StateAttack *pAttack = GETSTATE(StateAttack);
		dbAssert(pAttack);
		pAttack->Enter(entity);

		m_pMC->SetActiveState(StateIdle::SID_Attack);
	}
	else
	{
		// Can't attack
		dbFatalf("MOD -- Failed to transition to State Attack CType %i", m_pMC->GetControllerType());
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::ToStateAttackMove(const Vec3f &pos)
{
	StateAttackMove *pStateAM = GETSTATE(StateAttackMove);
	dbAssert(pStateAM);

	pStateAM->Enter(pos, 0.0f);
	m_pMC->SetActiveState(StateAttackMove::StateID);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void CommandProcessor::ToStateAttackMove(Entity *entity)
{
	StateAttackMove *pStateAM = GETSTATE(StateAttackMove);
	dbAssert(pStateAM);
	dbAssert(entity);

	pStateAM->Enter(entity->GetPosition(), 0.0f);
	m_pMC->SetActiveState(StateAttackMove::StateID);
}

// Pause state
void CommandProcessor::ToStatePause(bool bIgnoreCmds)
{
	StatePause *pStatePause = GETSTATE(StatePause);
	dbAssert(pStatePause);

	pStatePause->Enter(bIgnoreCmds);
	m_pMC->SetActiveState(StatePause::StateID);
}

void CommandProcessor::LeaveStatePause()
{
	StatePause *pStatePause = GETSTATE(StatePause);
	dbAssert(pStatePause);

	pStatePause->RequestExit();
}

// Gather state
void CommandProcessor::ToStateGather(const Entity *pResourceEntity)
{
	StateGather *pStateGather = GETSTATE(StateGather);
	dbAssert(pStateGather);

	pStateGather->Enter(pResourceEntity);
	m_pMC->SetActiveState(StateGather::StateID);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long CommandProcessor::GetDefaultEntityEntityCommand(const Entity *pMe, const Entity *pTe)
{
	dbTracef("CommandProcessor::GetDefaultEntityEntityCommand Controller 1: %s | Controller 2: %s", pMe->GetControllerBP()->GetFileName(), pTe->GetControllerBP()->GetFileName());

	if (!pMe->GetController() || !pTe->GetController())
	{
		return CMD_DefaultAction;
	}

	dbTracef("CommandProcessor::GetDefaultEntityEntityCommand Determining which action to use");

	if (RDNQuery::CanAttack(pMe, pTe))
	{
		if (GETSTATEENTITY(pMe, StateAttack))
		{
			return CMD_Attack;
		}
	}

	if (RDNQuery::CanGather(pMe, pTe))
	{
		dbTracef("Can gather :D");
		return CMD_Gather;
	}

	// Fallback if all cases above fail
	if (GETSTATEENTITY(pMe, StateMove))
	{
		return CMD_Move;
	}

	return CMD_DefaultAction;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::CanDoCommand(const Entity *pMe, unsigned long command, unsigned long param)
{
	UNREF_P(param);
	dbTracef("CommandProcessor::CanDoCommand");

	// grab the current state
	if (!pMe->GetController())
		return false;

	const State *pCurState = static_cast<const ModController *>(pMe->GetController())->QIActiveState();
	dbAssert(pCurState);

	if (!pCurState)
		return false;

	switch (command)
	{
	case CMD_Destroy:
		if (GETSTATEENTITY(pMe, StateDead))
			return true;
		break;
	case CMD_Stop:
		return true;
		break;
	case CMD_Move:
		if (GETSTATEENTITY(pMe, StateMove))
		{
			return true;
		}
		break;
	case CMD_Attack:
	case CMD_AttackMove:
		if (GETSTATEENTITY(pMe, StateAttackMove))
		{
			return true;
		}
		else if (GETSTATEENTITY(pMe, StateMove))
		{
			return true;
		}
		break;
	case CMD_Gather:
		if (GETSTATEENTITY(pMe, StateGather))
		{
			return true;
		}
		break;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool CommandProcessor::CanDoCommand(const Entity *pMe, const Entity *pTarget, unsigned long command, unsigned long param)
{
	dbTracef("CommandProcessor::CanDoCommand overload");
	UNREF_P(param);

	// grab the current state
	if (!pMe->GetController())
		return false;

	const State *pCurState = static_cast<const ModController *>(pMe->GetController())->QIActiveState();
	dbAssert(pCurState);

	if (!pCurState)
		return false;

	if (command == CMD_DefaultAction)
	{
		// translate command into an actual command from the default
		command = GetDefaultEntityEntityCommand(pMe, pTarget);
	}

	switch (command)
	{
	case CMD_Move:
		if (GETSTATEENTITY(pMe, StateMove))
		{
			// were we told to move to ourself?
			if (pMe != pTarget)
			{
				return true;
			}
		}
		break;
	case CMD_Attack:
		if (GETSTATEENTITY(pMe, StateAttack) &&
				RDNQuery::CanAttack(pMe, pTarget))
		{
			return true;
		}
		break;
	case CMD_AttackMove:
		if (GETSTATEENTITY(pMe, StateAttackMove))
		{
			return true;
		}
		break;
	}

	return false;
}
