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

StateGather::StateGather(EntityDynamics *e_dynamics) : State(e_dynamics), m_pStateMove(NULL)
{
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  //	HACK : signal the entity to play an idle animation
  // if (pEntity->GetAnimator())
  // 	pEntity->GetAnimator()->SetStyle((GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'IDLE');
  Entity *pEntity = GetEntity();
  dbTracef("StateGather::Enter entity %s", pEntity->GetControllerBP()->GetFileName());
  dbTracef("StateGather::Target entity %s", pResourceEntity->GetControllerBP()->GetFileName());

  SetExitStatus(false);

  Vec3f destination = pResourceEntity->GetPosition();

  m_pStateMove->Enter(destination, 0.0f);
  // m_pCurState = m_pStateMove;

  // m_State = AM_StateMove;
  // m_SearchTimer = ModObj::i()->GetWorld()->GetGameTicks() + SEARCH_INTERVAL;

  return;
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

void StateGather::LoadState(IFF &)
{
  // TODO: implement this
  // Enter();
}
