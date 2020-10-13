/////////////////////////////////////////////////////////////////////
// File    : StateAttack.cpp
// Desc    :
// Created : Saturday, September 15, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateAttack.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../RDNPlayer.h"
#include "../PlayerFOW.h"
#include "../RDNQuery.h"
#include "../RDNTuning.h"
#include "../CommandTypes.h"
#include "../AttackPackage.h"

#include "../Controllers/ModController.h"

#include "../Extensions/HealthExt.h"
#include "../Extensions/AttackExt.h"
#include "../Extensions/ModifierExt.h"

#include "StateMove.h"

#include <SimEngine/Player.h>
#include <SimEngine/Entity.h>
#include <SimEngine/EntityUtil.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/GroundDynamics.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <Util/Biff.H>
#include <Util/Iff.H>

#include <EngineAPI/SoundInterface.h>
#include <EngineAPI/EntityFactory.h>

#include <Math/MiscMath.h>

#include <SurfVol/OBB3.h>

#include <Assist/StlExSmallVector.h>

//////////////////////////////////////////////////////////////////////////////////////
// Attack
//////////////////////////////////////////////////////////////////////////////////////

StateAttack::StateAttack(EntityDynamics *e_dynamics)
		: State(e_dynamics),
			m_CurState(AS_Null),
			m_pStateMove(NULL),
			m_pAttack(NULL),
			m_pAttackInf(0),
			m_pAttackCurrent(0),
			m_exitState(AES_Invalid),
			m_AttackCount(0)
{
	return;
}

void StateAttack::Init(StateMove *pMove, AttackExt *pAttack, const AttackInfoPackage *pAttackInf)
{
	// validate parm
	dbAssert(pMove);
	dbAssert(pAttack);
	dbAssert(pAttackInf);

	// check for duplicate initialization
	dbAssert(m_pStateMove == 0);
	dbAssert(m_pAttack == 0);
	dbAssert(m_pAttackInf == 0);
	dbAssert(m_pAttackCurrent == 0);

	// init fields
	m_pStateMove = pMove;
	m_pAttack = pAttack;
	m_pAttackInf = pAttackInf;
	m_pAttackCurrent = NULL;

	return;
}

