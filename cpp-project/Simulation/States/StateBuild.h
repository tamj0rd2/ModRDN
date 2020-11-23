#pragma once

// Include  Files
#include "State.h"
#include "StateMove.h"

// Forward Declarations
class EntityDynamics;

class StateBuild : public State
{
  // types
public:
  enum
  {
    StateID = SID_Build,
  };

  enum StateBuildExitState
  {
    BES_Invalid,
    BES_RequestedToStop,
    BES_CouldNotReachBuilding,
    BES_BuildComplete,
  };

  // construction
public:
  StateBuild(EntityDynamics *e_dynamics);

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
  enum StateBuildInternalState
  {
    SB_Invalid,
    SB_MoveToBuilding,
    SB_BuildBuilding,
  };

  StateMove *m_pStateMove;
  unsigned long m_InternalState;
  float m_TickToCheckNextInternalState;
  StateBuildExitState m_ExitState;

  const Entity *m_pBuildingEntity;

  // these methods return Success if the state should exit
  bool StateBuild::ToWaitToGatherResourceState();
  bool StateBuild::ToBuildBuildingState();
  bool StateBuild::ToPickupResourceState();
  bool StateBuild::ToMoveToDepositState();
  bool StateBuild::ToDropOffResourceState();

  // these methods return Success if the state should exit
  bool StateBuild::HandleMoveToBuilding();
  bool StateBuild::HandleWaitToGatherResource();
  bool StateBuild::HandleBuildBuilding();
  bool StateBuild::HandlePickupResource();
  bool StateBuild::HandleMoveToDeposit();
  bool StateBuild::HandleDropOffResource();

  bool StateBuild::HandleResourceDepleted();

  // returns true if the exit was successful
  bool StateBuild::TriggerExit(StateBuildExitState exitState);

  bool StateBuild::IsDepositing();
  long StateBuild::GetTicks();

  // sets the resource to gather and moves to it
  bool StateBuild::MoveToBuilding();

  // find the resource closest to the current targetted resource, or nothing
  const Entity *StateBuild::FindLeastBusyResourceNearby(const Entity *pResourceEntity);

  // set the time that a future operation should occur, in seconds
  void StateBuild::SetTimerSeconds(float seconds);
  void StateBuild::SetTimerTicks(long ticks);
  // check if the set timer has elapsed
  bool StateBuild::HasTimerElapsed();

  void StateBuild::SetIsHoldingHammer(bool bShouldHold);
};
