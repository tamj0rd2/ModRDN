/////////////////////////////////////////////////////////////////////
// File    : StateAttack.h
// Desc    : 
// Created : 
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// Include Files
#include "State.h"
#include <SimEngine/EntityGroup.h>
#include <SimEngine/Target.h>

#include <Math/Vec2.h>

// Forward Declarations
class Entity;
class EntityDynamics;
class ControllerBlueprint;
class AttackExt;
class AttackExtInfo;
class HenchmanAttackExtInfo;
struct AttackPackage;
struct AttackInfoPackage;
class StateMove;


///////////////////////////////////////////////////////////////////// 
// StateAttack 

class StateAttack : public State
{
// types
public:
	enum
	{
		StateID = SID_Attack,
	};

	enum AttackExitState
	{
		AES_TargetDead,
		AES_CantPathToTargetEntity,
		AES_CantPathToTargetTerrain,
		AES_CantPathToTargetBuilding,
		AES_Blocked,
		AES_ExitRequested,
		AES_Invalid,
	};

// construction
public:
	StateAttack( EntityDynamics *e_dynamics );

// interface
public:
	void						Init( StateMove*, AttackExt*, const AttackInfoPackage* );

	void						Enter( Entity* pTarget );
	void						EnterNoMove( Entity* pTarget );

	const Entity*				GetTargetEntity() const;					// returns the Entity was are targeting.  Can be NULL.

	AttackExitState				GetExitState( );							// returns the reason for exiting

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

	bool						IsAttackingTarget( ) const;

// fields
private:

	enum AttackStates
	{
		AS_Null,
		AS_MovingTo_Target,
		AS_RandomWait,
		AS_Attacking,
		AS_CoolPeriod,
		AS_TargetDead,
		AS_Cant_Reach_Target,
		AS_Chasing_Target,
		AS_TransitionToChasingTarget,
		AS_Reaquire,				// used for fliers
	};

	StateMove*					m_pStateMove;

	EntityGroup					m_Target;

	AttackStates				m_CurState;

	long						m_FinishTime;	// Timer variable in game ticks.

	unsigned long				m_AttackCount;

	AttackExt*					m_pAttack;
	const AttackInfoPackage*	m_pAttackInf;
	const AttackPackage*		m_pAttackCurrent;	// Current attack it has picked (melee/range)

	float						m_lastDistSqr;

	AttackExitState				m_exitState;

	// Should the attack state try and move closer to the target
	bool						m_bNoMove;

	Vec2f						m_chargeStartHeading;		// will hold the heading to the target at the time a charge attack started

// implementation
private:

	bool	UpdateInternal();

	void	MoveToTarget();
	void	StartAttacking();
	void	StartCoolPeriod();
	bool	StartReaquire();

	bool	AtTargetForAnyAttack();
	bool	AtTargetForCurrentAttack();
	
	void	DoFlyerDamage();

	bool	ChooseAttack();

	Entity* GetTarget();

	void	ProcessRequestExit();

	void	HandleAttacking( );
	bool	HandleCoolPeriod( );


// copy -- do not define
private:
	StateAttack( const StateAttack& );
	StateAttack& operator= ( const StateAttack& );
};
