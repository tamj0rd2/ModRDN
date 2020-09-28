/////////////////////////////////////////////////////////////////////
// File    : StateIdle.h
// Desc    :
// Created : Friday, January 25, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

// Include  Files
#include "State.h"

// Forward Declarations
class EntityDynamics;

/////////////////////////////////////////////////////////////////////
// StateIdle
// This state is responsible for the entities idle reactions.

class StateIdle : public State
{
	// types
public:
	enum
	{
		StateID = SID_Idle,
	};

	// construction
public:
	StateIdle(EntityDynamics *e_dynamics);

	// interface
public:
	void Enter();

	// returns the number of ticks spent in idle since Enter
	long GetIdleTicks() const;

	// inherited -- State
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

	// implementation
private:
	// fields
private:
	long m_tickBegin;
};
