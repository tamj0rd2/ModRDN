/////////////////////////////////////////////////////////////////////
// File    : StateMove.cpp
// Desc    : 
// Created : Thursday, September 20, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateMove.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../CommandTypes.h"
#include "../RDNQuery.h"

#include "../Extensions/MovingExt.h"

#include "../Controllers/ModController.h"

#include <SimEngine/Player.h>
#include <SimEngine/Entity.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityUtil.h>

#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>

#include <Util/Iff.h>
#include <Util/Biff.h>

//////////////////////////////////////////////////////////////////////////////////////
// Move
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
StateMove::StateMove( EntityDynamics *e_dynamics ) :
	State		( e_dynamics ),
	m_subState	( MSS_MoveFailed ),
	m_waypointsVisited( 0 ),
	m_bCheckFOW( false )
{
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: Enter the move state with an Entity as the goal
//	Param.	: pDest - the destination entity
//			  AP - the distance from the entity we need to be
//
void StateMove::Enter( const Entity *pDest, float AP, bool bCheckFOW, unsigned long flags, RetryType rt, int retryLimit )
{
	// clear any pre-existing WayPoints
	m_WayPoints.clear();
	m_Offset.Set( 0.0f );

	m_bCheckFOW = bCheckFOW;

	if ( !pDest->GetEntityFlag( EF_CanCollide ) || !pDest->GetEntityFlag( EF_IsSpawned ) )
	{
		m_subState = MSS_MoveFailed;
		m_exitstate = MES_StoppedBeforeTarget;
		SetExitStatus( true );
		return;
	}

	m_Target.SetType(Target::eTargetEntity);
	m_Target.SetEntity( const_cast<Entity *>(pDest) );
	m_AP = AP;
	m_flags = flags;
	m_retryType = rt;
	m_retryLimit = retryLimit;

	m_WayPoints.push_back( pDest->GetPosition() );
	m_waypointsVisited = 0;

	DoEnter();
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: Enter the move state with an point as the goal
//	Param.	: dest - the destination position
//			  AP - the distance from the point we need to be
//
void StateMove::Enter( const Vec3f& dest, float AP, unsigned long flags, RetryType rt, int retryLimit )
{
	// clear any pre-existing WayPoints
	m_WayPoints.clear();
	m_Offset.Set( 0.0f );

	m_bCheckFOW = false;

	m_AP = AP;
	m_flags = flags;
	m_retryType = rt;
	m_retryLimit = retryLimit;

	m_WayPoints.push_back( dest );
	m_waypointsVisited = 0;

	DoInternalEnter( dest );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::Enter( const Vec3f& dest, const Vec3f& offset, float AP, unsigned long flags, RetryType rt, int retryLimit )
{
	// clear any pre-existing WayPoints
	m_WayPoints.clear();
	m_Offset = offset;

	m_bCheckFOW = false;

	m_AP = AP;
	m_flags = flags;
	m_retryType = rt;
	m_retryLimit = retryLimit;

	m_WayPoints.push_back( dest );
	m_waypointsVisited = 0;

	DoInternalEnter( dest );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: performs the code common to both Enter routines
//	Author	: dswinerd
//
void StateMove::DoEnter()
{
	m_retryCount = 0;
	SetExitStatus( false );

	Entity* pEntity = GetEntity();
	if ( pEntity->GetAnimator() )
		pEntity->GetAnimator()->SetStyle( (GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'MOVE' );

	DoRequestMove();
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateMove::DoRequestMove()
{
	m_subState = MSS_Normal;
	m_exitstate = MES_Invalid;
	GetDynamics()->RequestMove(m_Target, m_AP);

	// let the dynamics know if this is the only pending waypoint
	GetDynamics()->HintLastWaypoint( m_WayPoints.size() <= 1 );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::DoInternalEnter( const Vec3f& dest )
{
	m_Target.SetType(Target::eTargetPos);

	// clamp the offseted position to the world
	Vec3f clampedPos = dest + m_Offset;
	ModObj::i()->GetWorld()->ClampPointToWorld( clampedPos );

	m_Target.SetPos( clampedPos );

	DoEnter();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::AddWayPoint( const Vec3f& point )
{
	//
	m_WayPoints.push_back( point );

	GetDynamics()->HintLastWaypoint( false );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
size_t StateMove::GetNumWayPoints() const
{
	return m_WayPoints.size();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
const Vec3f* StateMove::GetWayPoints() const
{
	return &m_WayPoints[0];
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
const int StateMove::GetNumWayPointsVisited() const
{
	return m_waypointsVisited;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
const Vec3f& StateMove::GetOffset() const
{
	return m_Offset;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns the reason for the move state exiting
//	Result	: returns MoveExitState
//
StateMove::MoveExitState StateMove::GetExitState( )
{
	dbAssert( m_exitstate != MES_Invalid );

	return m_exitstate;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: returns true if the state is done.  Then check GetExitState() for why.
//	Param.	: 
//
bool StateMove::Update()
{
	// The high level movement logic.
	//	Supports movement waypoints.

	// change the animation if we changed terrain types (ie. from land to water)
	if (GetDynamics()->GetVisualMovementType() != GetDynamics()->GetPrevVisualMovementType())
	{
		Entity* pEntity = GetEntity();
		if ( pEntity->GetAnimator() )
			pEntity->GetAnimator()->SetStyle( (GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'MOVE' );
	}

	// update our dest location
	if ( m_Target.GetType() == Target::eTargetEntity && m_Target.GetEntity() )
	{
		m_WayPoints[ 0 ] = m_Target.GetEntity()->GetPosition();

		// if it's an ordered move
		if ( m_bCheckFOW )
		{
			if ( !RDNQuery::CanBeSeen( m_Target.GetEntity(), GetEntity()->GetOwner() ) )
			{
				// clear the target
				m_Target.SetEntity( NULL );
				m_Target.SetType( Target::eTargetPos );
				m_Target.SetPos( m_WayPoints[ 0 ] );

				if ( m_subState == MSS_Normal )
				{
					// we were moving normal to an entity, stop that and transition to moving to is last position
					GetDynamics()->RequestStop();
					m_subState = MSS_NormalEntityLost;
				}
			}
		}
	}

	bool bReturnVal = true;

	switch (m_subState)
	{
		case MSS_Normal:
			bReturnVal = HandleNormalState();
			break;
		case MSS_WaitingToRetry:
			bReturnVal = HandleWaitingToRetry();
			break;
		case MSS_NormalEntityLost:
			bReturnVal = HandleNormalEntityLost();
			break;
		case MSS_MoveFailed:
			bReturnVal = true;
			break;
		default:
			// should never hit this
			dbBreak();
			bReturnVal = true;
			break;
	}

	if ( bReturnVal && !IsExiting() && m_WayPoints.size() > 1 )
	{
		IncrementWaypoint();

		return false;
	}

	return bReturnVal;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateMove::IncrementWaypoint( )
{
	Vec3f nextDest = *(m_WayPoints.begin() + 1);
	m_WayPoints.erase( m_WayPoints.begin() );

	DoInternalEnter( nextDest );

	m_waypointsVisited++;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: start waiting for a retry
//	Param.	: retryTime - number of ticks to wait before retrying
//	Author	: dswinerd
//
void StateMove::ToRetry( long retryTime )
{
	m_retryTime = ModObj::i()->GetWorld()->GetGameTicks() + retryTime;
	m_subState = MSS_WaitingToRetry;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: returns true if the state is done
//	Param.	: 
//	Author	: dswinerd
//
bool StateMove::HandleNormalState()
{
	EntityDynamics::eEDState ret = GetDynamics()->QueryStatus();

	switch(ret)
	{
		case EntityDynamics::eStateMove:
		{
			// We are still moving.
			return false;
		}
		case EntityDynamics::eStateStop:
		{
			// The dynamics has reached it's goal, or it's goal is no longer valid...
			if ( m_Target.Valid() )
			{
				m_exitstate = MES_ReachedTarget;
			}
			else
			{
				m_exitstate = MES_NoTarget;
			}

			// Return that we have finished.
			return true;
		}
		case EntityDynamics::eStateCantGetThere:
		{	// entity was blocked by something -> what to do? retry or give up?
			
			// delegate the decision to CantGetThereProcess
			return CantGetThereProcess();
		}
		default:
		{
			dbBreak();	// This shouldn't ever get called.
			return true;
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: returns true if the state is done
//	Param.	: 
//	Author	: dswinerd
//
bool StateMove::HandleWaitingToRetry()
{
	if ( IsExiting() )
	{	// exit was requested
		m_exitstate = MES_StoppedBeforeTarget;
		return true;
	}

	if ( !m_Target.Valid() )
	{	// our target became invalid

		m_exitstate = MES_NoTarget;

		// the state is done
		return true;
	}

	if ( ModObj::i()->GetWorld()->GetGameTicks() >= m_retryTime )
	{	// done waiting to retry

		// increment the retry count
		m_retryCount++;

		DoRequestMove();
	}

	// the state isn't done
	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
bool StateMove::HandleNormalEntityLost()
{
	EntityDynamics::eEDState ret = GetDynamics()->QueryStatus();

	if ( ret != EntityDynamics::eStateMove )
	{
		// the previous move to entity is done, now move to its last known position
		DoRequestMove();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if a EntityDynamics::CantGetThereType matches the
//				StateMove::RetryType we care about
//	Result	: returns true if there is a match. ie. that we are blocked by something we care about
//	Param.	: EntityDynamics::CantGetThereType cantType - the type of blockage
//			  StateMove::RetryType retryType - the type of blockages we want to retry on.  bitfield
//			  pEntity - this entity
//			  target - the target of out movement
//			  spaceThreshold - if amount of space of freedom for RT_BlockedOnEntityAllowSpace
//	Author	: dswinerd
//
static bool RetryVsBlockageMatch( EntityDynamics::CantGetThereType cantType, StateMove::RetryType retryType, 
								  const Entity* pEntity, const Target &target, float spaceThreshold )
{
	bool bMatch = false;

	switch (cantType)
	{
		case EntityDynamics::CGTT_Entity:
			// dynamics was blocked by entities

			if (retryType & StateMove::RT_BlockedOnEntityAllowSpace)
			{
				float distSqr = EntityUtil::DistSqrDirCalcTarget( pEntity, &target, spaceThreshold, NULL );

				if ( distSqr > spaceThreshold*spaceThreshold )
				{
					bMatch = true;
				}
			}
			else if (retryType & StateMove::RT_BlockedOnEntity)
			{
				bMatch = true;
			}

			break;
		case EntityDynamics::CGTT_Building:
			// dynamics was blocked by buildings

			if (retryType & StateMove::RT_BlockedOnBuilding)
			{
				bMatch = true;
			}

			break;
		case EntityDynamics::CGTT_Terrain:
			// dynamics was blocked by terrain

			if (retryType & StateMove::RT_BlockedOnTerrain)
			{
				bMatch = true;
			}

			break;
		default:

			// shouldn't hit this -> no match
			bMatch = false;
				
			break;
	}

	return bMatch;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines what to do if we 'cant get there'.  Could be retry or just exit
//	Result	: returns true if the StateMove should exit
//	Author	: dswinerd
//
bool StateMove::CantGetThereProcess()
{
	// get the block reason from the dynamics
	EntityDynamics::CantGetThereType cantType = GetDynamics()->GetCantGetThereType();

	// do we have retries left and does our retry type match the blockage type?
	bool bRetry = (m_retryCount < m_retryLimit) && RetryVsBlockageMatch( cantType, m_retryType, GetEntity(), m_Target, 10.0f );		

	if (bRetry)
	{	// wait to retry

		const long RETRY_WAIT_TICKS = 16;
		ToRetry( RETRY_WAIT_TICKS );

		// since we are going to retry, don't exit the state
		return false;
	}
	else
	{	// dont want to retry -> just give up straight away for now

		// convert the blockage type to an m_exitstate
		switch ( cantType )
		{
			case EntityDynamics::CGTT_Entity:
				m_exitstate = MES_CantPathToTargetEntity;
				break;
			case EntityDynamics::CGTT_Terrain:
				m_exitstate = MES_CantPathToTargetTerrain;
				break;
			case EntityDynamics::CGTT_Building:
				m_exitstate = MES_CantPathToTargetBuilding;
				break;
			default:
				// should never hit this
				dbBreak();
				break;
		}
		
		// giving up the move -> exit the state
		return true;
	}
}


///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::RequestExit()
{
	MovingExt* pMoveExt = QIExt< MovingExt >( GetEntity() );
	if ( pMoveExt )
	{
		pMoveExt->RemoveSpeedOverride();
	}

	GetDynamics()->RequestStop();

	SetExitStatus( true );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateMove::ReissueOrder() const
{
	const unsigned long playerID = GetEntity()->GetOwner() ? GetEntity()->GetOwner()->GetID() : 0;
	const unsigned long entityID = GetEntity()->GetID();

	if ( m_Target.GetType() == Target::eTargetEntity )
	{	// moving to an entity
		if ( m_Target.GetEntity() )
		{
			
			const unsigned long targetID = m_Target.GetEntity()->GetID();

			ModObj::i()->GetWorld()->DoCommandEntityEntity( CMD_Move,		
															0,				
															CMDF_Queue,
															playerID,
															&entityID,
															1,
															&targetID,
															1
															);
		}
	}
	else
	{	// moving to a point

		ModObj::i()->GetWorld()->DoCommandEntityPoint( CMD_Move,		
														0,				
														CMDF_Queue,
														playerID,
														&entityID,
														1,
														&m_WayPoints[0],
														m_WayPoints.size()
														);

	}
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::ForceExit()
{
	MovingExt* pMoveExt = QIExt< MovingExt >( GetEntity() );
	if ( pMoveExt )
	{
		pMoveExt->RemoveSpeedOverride();
	}
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
State::StateIDType StateMove::GetStateID( ) const
{
	return (State::StateIDType)StateID;
}

unsigned long CURRENT_VERSION = 3;
///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::SaveState( BiFF& biff ) const
{
	IFF& iff = *biff.GetIFF();

	IFFWrite( iff, CURRENT_VERSION );

	IFFWrite( iff, m_AP );

	unsigned long retryTemp = static_cast<RetryType>(m_retryType);
	IFFWrite( iff, retryTemp );
	IFFWrite( iff, m_retryLimit );

	m_Target.SaveEmbedded( iff );

	// new version 3
	IFFWrite( iff, m_flags );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateMove::LoadState( IFF& iff )
{
	RetryType retryType = RT_None;
	int retryLimit = 0;
	float AP = 0;
	unsigned long flags = 0;
	Target target;

	unsigned long ver;
	IFFRead( iff, ver );

	if (ver == 1)
	{
		IFFRead( iff, AP );
		target.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );
	}
	else
	{
		// after version 1

		IFFRead( iff, AP );

		unsigned long retryTemp;
		IFFRead( iff, retryTemp );
		retryType = static_cast<RetryType>(retryTemp);

		IFFRead( iff, retryLimit );
		target.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );

		// new version 3
		if ( ver >= 3 )
		{
			IFFRead( iff, flags );
		}
	}

	// Restart the state
	if ( target.GetType() == Target::eTargetPos )
	{
		Enter( target.GetPos(), AP, flags, retryType, retryLimit );
	}
	else if ( target.GetType() == Target::eTargetEntity )
	{
		Enter( target.GetEntity(), AP, false, flags, retryType, retryLimit );
	}
	else
	{
		dbBreak();
	}
}