/////////////////////////////////////////////////////////////////////
//	Result	: returns the EntityGroup of target(s)
//
const Entity *StateAttack::GetTargetEntity() const
{
	return m_Target.front();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns the reason for the move state exiting
//	Result	: returns MoveExitState
//
StateAttack::AttackExitState StateAttack::GetExitState()
{
	//	dbAssert( m_exitState != AES_Invalid );

	return m_exitState;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Enter the Attack state
//	Param.	: pTarget - the Entity to attack
//
void StateAttack::Enter(Entity *pTarget)
{
	// clear previous targets
	m_Target.clear();

	m_exitState = AES_Invalid;

	//
	if (pTarget == 0)
	{
		// oops!
		dbBreak();
		SetExitStatus(true);
		m_CurState = AS_TargetDead;

		return;
	}
	else if (!pTarget->GetEntityFlag(EF_CanCollide))
	{
		m_CurState = AS_TargetDead;
		SetExitStatus(true);

		return;
	}
	else if (m_pAttackInf && m_pAttackInf->hasAttack)
	{
		m_Target.push_back(pTarget);
	}

	// move to target if need be
	m_bNoMove = false;

	if (!GetTarget())
	{
		SetExitStatus(true);
		m_CurState = AS_TargetDead;
		return;
	}
	else
	{
		Entity *pEntity = GetEntity();
		if (pEntity->GetAnimator())
		{
			pEntity->GetAnimator()->SetTargetLook(pTarget);
		}
	}

	m_FinishTime = ModObj::i()->GetWorld()->GetGameTicks() + (unsigned long)(4 * ModObj::i()->GetWorld()->GetRand());
	m_CurState = AS_RandomWait;

	SetExitStatus(false);
	m_lastDistSqr = 0.0f;

	m_AttackCount = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Enter the Attack state without the Entity moving
//	Param.	: pTarget - the Entity to attack
//
void StateAttack::EnterNoMove(Entity *pTarget)
{
	Enter(pTarget);

	// over ride the no move flag
	m_bNoMove = true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: The state update function
//	Result	: returns true if the state is exiting
//
bool StateAttack::Update()
{
	if (UpdateInternal())
	{
		m_CurState = AS_Null;

		Entity *pEntity = GetEntity();
		if (pEntity->GetAnimator())
		{
			pEntity->GetAnimator()->SetTargetLook(NULL);
		}

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: does the common RequestExit work
//
void StateAttack::ProcessRequestExit()
{
	SetExitStatus(true);

	switch (m_CurState)
	{
	case AS_TransitionToChasingTarget:
	case AS_MovingTo_Target:
	case AS_Chasing_Target:
	case AS_Reaquire:
	case AS_CoolPeriod:
		m_pStateMove->RequestExit();
		break;
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//
void StateAttack::RequestExit()
{
	m_exitState = AES_ExitRequested;
	ProcessRequestExit();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void StateAttack::ReissueOrder() const
{
	if (!m_Target.empty())
	{
		const unsigned long playerID = GetEntity()->GetOwner() ? GetEntity()->GetOwner()->GetID() : 0;
		const unsigned long entityID = GetEntity()->GetID();
		const unsigned long targetID = m_Target.front()->GetID();

		ModObj::i()->GetWorld()->DoCommandEntityEntity(CMD_Attack,
																									 0,
																									 CMDF_Queue,
																									 playerID,
																									 &entityID,
																									 1,
																									 &targetID,
																									 1);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StateAttack::ForceExit()
{
	m_exitState = AES_ExitRequested;

	switch (m_CurState)
	{
	case AS_TransitionToChasingTarget:
	case AS_MovingTo_Target:
	case AS_Chasing_Target:
	case AS_Reaquire:
	case AS_CoolPeriod:
		m_pStateMove->ForceExit();

		// don't leave the creature 'LEAP'ing
		if (GetEntity()->GetAnimator())
			GetEntity()->GetAnimator()->SetStyle('MOVE');
		break;
	}

	m_CurState = AS_Null;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State::StateIDType StateAttack::GetStateID() const
{
	return (State::StateIDType)StateID;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StateAttack::SaveState(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	unsigned long ver = 2;
	IFFWrite(iff, ver);

	m_Target.SaveEmbedded(iff);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StateAttack::LoadState(IFF &iff)
{
	unsigned long ver;
	IFFRead(iff, ver);

	Entity *pEntity = NULL;

	if (ver == 1)
	{
		Target temp;
		temp.LoadEmbedded(iff, ModObj::i()->GetEntityFactory());
		pEntity = temp.GetEntity();
	}

	if (ver > 1)
	{
		EntityGroup temp;
		temp.LoadEmbedded(iff, ModObj::i()->GetEntityFactory());
		pEntity = temp.front();
	}

	// Re - Initialize the state
	Enter(pEntity);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool StateAttack::UpdateInternal()
{
#pragma FIXME(Drew) // This is Fuxored
	if (m_pAttackInf && m_pAttackInf->hasAttack == 0)
	{
		m_exitState = AES_Invalid;
		return true;
	}

	Entity *pTarget = GetTarget();

	// Check if Target is alive.
	if (!pTarget)
	{
		m_exitState = AES_TargetDead;
		ProcessRequestExit();
	}

	// This is the attack state update function.
	//	Here is the logic for moving toward the target, or attacking, etc....

	if (IsExiting())
	{
		bool bReturnVal = true;
		switch (m_CurState)
		{
		// These states need to wait for move to finish before exiting.
		case AS_TransitionToChasingTarget:
		case AS_MovingTo_Target:
		{
			bReturnVal = m_pStateMove->Update();
		}
		//
		case AS_Attacking:
			bReturnVal = true;
			break;
		//
		case AS_CoolPeriod:
			bReturnVal = false;
			// we can't break off the coolperiod until it is done because if micromanaged this could lead to a player increasing a creature's rate of attack
			if (HandleCoolPeriod())
			{
				// cool is done

				// Note: StartReacquire() will only acquire the target during the sim tick.
				//       We won't start attacking the acquired target until the next sim tick.
				//       This is why m_FinishTime is computed the way it is in StartCoolPeriod().

				if (!StartReaquire())
				{
					// not doing a reaquire -> done!
					bReturnVal = true;
				}
			}
			break;

		//
		case AS_Reaquire:
		{
			// cant leave this until done or flyers will be able to attack more than every 4 seconds if they are micro-managed
			bReturnVal = ModObj::i()->GetWorld()->GetGameTicks() >= m_FinishTime;
		}
		}

		// Other states can just exit.
		dbAssert(!(bReturnVal && m_exitState == AES_Invalid));
		return bReturnVal;
	}

	switch (m_CurState)
	{
	case AS_RandomWait:
		if (ModObj::i()->GetWorld()->GetGameTicks() >= m_FinishTime)
		{
			MoveToTarget();
		}
		break;

	case AS_TransitionToChasingTarget:
	{
		if (m_pStateMove->Update())
		{
			// try chasing the target; we can only guess where the target may be... and not
			// chase after it indefinitely without actually seeing it
			Entity *pEntity = GetTarget();
			if (m_pAttackCurrent && pEntity)
			{
				m_CurState = AS_Chasing_Target;

				m_pStateMove->Enter(pEntity->GetPosition(), m_pAttackCurrent->m_maxRange);
			}
			else
			{
				m_CurState = AS_TargetDead;
			}
		}
		break;
	}

	case AS_MovingTo_Target:
	case AS_Chasing_Target:
	{
		if (m_pStateMove->Update())
		{ // move is done

			switch (m_pStateMove->GetExitState())
			{
			case StateMove::MES_ReachedTarget: // Got To the target
				if (m_CurState == AS_Chasing_Target)
				{
					if (!RDNQuery::CanBeSeen(pTarget, GetEntity()->GetOwner()))
					{
						// still can't see the target... oh well
						m_CurState = AS_TargetDead;
						break;
					}
					else
					{
						// make sure we are in range to continue the attack
						Entity *pEntity = GetTarget();
						m_pStateMove->Enter(pEntity, m_pAttackCurrent->m_maxRange);
						m_CurState = AS_MovingTo_Target;
						break;
					}
				}
				StartAttacking();
				break;
			case StateMove::MES_StoppedBeforeTarget: // We have stopped before reaching our target.
				MoveToTarget();
				break;
			case StateMove::MES_NoTarget:
			{
				m_exitState = AES_TargetDead;
			}
			case StateMove::MES_CantPathToTargetTerrain: // We cannot path to our target, blocked by terrain
			{
				m_exitState = AES_CantPathToTargetTerrain;
				return true;
			}
			case StateMove::MES_CantPathToTargetEntity: // We cannot path to our target, blocked by entities
			{
				m_exitState = AES_CantPathToTargetEntity;
				return true;
			}
			case StateMove::MES_CantPathToTargetBuilding: // We cannot path to our target, blocked by buildings
			{
				m_exitState = AES_CantPathToTargetBuilding;
				return true;
			}
			}
		}
	}
	break;

	case AS_Attacking:
		HandleAttacking();
		break;

	case AS_CoolPeriod:
		if (HandleCoolPeriod())
		{
			// Note: StartReacquire() will only acquire the target during the sim tick.
			//       We won't start attacking the acquired target until the next sim tick.
			//       This is why m_FinishTime is computed the way it is in StartCoolPeriod().

			if (!StartReaquire())
			{
				// not doing a reaquire
				MoveToTarget();
			}
		}
		break;

	case AS_Reaquire:
		if (ModObj::i()->GetWorld()->GetGameTicks() >= m_FinishTime)
		{
			MoveToTarget();
		}
		break;

	case AS_TargetDead:
	case AS_Cant_Reach_Target:
	{

		Entity *pEntity = static_cast<Entity *>(GetEntity());
		if (pEntity->GetAnimator())
			pEntity->GetAnimator()->SetStyle('MOVE');

		m_exitState = AES_TargetDead;
		return true;
	}
	break;
	}

	// Finished updating states.
	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//
void StateAttack::MoveToTarget()
{
	if (GetTarget())
	{
		// now we're choosing the attack before the move is done because we need to know how close to the target to get
		bool bValid = ChooseAttack();

		if (!bValid)
		{
			// no attack
			// this will cause the state to exit, which might not be what we want.  Maybe we want to go into a waiting substate until a valid attack become possible.
			m_CurState = AS_Cant_Reach_Target;
			return;
		}

		dbAssert(m_pAttackCurrent);

		if (!m_bNoMove)
		{
			m_CurState = AS_MovingTo_Target;
			Entity *pEntity = GetTarget();
			m_pStateMove->Enter(pEntity, m_pAttackCurrent->m_maxRange, false, 0, StateMove::RT_None, 0);

			m_lastDistSqr = 0.0f;
		}
		else
		{
			if (AtTargetForCurrentAttack())
			{
				StartAttacking();
			}
			else
			{
				m_CurState = AS_Cant_Reach_Target;
			}
		}
	}
	else
	{
		// Target is dead.
		m_CurState = AS_TargetDead;
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: this picks a current attack to use (melee/range) based on some basic logic
//	Result	: m_pAttackCurrent will be set
//
bool StateAttack::ChooseAttack()
{
	//
	const Entity *attacker = GetEntity();
	const Entity *defender = GetTarget();

	dbAssert(attacker && defender);

	// reset attack
	m_pAttackCurrent = NULL;

	//
	if (!m_pAttackCurrent && !m_pAttackInf->meleeList.empty())
	{
		// pick a melee attack here no matter what - it will be used as a back up in case of an error
		const AttackPackageList &melee = m_pAttackInf->meleeList;

		const size_t index =
				ModObj::i()->GetWorld()->GetRandMax(melee.size());

		m_pAttackCurrent = &m_pAttackInf->meleeList[index];
	}

	if (!m_pAttackCurrent)
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Starts reaquiring the target.  Right now only here for fliers.
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool StateAttack::StartReaquire()
{
	// Note: We should only attack the target we acquire here during the NEXT sim tick.
	//       If we change this behavior, we must also update the way m_FinishTime is computed
	//       in StartCoolPeriod().

	// Reaquire is only required when GetTurnAroundTicks() is > 0
	long turnTicks = 0;

	if (m_pAttackCurrent)
	{
		turnTicks = GetDynamics()->GetTurnAroundTicks(m_pAttackCurrent->m_attackticks, m_pAttackCurrent->m_coolticks);
	}

	if (turnTicks)
	{ // want to do the reaquire.  for fliers.
		m_CurState = AS_Reaquire;
		m_FinishTime = ModObj::i()->GetWorld()->GetGameTicks() + turnTicks;

		GetDynamics()->StartReaquire(m_pAttackCurrent->m_attackticks, m_pAttackCurrent->m_coolticks);

		return true;
	}
	else
	{ // don't bother with the reaquire.  for ground dudes.
		return false;
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//
void StateAttack::StartAttacking()
{
	// Perform the Attack.
	//	Note: Add offsets from the m_WaitStart to when the animations should start and stop.
	//	or at which game-tick should the health be removed from the creature, etc....

	// request that the Entity look at the target
	GetDynamics()->RequestEntityLookAtPoint(GetTarget()->GetPosition());

	m_CurState = AS_Attacking;

	dbAssert(m_pAttackCurrent);

	if (GetTarget())
	{
		//	Reveal self in FoW
		if (GetEntity()->GetOwner() != GetTarget()->GetOwner())
		{
			RDNPlayer *pPlayer = static_cast<RDNPlayer *>(GetTarget()->GetOwner());
			if (pPlayer)
			{
				pPlayer->GetFogOfWar()->RevealEntity(GetEntity(), RDNTuning::Instance()->GetFogOfWarInfo().attackerRevealTime);
			}
		}
	}

	// Set wait delay.
	m_FinishTime = ModObj::i()->GetWorld()->GetGameTicks() + m_pAttackCurrent->m_attackticks;

	// let the dynamics know the attack and cooldown duration
	GetDynamics()->StartAttack(m_pAttackCurrent->m_attackticks, m_pAttackCurrent->m_coolticks, m_pAttackCurrent->m_rangesqr > 0);

	// Start the attack animation, or setup a sim-tick time for the animation to start.
	//	HACK : signal the entity to play an attack animation
	Entity *pEntity = m_pStateMove->GetEntity();
	if (pEntity && pEntity->GetAnimator())
	{
		float needTime = ((float)m_pAttackCurrent->m_attackticks) / (float)(ModObj::i()->GetWorld()->GetNumSimsPerSecond());

		pEntity->GetAnimator()->ResetMotionTreeNode(m_pAttackCurrent->m_Animation);
		pEntity->GetAnimator()->SetMotionVariable("AttackTime", needTime);
		//	Tell the animator who his target is
		pEntity->GetAnimator()->SetEffectsTarget(GetTarget());
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//
void StateAttack::StartCoolPeriod()
{
	m_AttackCount++;

	// Set the length of the cool period.
	m_FinishTime = ModObj::i()->GetWorld()->GetGameTicks() + m_pAttackCurrent->m_coolticks;

	// Roll-back the finish-time by one tick to account for the fact that re-acquiring the target
	// delays by one tick.  See StartReaquire() and UpdateInternal().
	if (m_pAttackCurrent->m_coolticks > 0)
	{
		m_FinishTime--;
	}

	// let the dynamics know we are starting the cooldown
	GetDynamics()->StartCoolDown(m_pAttackCurrent->m_coolticks);

	m_CurState = AS_CoolPeriod;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns true if the entity is within range of the target for any of his attacks
//
bool StateAttack::AtTargetForAnyAttack()
{
	const SimEntity *attacker = static_cast<const SimEntity *>(GetEntity());
	const SimEntity *defender = static_cast<const SimEntity *>(GetTarget());
	dbAssert(attacker && defender);

	m_lastDistSqr = EntityUtil::DistSqrDirCalcEntity(attacker, defender, 0.0f, NULL);
	// also test against this abitrary melee distance ( should be done in a common place )
	if (m_lastDistSqr < 0.1f * 0.1f)
	{
		// within range of melee attack.
		return true;
	}

	float maxRangeSqr = 0.0f;

	if (m_pAttackInf)
	{
		if (m_pAttackInf->maxRange == 0.0f)
		{
			// This creature only has melee.
			return false;
		}

		maxRangeSqr = m_pAttackInf->maxRange * m_pAttackInf->maxRange;
	}

	// test dist against this creatures max range
	m_lastDistSqr = EntityUtil::DistSqrDirCalcEntity(attacker, defender, maxRangeSqr, NULL);
	if (m_lastDistSqr < maxRangeSqr)
	{
		// Within range of at least one of the creatures range attacks.
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns true if the entity is within range of the target for his chosen attack (m_pAttackCurrent)
//
bool StateAttack::AtTargetForCurrentAttack()
{
	if (!m_pAttackCurrent)
	{
		// must have an attack chosen
		return false;
	}

	const SimEntity *attacker = static_cast<const SimEntity *>(GetEntity());
	const SimEntity *defender = static_cast<const SimEntity *>(GetTarget());
	dbAssert(attacker && defender);

	m_lastDistSqr = EntityUtil::DistSqrDirCalcEntity(attacker, defender, 0.0f, NULL);

	return (m_lastDistSqr <= __max(m_pAttackCurrent->m_rangesqr, 0.1f * 0.1f));
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: gets the target Entity
//	Result	: returns a Entity* or NULL if no target
//
Entity *StateAttack::GetTarget()
{
	if (!m_Target.empty())
	{
		if (QIExt<HealthExt>(m_Target.front()))
		{
			return m_Target.front();
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void StateAttack::HandleAttacking()
{
	// Do the updating logic for the attack in here.

	// Put any per simframe animation updates in here.

	// Have we finished AS_Attacking yet?
	if (ModObj::i()->GetWorld()->GetGameTicks() >= m_FinishTime)
	{
		// this should be valid at all times - set up in StartAttacking->ChooseAttack
		dbAssert(m_pAttackCurrent);

		// Subtract health from the dude.
		if (GetTarget())
		{
			// there must be a target to do damage to

			// normal attack
			m_pAttack->DoDamageTo(*m_pAttackCurrent, GetTarget());
		}

		StartCoolPeriod();
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool StateAttack::HandleCoolPeriod()
{
	if (ModObj::i()->GetWorld()->GetGameTicks() >= m_FinishTime)
	{
		Entity *pEntity = static_cast<Entity *>(GetEntity());
		if (pEntity->GetAnimator())
			pEntity->GetAnimator()->SetStyle('MOVE');

		return true;
	}
	else
	{
		return false;
	}
}

bool StateAttack::IsAttackingTarget() const
{
	if (m_CurState == AS_Attacking || m_CurState == AS_CoolPeriod ||
			m_CurState == AS_Reaquire)
		return true;
	return false;
}
