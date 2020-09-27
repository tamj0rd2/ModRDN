/////////////////////////////////////////////////////////////////////
// File    : State.cpp
// Desc    : 
// Created : Thursday, November 01, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "state.h"

#include <SimEngine/EntityDynamics.h>
#include <SimEngine/SimEntity.h>

///////////////////////////////////////////////////////////////////// 
// State
//

State::State( EntityDynamics *e_dynamics ) 
:	m_pDynamics( e_dynamics ),
	m_bExiting( true )
{
}

bool State::AcceptCommand( int )
{
	return true;
}

void State::SoftExit()
{
}

void State::ForceExit()
{
}

State* State::GetSubState( unsigned char StateID )
{
	if ( IsExiting() )
	{
		return NULL;
	}
	
	if ( GetStateID() == StateID )
	{
		return this;
	}

	return NULL;
}

Entity* State::GetEntity( )
{
	return m_pDynamics->GetEntity();
}

const Entity* State::GetEntity( ) const
{
	return m_pDynamics->GetEntity();
}