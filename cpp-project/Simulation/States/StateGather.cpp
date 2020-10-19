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
                                                       m_pResourceEntity(NULL),
                                                       m_pDepositTarget(NULL),
                                                       m_TickToCheckNextInternalState(0),
                                                       m_pResourceExt(NULL),
                                                       c_ResourceIncrements(20),
                                                       m_NearbyResources(EntityGroup()),
                                                       m_TimeToMineCoal(3)
{
}

State::StateIDType StateGather::GetStateID() const
{
  return (State::StateIDType)StateID;
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  if (!GetEntity()->GetAnimator())
    dbFatalf("Entities that want to gather resources should have animators attached");

  if (!pResourceEntity)
    dbFatalf("No resource given");

  SetExitStatus(false);

  ModObj::i()->GetWorld()->FindAll(
      m_NearbyResources,
      FindClosestEntityOfType(Coal_EC),
      pResourceEntity->GetPosition(),
      10);

  m_pResourceEntity = pResourceEntity;
  MoveToLeastBusyResource();
}

void StateGather::ReissueOrder() const
{
  dbTracef("StateGather::ReissueOrder");
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
  if (IsExiting())
    return IsExiting();

  if (m_pResourceExt->IsDepleted() && !IsDepositing())
    return MoveToLeastBusyResource();

  if (m_InternalState == SG_MoveToResource)
    return HandleMoveToResource();

  if (m_InternalState == SG_WaitToGatherResource)
    return HandleWaitToGatherResource();

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

bool StateGather::MoveToLeastBusyResource()
{
  m_InternalState = SG_MoveToResource;

  // find a resource to move to
  const Entity *pLeastBusyResource = FindLeastBusyResourceNearby(m_pResourceEntity);
  if (!pLeastBusyResource)
    return TriggerExit(GES_NearbyResourcesDepleted);

  // cleanup previous target
  if (m_pResourceExt)
    m_pResourceExt->GathererRmv(GetEntity());

  // setup new target
  m_pResourceEntity = pLeastBusyResource;
  m_pResourceExt = const_cast<ResourceExt *>(QIExt<ResourceExt>(pLeastBusyResource->GetController()));
  m_pResourceExt->GathererAdd(GetEntity());

  // move
  m_pStateMove->Enter(m_pResourceEntity, 0);
  return Success;
}

bool StateGather::HandleMoveToResource()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget)
      return ToWaitToGatherResourceState();

    return TriggerExit(GES_CouldNotReachResource);
  }

  return NothingHappened;
}

bool StateGather::ToWaitToGatherResourceState()
{
  m_InternalState = SG_WaitToGatherResource;
  if (m_pResourceExt->CanGatherResourcesOnSiteNow(GetEntity()))
    return ToGatherResourceState();

  SetTimer(m_TimeToMineCoal);
  return Success;
}

bool StateGather::HandleWaitToGatherResource()
{
  if (m_pResourceExt->CanGatherResourcesOnSiteNow(GetEntity()))
    return ToGatherResourceState();

  if (HasTimerElapsed())
    return MoveToLeastBusyResource();

  return NothingHappened;
}

bool StateGather::ToGatherResourceState()
{
  m_InternalState = SG_GatherResources;
  m_pResourceExt->GatherersOnSiteAdd(GetEntity());
  GetEntity()->GetAnimator()->SetTargetLook(m_pResourceEntity);
  GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");
  SetTimer(m_TimeToMineCoal);
  return Success;
}

bool StateGather::HandleGatherResource()
{
  if (!HasTimerElapsed())
    return NothingHappened;

  return ToPickupResourceState();
}

bool StateGather::ToPickupResourceState()
{
  m_InternalState = SG_PickupResource;
  m_pResourceExt->GatherersOnSiteRmv(GetEntity());
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

  return MoveToLeastBusyResource();
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

const Entity *StateGather::FindLeastBusyResourceNearby(const Entity *pResourceEntity)
{
  // TODO: remove resources from the nearbyResources vector once they're depleted
  if (!pResourceEntity)
    dbFatalf("No resource entity given");

  ResourceExt *pResourceExt = const_cast<ResourceExt *>(QIExt<ResourceExt>(pResourceEntity->GetController()));
  if (!pResourceExt)
    dbFatalf("The given entity has no resource extension");

  // target the specified resource if there are no other gatherers
  if (!pResourceExt->IsDepleted() && pResourceExt->HasNoOtherGatherers(GetEntity()))
    return pResourceEntity;

  Entity *resourceWithLeastGatherers = NULL;
  size_t minGatherersFound = !pResourceExt->IsDepleted() ? pResourceExt->GetGathererCount() : 999;

  EntityGroup::iterator iter;
  for (iter = m_NearbyResources.begin(); iter != m_NearbyResources.end(); iter++)
  {
    ResourceExt *pResourceExt = QIExt<ResourceExt>((*iter)->GetController());
    if (pResourceExt->IsDepleted())
      continue;

    // target a resource where there are no other registered gatherers
    if (pResourceExt->HasNoOtherGatherers(GetEntity()))
      return (*iter);

    // keep track of the resource that has the least gatherers
    size_t gathererCount = pResourceExt->GetGathererCount();
    if (gathererCount < minGatherersFound)
    {
      resourceWithLeastGatherers = (*iter);
      minGatherersFound = gathererCount;
    }
  }

  if (resourceWithLeastGatherers)
    return resourceWithLeastGatherers;

  return pResourceExt->IsDepleted() ? NULL : pResourceEntity;
}

long StateGather::GetTicks()
{
  return ModObj::i()->GetWorld()->GetGameTicks();
}

bool StateGather::TriggerExit(StateGather::StateGatherExitState exitState)
{
  dbTracef("Exiting StateGather for reason %d", exitState);
  m_pResourceExt->GathererRmv(GetEntity());
  m_ExitState = exitState;
  SetExitStatus(ShouldExit);
  return ShouldExit;
}

void StateGather::RequestExit()
{
  dbTracef("StateGather::RequestExit");
  TriggerExit(GES_RequestedToStop);
}

void StateGather::ForceExit()
{
  dbTracef("StateGather::ForceExit");
  TriggerExit(GES_RequestedToStop);
}
