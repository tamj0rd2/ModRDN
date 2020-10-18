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

const bool Success = false;
const bool NothingHappened = false;
const bool ShouldExit = true;

StateGather::StateGather(EntityDynamics *e_dynamics) : State(e_dynamics), m_pStateMove(NULL),
                                                       m_InternalState(SG_Invalid),
                                                       m_pResourceTarget(NULL),
                                                       m_pDepositTarget(NULL),
                                                       m_TickToCheckNextInternalState(0),
                                                       m_pResourceExt(NULL),
                                                       c_ResourceIncrements(20)
{
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  if (!GetEntity()->GetAnimator())
  {
    dbFatalf("Entities that want to gather resources should have animators attached");
  }

  SetExitStatus(false);
  SetTargetResource(pResourceEntity);
}

void StateGather::RequestExit()
{
  dbTracef("StateGather::RequestExit");
  TriggerExit(GES_RequestedToStop);
}

void StateGather::ReissueOrder() const
{
  dbTracef("StateGather::ReissueOrder");
}

void StateGather::ForceExit()
{
  dbTracef("StateGather::ForceExit");
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
  if (m_pResourceExt->GetResources() <= 0.0f && !IsDepositing())
    return HandleResourceDepleted();

  if (m_InternalState == SG_MoveToResource)
    return HandleMoveToResource();

  if (m_InternalState == SG_GatherResources)
    return HandleGatherResource();

  if (m_InternalState == SG_PickupResource)
    return HandlePickupResource();

  if (m_InternalState == SG_MoveToDeposit)
    return HandleMoveToDeposit();

  if (m_InternalState == SG_DropOffResource)
    return HandleDropOffResource();

  dbFatalf("Unimplemented StateGather state %d", m_InternalState);
}

bool StateGather::ToMoveToResourceState()
{
  m_InternalState = SG_MoveToResource;
  m_pStateMove->Enter(m_pResourceTarget, 0);
  return Success;
}

bool StateGather::HandleMoveToResource()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget)
      return ToGatherResourceState();
    else
      return TriggerExit(GES_CouldNotReachResource);
  }

  return NothingHappened;
}

bool StateGather::ToGatherResourceState()
{
  m_InternalState = SG_GatherResources;
  GetEntity()->GetAnimator()->SetTargetLook(m_pResourceTarget);
  GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");
  SetTimer(3);
  return Success;
}

bool StateGather::HandleGatherResource()
{
  if (HasTimerElapsed())
    return ToPickupResourceState();

  return NothingHappened;
}

bool StateGather::ToPickupResourceState()
{
  m_InternalState = SG_PickupResource;
  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPickup");
  SetTimer(1.04f);
  return Success;
}

bool StateGather::HandlePickupResource()
{
  if (!HasTimerElapsed())
    return NothingHappened;

  m_pResourceExt->DecResources(c_ResourceIncrements);
  return ToMoveToDepositState();
}

bool StateGather::ToMoveToDepositState()
{
  m_InternalState = SG_MoveToDeposit;

  FindClosestResourceDepsoit filter;
  Entity *depositEntity = ModObj::i()->GetWorld()->FindClosestEntity(filter, GetEntity()->GetPosition(), 1000, NULL);

  if (!depositEntity)
    return TriggerExit(GES_CouldNotReachDeposit);

  // maybe CpOverlay motion
  m_pStateMove->Enter(depositEntity, 0);
  GetEntity()->GetAnimator()->SetMotionTreeNode("CpOverlay");
  return Success;
}

bool StateGather::HandleMoveToDeposit()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget)
      return ToDropOffResourceState();
    else
      dbFatalf("Hench could not reach the deposit. state: %d", moveExitState);
  }

  return NothingHappened;
}

bool StateGather::ToDropOffResourceState()
{
  m_InternalState = SG_DropOffResource;

  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPutdown");
  SetTimer(1.03f);
  return Success;
}

bool StateGather::HandleDropOffResource()
{
  if (!HasTimerElapsed())
    return NothingHappened;

  RDNPlayer *player = static_cast<RDNPlayer *>(GetEntity()->GetOwner());
  player->IncResourceCash(c_ResourceIncrements, RDNPlayer::RES_Resourcing);
  return ToMoveToResourceState();
}

bool StateGather::HandleResourceDepleted()
{
  Entity *nearestResource = FindResourceNearTargetResource();

  if (nearestResource)
    return SetTargetResource(nearestResource);

  return TriggerExit(GES_ResourceDepleted);
}

bool StateGather::TriggerExit(StateGather::StateGatherExitState exitState)
{
  m_pResourceExt->GathererRmv(GetEntity());
  m_pResourceExt->GatherersOnSiteRmv(GetEntity());

  m_ExitState = exitState;
  SetExitStatus(ShouldExit);
  return ShouldExit;
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

bool StateGather::SetTargetResource(const Entity *pResourceEntity)
{
  m_pResourceTarget = pResourceEntity;
  if (!m_pResourceTarget)
    dbFatalf("No resource entity given");

  m_pResourceExt = const_cast<ResourceExt *>(QIExt<ResourceExt>(m_pResourceTarget->GetController()));
  if (!m_pResourceExt)
    dbFatalf("The given resource entity has no resource ext");

  m_pResourceExt->GathererAdd(GetEntity());
  return ToMoveToResourceState();
}

Entity *StateGather::FindResourceNearTargetResource()
{
  FindClosestEntityOfType filter(Coal_EC);

  return ModObj::i()->GetWorld()->FindClosestEntity(
      filter,
      m_pResourceTarget->GetPosition(),
      10,
      m_pResourceTarget);
}

long StateGather::GetTicks()
{
  return ModObj::i()->GetWorld()->GetGameTicks();
}
