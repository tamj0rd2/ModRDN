/////////////////////////////////////////////////////////////////////
// File    : StateGather.cpp
// Desc    :
// Created : Friday, January 25, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateGather.h"
#include "StateMove.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../RDNQuery.h"
#include "../RDNPlayer.h"
#include "../PlayerFoW.h"

#include "../Controllers/ModController.h"

#include "../Extensions/SightExt.h"

#include <SimEngine/SimEntity.h>
#include <SimEngine/World.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>

enum
{
  SG_Invalid,
  SG_MoveToCoal,
  SG_Exiting,
};

StateGather::StateGather(EntityDynamics *e_dynamics) : State(e_dynamics), m_pStateMove(NULL),
                                                       m_pCurState(NULL),
                                                       m_State(SG_Invalid),
                                                       m_pResourceTarget(NULL)
{
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  dbAssert(pResourceEntity);
  m_pResourceTarget = pResourceEntity;
  //	HACK : signal the entity to play an idle animation
  // if (pEntity->GetAnimator())
  // 	pEntity->GetAnimator()->SetStyle((GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'IDLE');
  Entity *pEntity = GetEntity();
  dbTracef("StateGather::Enter entity %s", pEntity->GetControllerBP()->GetFileName());
  dbTracef("StateGather::Target entity %s", pResourceEntity->GetControllerBP()->GetFileName());
  SetExitStatus(false);

  ToMoveToCoalState();
}

bool StateGather::Update()
{
  // if we want to exit at any time
  return IsExiting();
}

void StateGather::SoftExit()
{
  SetExitStatus(true);
}

void StateGather::RequestExit()
{
  SetExitStatus(true);
}

void StateGather::ReissueOrder() const
{
  // nothing to do
}

void StateGather::ForceExit()
{
  // nothing
}

State::StateIDType StateGather::GetStateID() const
{
  return (State::StateIDType)StateID;
}

void StateGather::SaveState(BiFF &) const
{
}

// TODO: implement this
void StateGather::LoadState(IFF &)
{
  // Enter();
}

void StateGather::ToMoveToCoalState()
{
  Vec3f destination = m_pResourceTarget->GetPosition();
  m_pStateMove->Enter(destination, 0.0f);
  m_pCurState = m_pStateMove;
  m_State = SG_MoveToCoal;
}
