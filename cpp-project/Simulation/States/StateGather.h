#pragma once

// Include  Files
#include "State.h"
#include "StateMove.h"

// Forward Declarations
class EntityDynamics;
class ResourceExt;

/////////////////////////////////////////////////////////////////////
// StateGather
// This state is responsible for gathering resources

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
    GES_ResourceDepleted,
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
    SG_GatherResources,
    SG_PickupResource,
    SG_MoveToDeposit,
    SG_DropOffResource,
  };

  StateMove *m_pStateMove;
  unsigned long m_InternalState;
  float m_TickToCheckNextInternalState;
  StateGatherExitState m_ExitState;

  const Entity *m_pResourceTarget;
  const Entity *m_pDepositTarget;
  ResourceExt *m_pResourceExt;

  // TODO: these constants should come from tuning or something
  const float c_ResourceIncrements;

  // these methods return Success if the state should exit
  bool StateGather::ToMoveToResourceState();
  bool StateGather::ToGatherResourceState();
  bool StateGather::ToPickupResourceState();
  bool StateGather::ToMoveToDepositState();
  bool StateGather::ToDropOffResourceState();

  // these methods return Success if the state should exit
  bool StateGather::HandleMoveToResource();
  bool StateGather::HandleGatherResource();
  bool StateGather::HandlePickupResource();
  bool StateGather::HandleMoveToDeposit();
  bool StateGather::HandleDropOffResource();

  bool StateGather::HandleResourceDepleted();

  // returns true if the exit was successful
  bool StateGather::TriggerExit(StateGatherExitState exitState);

  bool StateGather::IsDepositing();
  long StateGather::GetTicks();

  // sets the resource to mine to be the given entity
  bool StateGather::SetTargetResource(const Entity *pResourceEntity);
  // find the resource closest to the current targetted resource
  Entity *StateGather::FindResourceNearTargetResource();

  // set the time that a future operation should occur, in seconds
  void StateGather::SetTimer(float seconds);
  // check if the set timer has elapsed
  bool StateGather::HasTimerElapsed();
};
