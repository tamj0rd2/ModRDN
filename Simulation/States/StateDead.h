/////////////////////////////////////////////////////////////////////
// File    : StateDead.h
// Desc    :
// Created : Wednesday, March 07, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// Include  Files
#include "State.h"

// Forward Declarations
class EntityDynamics;

/////////////////////////////////////////////////////////////////////
// StateDead

// * this state is responsible for the entities Death.

class StateDead : public State
{
	// types
public:
	enum
	{
		StateID = SID_Dead,
	};

	// fields
private:
	long m_DeathCount;
	bool m_bUseWaterDeathAnim;
	long m_fadeDelayCount;

	// Functions.
public:
	StateDead(EntityDynamics *e_dynamics)
			: State(e_dynamics),
				m_bUseWaterDeathAnim(false),
				m_fadeDelayCount(0L) { ; }

	void Enter();

	void UseWaterDeathAnim(bool bUseWaterDeathAnim);
	void SetFadeDelay(long ticks);

	// Inherited -- State
public:
	virtual bool Update();
	virtual void RequestExit();
	virtual void ReissueOrder() const { ; }

	virtual void ForceExit();

	// does this state allow this command to be processed
	virtual bool AcceptCommand(int);

	// retrieve the ID of this state
	virtual State::StateIDType GetStateID() const;

	// Save Load
	virtual void SaveState(BiFF &) const;
	virtual void LoadState(IFF &);
};
