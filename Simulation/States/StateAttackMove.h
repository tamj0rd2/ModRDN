/////////////////////////////////////////////////////////////////////
// File    : StateAttackMove.h
// Desc    : 
// Created : Thursday, March 01, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// Include Files
#include "State.h"

#include <Math/Vec3.h>

#include <SimEngine/EntityGroup.h>

// Forward Declarations
class Entity;
class EntityDynamics;
class StateMove;
class StateAttack;


///////////////////////////////////////////////////////////////////// 
// StateMove 

class StateAttackMove : public State
{
// types
public:
	enum
	{
		StateID = SID_AttackMove,
	};

private:

	StateMove*			m_pStateMove;
	StateAttack*		m_pStateAttack;
	State*				m_pCurState;

	Vec3f				m_Destination;
	float				m_AP;
	float				m_SearchRad;

	unsigned long		m_State;

	long				m_SearchTimer;

	EntityGroup			m_Targets;

	long				m_lastsearchtime;

// Functions.
public:

	StateAttackMove( EntityDynamics *e_dynamics );

	void Init( StateMove*, StateAttack* );
	
	void Enter( const Entity* , float AP );
	void Enter( const Vec3f& dest, float AP );

// Inherited -- State
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

	virtual State*				GetSubState( unsigned char stateid );

private:
	
	void ToStateMove();

	void RememberBlockedTarget( Entity *pBlockedTarget );
	bool FindClosestEnemy( float SearchRad );
	bool FindRetaliationEnemy( );
	bool FindAlternateEnemy( );
};
