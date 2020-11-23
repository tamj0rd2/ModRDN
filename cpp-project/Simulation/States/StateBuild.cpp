/////////////////////////////////////////////////////////////////////
// File    : StateBuild.cpp
// Desc    :
// Created : Friday, January 25, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateBuild.h"
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
#include "../Simulation/ExtInfo/CostExtInfo.h"

#include <SimEngine/SimEntity.h>
#include <SimEngine/World.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/BuildingDynamics.h>

const bool c_success = false;
const bool c_nothingHappened = false;
const bool c_shouldExit = true;

const char *c_lowBuild = "LowBuild";
const char *c_highBuild = "HighBuild";

StateBuild::StateBuild(EntityDynamics *e_dynamics) : State(e_dynamics), m_pStateMove(NULL),
                                                     m_InternalState(SB_Invalid),
                                                     m_pBuildingEntity(NULL),
                                                     m_TickToCheckNextInternalState(0)
{
}

State::StateIDType StateBuild::GetStateID() const
{
  return (State::StateIDType)StateID;
}

void StateBuild::Init(StateMove *pStateMove)
{
  m_pStateMove = pStateMove;
}

void StateBuild::Enter(const Entity *pBuildingEntity)
{
  if (!GetEntity()->GetAnimator())
    dbFatalf("Entities that want to build buildings should have animators attached");

  if (!pBuildingEntity)
    dbFatalf("No resource given");

  SetExitStatus(false);
  m_pBuildingEntity = pBuildingEntity;
  MoveToBuilding();
}

void StateBuild::ReissueOrder() const
{
  dbTracef("StateBuild::ReissueOrder");
}

void StateBuild::SaveState(BiFF &) const
{
}

void StateBuild::LoadState(IFF &)
{
  // Enter();
}

bool StateBuild::Update()
{
  if (IsExiting())
    return IsExiting();

  // bool isTheBuildingComplete = false;

  if (IsExiting())
  {
    return IsExiting();
  }

  // if (isTheBuildingComplete) {
  //   return TriggerExit(BES_BuildComplete);
  // }

  if (m_InternalState == SB_MoveToBuilding)
  {
    return HandleMoveToBuilding();
  }

  if (m_InternalState == SB_BuildBuilding)
  {
    return HandleBuildBuilding();
  }

  // TODO
  // 1 - move to the building
  // 2 - start building it
  // 3 - exit once the building completion is at 100%

  dbFatalf("Unimplemented StateBuild state %d", m_InternalState);
  return c_shouldExit;
}

bool StateBuild::MoveToBuilding()
{
  m_InternalState = SB_MoveToBuilding;

  // move
  m_pStateMove->Enter(m_pBuildingEntity, 0);
  SetIsHoldingHammer(true);
  return c_success;
}

bool StateBuild::HandleMoveToBuilding()
{
  if (m_pStateMove->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget)
      return ToBuildBuildingState();

    return TriggerExit(BES_CouldNotReachBuilding);
  }

  return c_nothingHappened;
}

bool StateBuild::ToBuildBuildingState()
{
  m_InternalState = SB_BuildBuilding;
  SetIsHoldingHammer(true);
  GetEntity()->GetAnimator()->SetTargetLook(m_pBuildingEntity);
  GetEntity()->GetAnimator()->SetMotionTreeNode(c_lowBuild);

  const CostExtInfo *costExitInfo = QIExtInfo<CostExtInfo>(m_pBuildingEntity);
  if (!costExitInfo)
    dbFatalf("StateBuild::ToBuildBuildingState no cost ext info for entity %s", GetEntity()->GetControllerBP()->GetFileName());

  SetTimerTicks(costExitInfo->constructionTicks);
  return c_success;
}

bool StateBuild::HandleBuildBuilding()
{
  // TODO: don't hardcode this
  bool isBuildingComplete = false;

  if (HasTimerElapsed() && isBuildingComplete)
  {
    return TriggerExit(BES_BuildComplete);
  }

  return c_nothingHappened;
}

void StateBuild::SetIsHoldingHammer(bool bShouldHold)
{
  GetEntity()->GetAnimator()->SetMotionVariable("Hammer", bShouldHold ? 50.0f : 0);
}

long StateBuild::GetTicks()
{
  return ModObj::i()->GetWorld()->GetGameTicks();
}

void StateBuild::SetTimerSeconds(float seconds)
{
  SetTimerTicks(k_SimStepsPerSecond * seconds);
}

void StateBuild::SetTimerTicks(long ticks)
{
  m_TickToCheckNextInternalState = GetTicks() + ticks;
}

bool StateBuild::TriggerExit(StateBuild::StateBuildExitState exitState)
{
  dbTracef("Exiting StateBuild for reason %d", exitState);
  m_ExitState = exitState;
  SetExitStatus(c_shouldExit);

  SetIsHoldingHammer(false);
  return c_shouldExit;
}

void StateBuild::RequestExit()
{
  dbTracef("StateBuild::RequestExit");
  TriggerExit(BES_RequestedToStop);
}

bool StateBuild::HasTimerElapsed()
{
  return GetTicks() > m_TickToCheckNextInternalState;
}

void StateBuild::ForceExit()
{
  dbTracef("StateBuild::ForceExit");
  TriggerExit(BES_RequestedToStop);
}
