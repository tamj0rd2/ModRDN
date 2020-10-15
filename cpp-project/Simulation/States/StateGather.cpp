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

#include <SimEngine/SimEntity.h>
#include <SimEngine/World.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>

enum
{
  SG_Invalid,
  SG_MoveToCoal,
  SG_GatherResources,
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

bool StateGather::Update()
{
  if (m_State == SG_MoveToCoal && m_pCurState->Update())
  {
    StateMove::MoveExitState moveExitState = m_pStateMove->GetExitState();
    if (moveExitState == StateMove::MES_ReachedTarget || moveExitState == StateMove::MES_CantPathToTargetTerrain)
    {
      // maybe this terrain issue also explains why henchmen walk behind the coal instead of to the front
      dbTracef("Hench reached the resource, or could not reach it because of terrain. TODO fixme");
    }

    ToGatherResourceState();
  }

  dbTracef("StateGather state is %d", m_State);

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
  // walk over to coal
  Vec3f destination = m_pResourceTarget->GetPosition();
  m_pCurState = m_pStateMove;
  m_pStateMove->Enter(destination, 0.0f);
  m_State = SG_MoveToCoal;

  // if (GetEntity()->GetAnimator())
  // {
  //   GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");
  // }

  // resource->GathererAdd(pEntity);

  // once the hench has reached the coal, change state to MiningCoal
  // face it
  // switch to the mining animation

  // here is how you can face things. found in lab controller:OnUnitSpawn
  // pSimController->GetEntityDynamics()->SetEntityFacing(facing);
}

void StateGather::ToGatherResourceState()
{
  m_pCurState = NULL;
  m_State = SG_GatherResources;

  // turn to face the coal
  SimController *pSimController = static_cast<SimController *>(GetEntity()->GetController());
  if (pSimController && pSimController->GetEntityDynamics())
  {
    Vec2f directionOfResource;
    directionOfResource = Vec2f(m_pResourceTarget->GetPosition().x, m_pResourceTarget->GetPosition().z) - Vec2f(GetEntity()->GetPosition().x, GetEntity()->GetPosition().z);

    // watch out for Normalizing 0 length vector
    if (directionOfResource.LengthSqr() > 0.1)
    {
      directionOfResource.NormalizeSelf();
    }
    else
    {
      directionOfResource.Set(0.0, 1.0f);
    }

    // use the solution from mod actions for a smoother transition
    pSimController->GetEntityDynamics()->SetEntityFacing(directionOfResource);
  }

  if (GetEntity()->GetAnimator())
  {
    GetEntity()->GetAnimator()->SetMotionTreeNode("PaSwing");
  }

  ResourceExt *resource = const_cast<ResourceExt *>(QIExt<ResourceExt>(m_pResourceTarget));
  resource->GathererAdd(GetEntity());
}
