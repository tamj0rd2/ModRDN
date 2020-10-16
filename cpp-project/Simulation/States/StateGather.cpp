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
#include "../Controllers/HenchmenController.h"

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
                                                       m_TickToCheckNextInternalState(0)
{
}

void StateGather::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateGather::Enter(const Entity *pResourceEntity)
{
  // TODO: make sure there is an animator for the entity
  // make sure the entity is henchmen controller type

  dbAssert(pResourceEntity);
  m_pResourceTarget = pResourceEntity;
  //	HACK : signal the entity to play an idle animation
  // if (pEntity->GetAnimator())
  // 	pEntity->GetAnimator()->SetStyle((GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'IDLE');
  Entity *pEntity = GetEntity();
  dbTracef("StateGather::Enter entity %s", pEntity->GetControllerBP()->GetFileName());
  dbTracef("StateGather::Target entity %s", pResourceEntity->GetControllerBP()->GetFileName());
  SetExitStatus(false);

  ResourceExt *resource = const_cast<ResourceExt *>(QIExt<ResourceExt>(pResourceEntity->GetController()));
  dbAssert(resource);

  // some of this should probably be made generic based on the controller type.
  // i.e, remove references to "coal"
  ToMoveToCoalState();

  // ToGatherResourceState();

  // ToPickupResourceState();

  // IncAndDecrementCoalAter1Second();

  // ToMoveToDepositState();

  // ToDepositResourceState();

  // ToMoveToCoalState();
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

bool StateGather::Update()
{
  switch (m_InternalState)
  {
  case SG_MoveToResource:
  {
    if (m_pStateMove->Update())
    {
      StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
      if (moveExitState == StateMove::MES_ReachedTarget)
      {
        ToGatherResourceState();
      }
      else
      {
        dbFatalf("Could not get to the resource. Status %d", moveExitState);
      }
    }
    break;
  }
  case SG_GatherResources:
  {
    if (GetTicks() > m_TickToCheckNextInternalState)
    {
      ToPickupResourceState();
    }
    break;
  }
  case SG_PickupResource:
  {
    if (GetTicks() > m_TickToCheckNextInternalState)
    {
      ToMoveToDepositState();
    }
    break;
  }
  case SG_MoveToDeposit:
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
    break;
  }
  case SG_DropOffResource:
  {
    if (GetTicks() > m_TickToCheckNextInternalState)
    {
      ToMoveToCoalState();
    }
    break;
  }
  default:
  {
    dbWarningf(1, "Unhandled StateGather state %d", m_InternalState);
    break;
  }
  }

  // if we want to exit at any time
  return IsExiting();
}

void StateGather::ToMoveToCoalState()
{
  m_InternalState = SG_MoveToResource;
  m_pStateMove->Enter(m_pResourceTarget, 0);
}

void StateGather::ToGatherResourceState()
{
  m_InternalState = SG_GatherResources;
  GetEntity()->GetAnimator()->SetTargetLook(m_pResourceTarget);
  GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");

  m_TickToCheckNextInternalState = GetTicks() + (k_SimStepsPerSecond * 3);

  // TODO: tie in with the actual resource ext
  // ResourceExt *resource = const_cast<ResourceExt *>(QIExt<ResourceExt>(m_pResourceTarget));
  // resource->GathererAdd(GetEntity());
}

void StateGather::ToPickupResourceState()
{
  m_InternalState = SG_PickupResource;
  dbTracef("Should start picking up the resource now");

  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPickup");

  m_TickToCheckNextInternalState = GetTicks() + (k_SimStepsPerSecond * 1.03f);
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

void StateGather::ToDropOffResourceState()
{
  dbTracef("Should be dropping off the resource");
  m_InternalState = SG_DropOffResource;

  GetEntity()->GetAnimator()->SetMotionTreeNode("CpPutdown");

  m_TickToCheckNextInternalState = GetTicks() + (k_SimStepsPerSecond * 1.03f);
}

long StateGather::GetTicks()
{
  return ModObj::i()->GetWorld()->GetGameTicks();
}
