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

  virtual void SoftExit();
  virtual void RequestExit();
  virtual void ReissueOrder() const;

  virtual void ForceExit();

  // retrieve the ID of this state
  virtual State::StateIDType GetStateID() const;

  // Save Load
  virtual void SaveState(BiFF &) const;
  virtual void LoadState(IFF &);

  StateGatherExitState GetExitState();

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

  void StateGather::ToMoveToResourceState();
  void StateGather::ToGatherResourceState();
  void StateGather::ToPickupResourceState();
  void StateGather::ToMoveToDepositState();
  void StateGather::ToDropOffResourceState();

  void StateGather::HandleMoveToResource();
  void StateGather::HandleGatherResource();
  void StateGather::HandlePickupResource();
  void StateGather::HandleMoveToDeposit();
  void StateGather::HandleDropOffResource();

  bool StateGather::TriggerExit(StateGatherExitState exitState);

  bool StateGather::IsDepositing();
  long StateGather::GetTicks();
  // set the time that a future operation should occur, in seconds
  void StateGather::SetTimer(float seconds);
  // check if the set timer has elapsed
  bool StateGather::HasTimerElapsed();
};
