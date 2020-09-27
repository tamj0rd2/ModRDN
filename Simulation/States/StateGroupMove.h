/////////////////////////////////////////////////////////////////////
// File    : StateGroupMove.h
// Desc    : 
// Created : Wednesday, May 1, 2002
// Author  : dswinerd
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once


#include "State.h"

#include <SimEngine/Target.h>

/////////////////////////////////////////////////////////////////////
//	Forward Declarations:
//
class StateMove;

/////////////////////////////////////////////////////////////////////
//	Class.	: StateGroupMove
//
class StateGroupMove : public State
{
// types
public:
	enum
	{
		StateID = SID_GroupMove,
	};

// functions
public:

	StateGroupMove( EntityDynamics *e_dynamics );
	
	// intialize the state
	void	Init( StateMove* pMove );	

	void	Enter( Entity* pEntity,			 float AP, unsigned long flags = 0 );
	void	Enter( const Vec3f& destination, float AP, unsigned long flags = 0 );

	void	EnterGroup( const Vec3f& offset, float maxSpeed );

	bool	CanGroup() const;

	void	AddWayPoint( const Vec3f& point );

// inherited -- State
public:
	virtual bool				Update();			// Return values:
													//	0 - still moving.
													//	1 - stopped because reached its goal.
													//	2 - can't find path to goal.
	virtual void				RequestExit();

	virtual void				ReissueOrder() const;

	virtual void				ForceExit();

	// retrieve the ID of this state
	virtual State::StateIDType	GetStateID( ) const;

	virtual State*				GetSubState( unsigned char stateid );

	// Save Load
	virtual void				SaveState( BiFF& ) const;
	virtual void				LoadState( IFF& );

// types
private:

	enum SubState
	{
		SS_Invalid,
		SS_WaitingForGroup,
		SS_Group,
		SS_Solo,
	};

// functions
private:

	void						DoInternalEnter();

	void						ExitGroup();

	void						ToGroup( const Vec3f& offset, float maxSpeed );
	void						ToSolo();

	bool						HandleWaitingForGroup();
	bool						HandleSolo();
	bool						HandleGroup();

// data
private:

	StateMove*		m_pStateMove;

	Target			m_target;
	float			m_AP;

	unsigned long	m_flags;

	long			m_enterTick;

	SubState		m_subState;
};
