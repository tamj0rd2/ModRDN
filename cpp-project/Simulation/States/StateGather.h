#pragma once

// Include  Files
#include "State.h"
#include "StateMove.h"

// Forward Declarations
class EntityDynamics;

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

private:
  enum StateGatherInternalState
  {
    SG_Invalid,
    SG_MoveToResource,
    SG_GatherResources,
    SG_PickupResource,
    SG_MoveToDeposit,
    SG_DropOffResource,
    SG_Exiting,
  };

  StateMove *m_pStateMove;
  unsigned long m_InternalState;
  float m_TickToCheckNextInternalState;

  const Entity *m_pResourceTarget;
  const Entity *m_pDepositTarget;

  void StateGather::ToMoveToCoalState();
  void StateGather::ToGatherResourceState();
  void StateGather::ToPickupResourceState();
  void StateGather::ToMoveToDepositState();
  void StateGather::ToDropOffResourceState();

  long StateGather::GetTicks();
};
