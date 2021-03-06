#pragma once

// Include  Files
#include "State.h"
#include "StateMove.h"

// Forward Declarations
class EntityDynamics;
class ResourceExt;

// check StateGather.puml for a flow diagram
class StateGather : public State
{
  // types
public:
  enum
  {
    StateID = SID_Gather,
  };

  enum StateGatherExitState
  {
    GES_Invalid,
    GES_NearbyResourcesDepleted,
    GES_RequestedToStop,
    GES_CouldNotReachResource,
    GES_CouldNotReachDeposit,
  };

  // construction
public:
  StateGather(EntityDynamics *e_dynamics);

  // should be called before Enter
  void Init(StateMove *);

  // make an entity gather from the specified resource
  void Enter(const Entity *pResourceEntity);

public:
  virtual bool Update();

  virtual void RequestExit();

  virtual void ReissueOrder() const;

  virtual void ForceExit();

  // retrieve the ID of this state
  virtual State::StateIDType GetStateID() const;

  // Save Load
  virtual void SaveState(BiFF &) const;
  virtual void LoadState(IFF &);

private:
  enum StateGatherInternalState
  {
    SG_Invalid,
    SG_MoveToResource,
    SG_WaitToGatherResource,
    SG_GatherResources,
    SG_PickupResource,
    SG_MoveToDeposit,
    SG_DropOffResource,
  };

  StateMove *m_pStateMove;
  unsigned long m_InternalState;
  float m_TickToCheckNextInternalState;
  StateGatherExitState m_ExitState;

  const Entity *m_pResourceEntity;
  const Entity *m_pDepositTarget;
  ResourceExt *m_pResourceExt;
  EntityGroup m_NearbyResources;
  float m_TimeToMineCoal;

  // TODO: these constants should come from tuning or something
  const float c_ResourceIncrements;

  // these methods return Success if the state should exit
  bool StateGather::ToWaitToGatherResourceState();
  bool StateGather::ToGatherResourceState();
  bool StateGather::ToPickupResourceState();
  bool StateGather::ToMoveToDepositState();
  bool StateGather::ToDropOffResourceState();

  // these methods return Success if the state should exit
  bool StateGather::HandleMoveToResource();
  bool StateGather::HandleWaitToGatherResource();
  bool StateGather::HandleGatherResource();
  bool StateGather::HandlePickupResource();
  bool StateGather::HandleMoveToDeposit();
  bool StateGather::HandleDropOffResource();

  bool StateGather::HandleResourceDepleted();

  // returns true if the exit was successful
  bool StateGather::TriggerExit(StateGatherExitState exitState);

  bool StateGather::IsDepositing();
  long StateGather::GetTicks();

  // sets the resource to gather and moves to it
  bool StateGather::MoveToLeastBusyResource();

  // find the resource closest to the current targetted resource, or nothing
  const Entity *StateGather::FindLeastBusyResourceNearby(const Entity *pResourceEntity);

  // set the time that a future operation should occur, in seconds
  void StateGather::SetTimer(float seconds);
  // check if the set timer has elapsed
  bool StateGather::HasTimerElapsed();

  void StateGather::SetIsHoldingPickaxe(bool bShouldHold);
  void StateGather::SetIsHoldingCoalpails(bool bShouldHold);
};
