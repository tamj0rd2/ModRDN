/////////////////////////////////////////////////////////////////////
// File    : StatePause.h
// Desc    : 
// Created : Thursday, June 4, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

// Include  Files
#include "State.h"

///////////////////////////////////////////////////////////////////// 
// StateIdle 
// This state is responsible for the entities idle reactions.

class StatePause : public State
{
// types
public:
	enum
	{
		StateID = SID_Pause,
	};

	enum 
	{
		IgnoreCommands = 1,
	};

// construction
public:
	StatePause( EntityDynamics* e_dynamics );

// interface
public:
	void Enter( bool bIgnoreCmds = true );

	bool AcceptCommand( int cmdtype );
	
	void IncrementPauseCount();

// inherited -- State
public:
	virtual bool				Update();
	
	virtual void				RequestExit();
	virtual void				ReissueOrder() const;

	virtual void				ForceExit();

	// retrieve the ID of this state
	virtual State::StateIDType	GetStateID( ) const;

	// Save Load
	virtual void				SaveState( BiFF& ) const;
	virtual void				LoadState( IFF& );

private:

	bool m_bIgnoreCmds;
	int m_pauseCount;

	bool m_bImmobilized;

};
