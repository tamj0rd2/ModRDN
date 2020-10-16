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
#include "../Extensions/ResourceExt.h"

#include "../Simulation/UnitConversion.h"

#include <SimEngine/SimEntity.h>
#include <SimEngine/World.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/BuildingDynamics.h>

StateGather::StateGather(EntityDynamics *e_dynamics) : State(e_dynamics), m_pStateMove(NULL),
                                                       m_InternalState(SG_Invalid),
                                                       m_pResourceTarget(NULL),
                                                       m_pDepositTarget(NULL),
                                                       m_TickToCheckNextInternalState(0),
                                                       m_resourceExt(NULL),
                                                       c_ResourceIncrements(20)
{
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  // TODO: make sure there is an animator for the entity
  dbAssert(pResourceEntity);
  m_pResourceTarget = pResourceEntity;

  m_resourceExt = const_cast<ResourceExt *>(QIExt<ResourceExt>(pResourceEntity->GetController()));
  dbAssert(m_resourceExt);

  if (!GetEntity()->GetAnimator())
  {
    dbFatalf("Entities that want to gather resources should have animators attached");
  }

  SetExitStatus(false);
  ToMoveToResourceState();
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
  // Enter();
}

bool StateGather::Update()
{
  if (m_resourceExt->GetResources() <= 0.0f && !IsDepositing())
  {
    return TriggerExit(GES_ResourceDepleted);
  }

  switch (m_InternalState)
  {
  case SG_MoveToResource:
  {
    HandleMoveToResource();
    break;
  }
  case SG_GatherResources:
  {
    HandleGatherResource();
    break;
  }
  case SG_PickupResource:
  {
    HandlePickupResource();
    break;
  }
  case SG_MoveToDeposit:
  {
    HandleMoveToDeposit();
    break;
  }
  case SG_DropOffResource:
  {
    HandleDropOffResource();
    break;
  }
  default:
  {
    dbFatalf("Unimplemented StateGather state %d", m_InternalState);
    break;
  }
  }

  // if we want to exit at any time
  return IsExiting();
}

void StateGather::ToMoveToResourceState()
{
  m_InternalState = SG_MoveToResource;
  m_pStateMove->Enter(m_pResourceTarget, 0);
}

void StateGather::HandleMoveToResource()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget)
    {
      ToGatherResourceState();
      return;
    }
    else
    {
      dbFatalf("Could not get to the resource. Status %d", moveExitState);
    }
  }
}

void StateGather::ToGatherResourceState()
{
  m_InternalState = SG_GatherResources;
  GetEntity()->GetAnimator()->SetTargetLook(m_pResourceTarget);
  GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");
  SetTimer(3);
}

void StateGather::HandleGatherResource()
{
  if (HasTimerElapsed())
  {
    ToPickupResourceState();
  }
}

void StateGather::ToPickupResourceState()
{
  m_InternalState = SG_PickupResource;
  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPickup");
  SetTimer(1.04f);
}

void StateGather::HandlePickupResource()
{
  if (!HasTimerElapsed())
  {
    return;
  }

  m_resourceExt->DecResources(c_ResourceIncrements);
  ToMoveToDepositState();
}

void StateGather::ToMoveToDepositState()
{
  dbTracef("Should be finished picking up resource");
  m_InternalState = SG_MoveToDeposit;

  FindClosestResourceDepsoit filter;
  Entity *depositEntity = ModObj::i()->GetWorld()->FindClosestEntity(filter, GetEntity()->GetPosition(), 1000, NULL);

  if (!depositEntity)
  {
    // TODO: it should actually exit the gather state if there is no place to deposit coal
    dbFatalf("Could not find a deposit :(");
  }

  // maybe CpOverlay motion
  m_pStateMove->Enter(depositEntity, 0);
  GetEntity()->GetAnimator()->SetMotionTreeNode("CpOverlay");
}

void StateGather::HandleMoveToDeposit()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget || moveExitState == StateMove::MES_CantPathToTargetTerrain)
    {
      // maybe this terrain issue also explains why henchmen walk behind the coal instead of to the front
      dbTracef("Hench reached the deposit or the terrain blocked it. state: %d", moveExitState);
      ToDropOffResourceState();
    }
  }
}

void StateGather::ToDropOffResourceState()
{
  dbTracef("Should be dropping off the resource");
  m_InternalState = SG_DropOffResource;

  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPutdown");
  SetTimer(1.03f);
}

void StateGather::HandleDropOffResource()
{
  if (HasTimerElapsed())
  {
    ToMoveToResourceState();
  }
}

bool StateGather::TriggerExit(StateGather::StateGatherExitState exitState)
{
  m_ExitState = exitState;
  SetExitStatus(true);
  return true;
}

StateGather::StateGatherExitState StateGather::GetExitState()
{
  return m_ExitState;
}

bool StateGather::IsDepositing()
{
  return m_InternalState == SG_MoveToDeposit || m_InternalState == SG_DropOffResource;
}

bool StateGather::HasTimerElapsed()
{
  return GetTicks() > m_TickToCheckNextInternalState;
}

void StateGather::SetTimer(float seconds)
{
  m_TickToCheckNextInternalState = GetTicks() + (k_SimStepsPerSecond * seconds);
}

long StateGather::GetTicks()
{
  return ModObj::i()->GetWorld()->GetGameTicks();
}
