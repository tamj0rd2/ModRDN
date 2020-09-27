/////////////////////////////////////////////////////////////////////
// File    : StateGroupMove.h
// Desc    : 
// Created : Wednesday, May 1, 2002
// Author  : dswinerd
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateGroupMove.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../CommandParams.h"
#include "../CommandTypes.h"

#include "../Controllers/ModController.h"

#include "../Extensions/MovingExt.h"

#include "StateMove.h"

#include <SimEngine/Player.h>
#include <SimEngine/EntityDynamics.h>

#include <Util/Iff.h>
#include <Util/IffMath.h>
#include <Util/Biff.h>


///////////////////////////////////////////////////////////////////// 
// Desc.     : StateGroupMove constructor
// Param.    : e_dynamics - pointer to the dynamics of the entity
//
StateGroupMove::StateGroupMove( EntityDynamics *e_dynamics ) :
	State( e_dynamics ),
	m_pStateMove( 0 ),
	m_AP( 0 ),
	m_enterTick( 0 ),
	m_subState( SS_Invalid )
{
	m_target.SetType( Target::eTargetPos );
	m_target.SetPos( Vec3f(0.0f, 0.0f, 0.0f ) );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns SID_GroupMove
//
State::StateIDType StateGroupMove::GetStateID( ) const
{
	return (State::StateIDType)StateID;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : initialize the state
// Param.    : pMove - the StateMove of the Entity
//
void StateGroupMove::Init( StateMove* pMove )
{
	dbAssert( pMove != 0 );
	dbAssert( m_pStateMove == 0 );

	m_pStateMove	= pMove;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: enter move to an entity
//	Param.	: pEntity - the target
//			  AP - the Acceptable Proximity
//
void StateGroupMove::Enter( Entity* pEntity, float AP, unsigned long flags )
{
	m_target.SetType( Target::eTargetEntity );
	m_target.SetEntity( pEntity );

	m_flags = flags;
	m_AP = AP;

	// do common processing
	DoInternalEnter();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: enter move to a destination
//	Param.	: destination - the target position
//			  AP - the Acceptable Proximity
//
void StateGroupMove::Enter( const Vec3f& destination, float AP, unsigned long flags )
{
	m_target.SetType( Target::eTargetPos );
	m_target.SetPos( destination );

	m_flags = flags;
	m_AP = AP;

	// do common processing
	DoInternalEnter();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: code common to both Enter routines
//
void StateGroupMove::DoInternalEnter()
{
	m_enterTick = ModObj::i()->GetWorld()->GetGameTicks();
	m_subState = SS_WaitingForGroup;

	SetExitStatus( false );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::EnterGroup( const Vec3f& offset, float maxSpeed )
{
	dbAssert( m_subState == SS_WaitingForGroup );

	// got what we were waiting for, we're actually part of a group
	ToGroup( offset, maxSpeed );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateGroupMove::AddWayPoint( const Vec3f& point )
{
	//
	m_pStateMove->AddWayPoint( point );
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: main update call
//	Result	: returns true if the state is done
//
bool StateGroupMove::Update()
{
	bool bReturnValue = false;

	if ( IsExiting() )
	{
		bool bResult = true;
		switch( m_subState )
		{
			//
			case SS_WaitingForGroup:
				bResult = true;
				break;
			//
			case SS_Solo:
				bResult = HandleSolo();
				break;
			//
			case SS_Group:
				bResult = HandleGroup();
				break;
		}

		if ( bResult )
		{
			// leave the group
			//ExitGroup();
		}

		return bResult;
	}

	switch( m_subState )
	{
		//
		case SS_WaitingForGroup:
		{
			bReturnValue = HandleWaitingForGroup();
			break;
		}
		//
		case SS_Group:
		{
			bReturnValue = HandleGroup();
			break;
		}
		case SS_Solo:
		{
			bReturnValue = HandleSolo();
			break;
		}
		//
		default:
			// should never hit thiis
			dbBreak();
			break;
	}

	if ( bReturnValue )
	{
		ExitGroup();
	}

	return bReturnValue;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: process while we are waiting a tick for an EnterGroup to happen
//
bool StateGroupMove::HandleWaitingForGroup()
{
	if ( ModObj::i()->GetWorld()->GetGameTicks() > m_enterTick )
	{
		// if we were part of a group, EnterGroup would have been called by now

		ToSolo();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: process while moving solo
//	Result	: returns true if the state is done
//
bool StateGroupMove::HandleSolo()
{
	return m_pStateMove->Update();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: process while in a group
//	Result	: returns true if the state is done
//
bool StateGroupMove::HandleGroup()
{
	bool bResult = m_pStateMove->Update();

	// is it time to leave the group and head on own?
	if ( m_pStateMove->GetNumWayPointsVisited() > 0 )
	{
		ExitGroup();
		m_subState = SS_Solo;
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: starts the entity on a group move
//
void StateGroupMove::ToGroup( const Vec3f& offset, float maxSpeed )
{
	m_subState = SS_Group;

	// let the dynamics know we are doing a groupmove
	//RequestGroupMove( m_target, m_AP ) would go here

	if ( !( m_flags & CMD_MP_NoGroupSpeed ) )
	{
		// go at the group speed
		MovingExt* pMoveExt = QIExt< MovingExt >( GetEntity() );
		if ( pMoveExt && maxSpeed > 0 )
		{
			pMoveExt->SetSpeedOverride( maxSpeed );
		}
	}

	// fire up StateMove
	if ( m_target.GetType() == Target::eTargetEntity )
	{
		// moving to an Entity
		m_pStateMove->Enter( m_target.GetEntity(), m_AP, false, 0, StateMove::RT_BlockedOnEntityAllowSpace, 4 );
	}
	else
	{
		// moving to a destination
		m_pStateMove->Enter( m_target.GetPos(), offset, m_AP, 0, StateMove::RT_BlockedOnEntityAllowSpace, 4 );
	}		
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: starts the entity on a solo move
//
void StateGroupMove::ToSolo()
{
	// just going solo
	m_subState = SS_Solo;

	// fire up StateMove
	if ( m_target.GetType() == Target::eTargetEntity )
	{
		// moving to an Entity
		m_pStateMove->Enter( m_target.GetEntity(), m_AP, false, 0, StateMove::RT_BlockedOnEntityAllowSpace, 4 );
	}
	else
	{
		// moving to a destination
		m_pStateMove->Enter( m_target.GetPos(), m_AP, 0, StateMove::RT_BlockedOnEntityAllowSpace, 4 );
	}		
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: let the state know it should exit
//
void StateGroupMove::RequestExit()
{
	switch( m_subState )
	{
		case SS_WaitingForGroup:
			break;
		case SS_Solo:
		case SS_Group:
			m_pStateMove->RequestExit();
			break;
	}

	SetExitStatus( true );

	//
	ExitGroup();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::ReissueOrder() const
{
	const unsigned long playerID = GetEntity()->GetOwner() ? GetEntity()->GetOwner()->GetID() : 0;
	const unsigned long entityID = GetEntity()->GetID();

	if ( m_target.GetType() == Target::eTargetEntity )
	{	
		// moving to an entity
		if ( m_target.GetEntity() )
		{
			
			const unsigned long targetID = m_target.GetEntity()->GetID();

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
	{	
		// moving to a point
		ModObj::i()->GetWorld()->DoCommandEntityPoint( CMD_Move,		
														0,				
														CMDF_Queue,
														playerID,
														&entityID,
														1,
														&m_target.GetPos(),
														1
														);

	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::ForceExit()
{
	ExitGroup();
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: called when the Entity is leaving a group
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::ExitGroup( )
{
	Entity *pMe = GetEntity();
	if ( pMe->GetController() )
	{
		if ( pMe->GetController()->GetGroupController() )
		{
			pMe->GetController()->GetGroupController()->RemoveEntity( pMe );
		}
	}
	else
	{
		// why would an entity without a controller be in a group
		dbBreak();
	}

	MovingExt* pMoveExt = QIExt< MovingExt >( GetEntity() );
	if ( pMoveExt )
	{
		pMoveExt->RemoveSpeedOverride();
	}

}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
State* StateGroupMove::GetSubState( unsigned char id )
{
	if ( IsExiting() )
	{
		return NULL;
	}

	if ( id == StateID )
	{
		return this;
	}
	else
	{
		switch ( m_subState )
		{
			case SS_Group:
				return m_pStateMove->GetSubState( id );
			case SS_Solo:
				return m_pStateMove->GetSubState( id );
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
bool StateGroupMove::CanGroup() const
{
	return m_subState == SS_WaitingForGroup;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::SaveState( BiFF& biff ) const
{
	IFF& iff = *biff.GetIFF();

	unsigned long ver = 2;
	IFFWrite( iff, ver );

	IFFWrite( iff, m_AP );
	m_target.SaveEmbedded( iff );
	
	// new ver 2
	IFFWrite( iff, m_flags );

	bool bGroup = m_subState == SS_Group;
	IFFWrite( iff, bGroup );
	if ( bGroup )
	{
		//
		Vec3f offset = m_pStateMove->GetOffset();
		IFFWrite( iff, offset );

		//
		float maxSpeed = 0;
		const MovingExt* pMoveExt = QIExt< MovingExt >( GetEntity() );
		if ( pMoveExt )
		{
			maxSpeed = pMoveExt->GetSpeedOverride( );
		}
		IFFWrite( iff, maxSpeed );
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: dswinerd
//
void StateGroupMove::LoadState( IFF& iff )
{
	unsigned long ver;
	IFFRead( iff, ver );

	float AP;
	IFFRead( iff, AP );

	Target target;
	target.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );

	unsigned long flags = 0;
	if ( ver >= 2 )
	{
		IFFRead( iff, flags );
	}

	if ( target.GetType() == Target::eTargetPos )
	{
		Enter( target.GetPos(), AP, flags );
	}
	else
	{
		Enter( target.GetEntity(), AP, flags );
	}

	bool bGroup = false;
	IFFRead( iff, bGroup );
	if ( bGroup )
	{
		Vec3f offset;
		IFFRead( iff, offset );

		float maxSpeed = 0;
		IFFRead( iff, maxSpeed );

		EnterGroup( offset, maxSpeed );
	}
}
