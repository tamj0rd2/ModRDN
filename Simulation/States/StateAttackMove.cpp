/////////////////////////////////////////////////////////////////////
// File    : StateAttackMove.cpp
// Desc    : 
// Created : Thursday, March 01, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//
#include "pch.h"
#include "StateAttackMove.h"

#include "../../ModObj.h"
#include "../RDNPlayer.h"
#include "../RDNWorld.h"
#include "../RDNQuery.h"
#include "../CommandTypes.h"

#include "../Controllers/ModController.h"

#include "../Extensions/HealthExt.h"
#include "../Extensions/AttackExt.h"

#include "StateMove.h"
#include "StateAttack.h"

#include <SimEngine/Entity.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/Player.h>
#include <SimEngine/SimHelperFuncs.h>

#include <Util/Biff.h>
#include <Util/Iff.h>
#include <Util/IffMath.h>

///////////////////////////////////////////////////////////////////// 
// 
const long k_UnreachableMemoryLength = 24;

//////////////////////////////////////////////////////////////////////////////////////
// AttackMove  Public Functions

enum
{
	AM_Invalid,
	AM_StateMove,
	AM_StateToAttack,
	AM_StateAttack,
	AM_StateMoveToAttack,
	AM_Exiting,
};

const long SEARCH_INTERVAL = 2;
const long BLOCK_INTERVAL = 20*8;

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
StateAttackMove::StateAttackMove( EntityDynamics *e_dynamics )
:	State( e_dynamics ),
	m_pStateMove( NULL ),
	m_pStateAttack( NULL ),
	m_pCurState( NULL ),
	m_SearchTimer( 0 ),
	m_State( AM_Invalid ),
	m_lastsearchtime( 0 )
{
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::Init( StateMove* pMove, StateAttack* pAttack )
{
	m_pStateMove	= pMove;
	m_pStateAttack	= pAttack;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::Enter( const Entity* entity, float AP )
{
	dbAssert( entity );
	const Vec3f &epos = entity->GetPosition();
	Enter( epos, AP );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::Enter( const Vec3f& dest, float AP )
{
	// Called upon entering the move state.
	m_Destination = dest;

	m_AP = AP;

	//
	m_lastsearchtime = ModObj::i()->GetWorld()->GetGameTicks();

	AttackExt* pAttackExt = QIExt<AttackExt>( GetEntity() );	
	if ( !pAttackExt )
	{
		// should this ever happen?
		SetExitStatus( true );
		m_State = AM_Exiting;
		return;
	}
	
	m_SearchRad = pAttackExt->GetAttackSearchRadius();

	if ( FindClosestEnemy( m_SearchRad ) )
	{
		m_pStateAttack->Enter( m_Targets.front() );
		m_pCurState = m_pStateAttack;
		m_State = AM_StateAttack;
	}
	else
	{
		ToStateMove();
	}

	SetExitStatus( false );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool StateAttackMove::Update()
{
	bool retval = false;

	if ( IsExiting() )
	{
		switch ( m_State )
		{
			case AM_StateAttack:
				if ( m_pCurState->Update() )
				{
					retval = true;
				}
			break;
			case AM_StateMove:
			case AM_StateToAttack:
			case AM_StateMoveToAttack:
			{
				if ( m_pCurState->Update() )
				{
					retval = true;
				}
			}
			break;
			case AM_Exiting:
				retval = true;
			break;
		}
	}
	else
	{
		switch ( m_State )
		{
			//
			case AM_StateAttack:
			{
 				if ( !RDNQuery::WasAttackedBy( GetEntity(), m_Targets.front(), 8 * 5 ) )
				{	
					// not fighting -> search for a retaliation target
					long gameTicks = ModObj::i()->GetWorld()->GetGameTicks();

					if ( gameTicks > m_lastsearchtime + 4)
					{	
						// time to search again

						// don't need to search for a while
						m_lastsearchtime = gameTicks;
						
						if ( FindRetaliationEnemy() )
						{	
							// found a better enemy to attack
							m_State = AM_StateToAttack;
							m_pCurState->RequestExit();
							return false;
						}
					}
				}


				if ( m_pCurState->Update() )
				{
					// State attack has exited, why did it exit?
					switch( m_pStateAttack->GetExitState() )
					{
						//
						case StateAttack::AES_CantPathToTargetEntity:
						{
							// need to remember this target as blocked so that we don't immediately try to target it again
							RememberBlockedTarget( m_Targets.front() );

							// is there another enemy nearby?
							if ( FindClosestEnemy( m_SearchRad ) )
							{	
								// FindClosestEnemy has found a new m_Target, bring it on!
								m_pStateAttack->Enter( m_Targets.front() );
								m_pCurState = m_pStateAttack;
							}
							else if ( FindAlternateEnemy() )
							{
								m_pStateAttack->Enter( m_Targets.front() );
								m_pCurState = m_pStateAttack;
							}
							else
							{								
								// no one to attack, keep truckin' to destination
								ToStateMove();	
							}

							break;
						}
						//
						case StateAttack::AES_CantPathToTargetBuilding:
						{	
							// is the blocker an enemy? if so, attack it

							SimEntity *pBlockingBuilding = GetDynamics()->GetBlockingBuilding();
							if ( pBlockingBuilding && RDNQuery::CanAttack( GetEntity(), pBlockingBuilding ) )
							{	
								// blocked by an enemy wall or building -> attack it!
								m_pStateAttack->Enter( pBlockingBuilding );
								m_pCurState = m_pStateAttack;
							}
							else
							{
								// can't attack the blocking building

								// need to remember this target as blocked so that we don't immediately try to target it again?
								RememberBlockedTarget( m_Targets.front() );

								// keep moving to destination
								ToStateMove();
							}

							break;
						}
						//
						case StateAttack::AES_CantPathToTargetTerrain:
						case StateAttack::AES_Blocked:
						case StateAttack::AES_ExitRequested:
						{
							// keep moving
							ToStateMove();
							break;
						}
						case StateAttack::AES_TargetDead:
						{
							// just killed a guy, search for another target
							if ( FindClosestEnemy( m_SearchRad ) )
							{
								// found another target
								m_pStateAttack->Enter( m_Targets.front() );
								m_pCurState		= m_pStateAttack;
								m_State	= AM_StateAttack;
							}
							else
							{
								// noone in the immediate vicinity to attack -> keep moving
								ToStateMove();
							}

							// reset our search timer
							m_SearchTimer = ModObj::i()->GetWorld()->GetGameTicks() + SEARCH_INTERVAL;

							break;
						}
						//
						default:
							// should never hit this
							dbBreak();
							break;
					}			
				}
				break;
			}
			//
			case AM_StateToAttack:
			{
				if ( m_pCurState->Update() != 0 )
				{
					// Tell the attack state to start with our selected targets
					if ( m_Targets.front() )
					{
						m_pStateAttack->Enter( m_Targets.front() );
						m_pCurState		= m_pStateAttack;
						m_State	= AM_StateAttack;
					}
					else
					{	
						// no target -> keep moving to goal
						ToStateMove();
					}
				}
			}
			break;
			//
			case AM_StateMove:
				if ( m_pCurState->Update() )
				{
					switch( m_pStateMove->GetExitState() )
					{
						//
						case StateMove::MES_StoppedBeforeTarget:
							ToStateMove();
							break;

						//
						case StateMove::MES_CantPathToTargetBuilding:					
						{
							// blocked by a building, is it an enemy's?
							SimEntity *pBlockingBuilding = GetDynamics()->GetBlockingBuilding();
							if ( pBlockingBuilding && RDNQuery::CanAttack( GetEntity(), pBlockingBuilding ) )
							{	
								// blocked by an enemy wall or building -> attack it!
								m_pStateAttack->Enter( pBlockingBuilding );
								m_pCurState = m_pStateAttack;
								m_State = AM_StateAttack;
							}
							else
							{
								// blocked by friendly, nothing we can do
								retval = true;
							}
							break;
						}
						//
						case StateMove::MES_CantPathToTargetEntity:
						{
							// is there another enemy nearby?
							if ( FindClosestEnemy( m_SearchRad ) )
							{	
								// FindClosestEnemy has found a new m_Target, bring it on!
								m_pStateAttack->Enter( m_Targets.front() );
								m_pCurState = m_pStateAttack;
								m_State = AM_StateAttack;
							}
							else if ( FindAlternateEnemy() )
							{
								m_pStateAttack->Enter( m_Targets.front() );
								m_pCurState = m_pStateAttack;
								m_State = AM_StateAttack;
							}
							else
							{				
								// no alternate targets to destroy that might remove the blockage, must exit
								//	todo : another layer of retry logic.  fuxk no.
								retval = true;
							}

							break;
						}
						//
						case StateMove::MES_ReachedTarget:
						case StateMove::MES_NoTarget:
						case StateMove::MES_CantPathToTargetTerrain:
							retval = true;
							break;
						//
						default:
							// this should never happen
							dbBreak();
							retval = true;
							break;

					}				
				}
				else
				{
					if ( ModObj::i()->GetWorld()->GetGameTicks() > m_SearchTimer )
					{
						if ( FindClosestEnemy( m_SearchRad ) )
						{
							m_pStateMove->RequestExit();
							m_State = AM_StateMoveToAttack;
						}
						// reset our search timer
						m_SearchTimer = ModObj::i()->GetWorld()->GetGameTicks() + SEARCH_INTERVAL;
					}
				}
			break;
			case AM_StateMoveToAttack:
			{
				if ( m_pCurState->Update() )
				{
					if ( FindClosestEnemy( m_SearchRad ) )
					{
						m_pStateAttack->Enter( m_Targets.front() );
						m_pCurState = m_pStateAttack;
						m_State = AM_StateAttack;
					}
					else
					{
						// State attack has exited, lets go onto the move state
						ToStateMove();
					}
				}
			}
			break;
		}
	}

	if ( retval )
		m_State = AM_Invalid;
	
	return retval;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::RequestExit()
{
	switch ( m_State )
	{
		case AM_StateAttack:
			m_pStateAttack->RequestExit();
		break;
		case AM_StateMove:
		case AM_StateMoveToAttack:
			m_pStateMove->RequestExit();
		break;
	}
	
	SetExitStatus( true );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateAttackMove::ReissueOrder() const
{
	const unsigned long playerID = GetEntity()->GetOwner() ? GetEntity()->GetOwner()->GetID() : 0;
	const unsigned long entityID = GetEntity()->GetID();
	
	ModObj::i()->GetWorld()->DoCommandEntityPoint
		( 
		CMD_AttackMove,
		0, 
		CMDF_Queue, 
		playerID, 
		&entityID,
		1,
		&m_Destination,
		1
		);
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::ForceExit()
{
	switch ( m_State )
	{
		case AM_StateAttack:
			m_pStateAttack->ForceExit();
		break;
		case AM_StateMove:
		case AM_StateMoveToAttack:
			m_pStateMove->ForceExit();
		break;
	}

	m_State = AM_Invalid;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
State::StateIDType StateAttackMove::GetStateID( ) const
{
	return (State::StateIDType)StateID;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::SaveState( BiFF& biff ) const
{
	IFF& iff = *biff.GetIFF();

	unsigned long ver = 3;
	IFFWrite( iff, ver );

	IFFWrite( iff, m_AP );
	
	IFFWrite( iff, m_Destination );

	return;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateAttackMove::LoadState( IFF& iff )
{
	unsigned long ver;
	IFFRead( iff, ver );

	float ap, searchrad;
	IFFRead( iff, ap );

	if ( ver == 1 )
	{
		IFFRead( iff, searchrad );
	}
	
	Vec3f temp;
	IFFRead( iff, temp );

	// Re-Initialize the state
	Enter( temp, ap );
}

State* StateAttackMove::GetSubState( unsigned char id )
{
	if ( IsExiting() )
	{
		return NULL;
	}

	if ( id == StateID )
	{
		return this;
	}
	else if ( id == State::SID_Attack && m_State == AM_StateAttack )
	{
		return m_pStateAttack->GetSubState( id );
	}
	else if ( id == State::SID_Move && m_State == AM_StateMove)
	{
		return m_pStateMove->GetSubState( id );
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
// AttackMove : Private Functions

/////////////////////////////////////////////////////////////////////
//	Desc.	: call to transition to AM_StateMove
//
void StateAttackMove::ToStateMove( )
{
	m_pStateMove->Enter( m_Destination, m_AP );
	m_pCurState = m_pStateMove;

	m_State = AM_StateMove;
	m_SearchTimer = ModObj::i()->GetWorld()->GetGameTicks() + SEARCH_INTERVAL;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: call to mark an entity as blocked, so we don't choose it as a target again.
//
void StateAttackMove::RememberBlockedTarget( Entity *pBlockedTarget )
{
	if ( !pBlockedTarget )
	{
		return;
	}

	AttackExt* pAttackExt = QIExt< AttackExt >( GetEntity() );
	if ( pAttackExt )
	{
		pAttackExt->GetUnreachableMemory().AddMemory( pBlockedTarget, ModObj::i()->GetWorld()->GetGameTicks() );
	}
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : uses the prioritizer to find a close enemy
// Result    : 
// Param.    : 
// Author    : 
//
bool StateAttackMove::FindClosestEnemy( float SearchRad )
{
	Entity* pClosestCreature = NULL;	

	//
	EntityGroup unreachables;
	AttackExt* pAttackExt = QIExt< AttackExt >( GetEntity() );
	if ( pAttackExt )
	{
		const long currentTime = ModObj::i()->GetWorld()->GetGameTicks();
		pAttackExt->GetUnreachableMemory().GetMemories( unreachables, currentTime - k_UnreachableMemoryLength );
	}

	// check if this unit is a flyer
	ThreatPrioritizerAll prioritizer( GetEntity()->GetOwner());
	pClosestCreature = const_cast<Entity*>(RDNQuery::FindClosestEnemy( GetEntity(), GetEntity()->GetPosition(), SearchRad, prioritizer, &unreachables ));	
			
	m_Targets.clear();
	
	if ( pClosestCreature )
	{
		// is this unit closer than
		m_Targets.push_back( pClosestCreature );
		return true;
	}

	return false;	
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Tries to find a higher priority Entity to attack.  It will bias towards Entitys that are damaging self
//	Result	: returns true if found a higher priority Entity
//	Param.	: 
//	Author	: dswinerd
//
bool StateAttackMove::FindRetaliationEnemy()
{
	Entity *pSelf = GetEntity();

	AttackExt *pAttackExt = QIExt< AttackExt >( pSelf );
	if ( !pAttackExt )
	{
		return false;
	}

	// find an enemy to retaliate against, no matter how far away (as long as they are visible in the FoW)
	float searchRadius = FLT_MAX;	// was pAttackExt->GetAttackSearchRadius();
	const Entity *pAttacker = RDNQuery::FindRetaliationEnemy( pSelf, searchRadius, 8 );

	if (pAttacker)
	{
		m_Targets.clear();
		m_Targets.push_back( const_cast<Entity*>( pAttacker ) );

		return true;
	}
	else
	{
		return false;
	}
}


///////////////////////////////////////////////////////////////////// 
// Desc.     : if we were blocked we will find an alternate enemy
// Result    : return true if we found an enemy
// Author    : dswinerd
//
bool StateAttackMove::FindAlternateEnemy( )
{	

	//
	EntityGroup unreachables;
	AttackExt* pAttackExt = QIExt< AttackExt >( GetEntity() );
	if ( pAttackExt )
	{
		const long currentTime = ModObj::i()->GetWorld()->GetGameTicks();
		pAttackExt->GetUnreachableMemory().GetMemories( unreachables, currentTime - k_UnreachableMemoryLength );
	}

	FindClosestDetectionFilter filter
	(
		GetEntity(),
		GetEntity()->GetOwner(),
		&unreachables,
		NULL
	);

	// find closest enemy
	EntityGroup tempGroup;
	ModObj::i()->GetWorld()->FindClosest( tempGroup, filter, 1, GetEntity()->GetPosition(), pAttackExt->GetAttackSearchRadius(), GetEntity() );

	Entity* pClosestCreature = tempGroup.front();

	if ( pClosestCreature )
	{
		// is this unit closer than
		m_Targets.push_back( pClosestCreature );
		return true;
	}

	return false;
}
