/////////////////////////////////////////////////////////////////////
// File    : Objective.cpp
// Desc    : 
// Created : Friday, September 21, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "Objective.h"

#include "RDNHUD.h"
#include "RDNTaskbar.h"

#include "../RDNDllSetup.h"

#include "../Simulation/GameEventDefs.h"
#include "../Simulation/GameEventSys.h"

#include <util/iff.h>

//------------------------------------------------------------------------------------
// Objective::Objective
//------------------------------------------------------------------------------------

Objective::Objective()
{
	m_ID			= 0;
	m_shortDescID	= 0;
	m_tipID			= 0;
	m_type			= OT_Primary;
	m_state			= OS_Off;
}

Objective::~Objective()
{
}

void Objective::Load( IFF& iff )
{
	long temp;

	IFFRead( iff, m_shortDescID );
	IFFRead( iff, m_tipID );

	IFFRead( iff, temp );
	m_type = static_cast<Type>(temp);

	IFFRead( iff, temp );
	m_state = static_cast<State>(temp);
}

void Objective::Save( IFF& iff )
{
	long temp;

	IFFWrite( iff, m_shortDescID );
	IFFWrite( iff, m_tipID );

	temp = static_cast<long>(m_type);
	IFFWrite( iff, temp );

	temp = static_cast<long>(m_state);
	IFFWrite( iff, temp );
}

Entity* Objective::GetEntity() const
{
	if (m_eGroup.size() == 0)
	{
		return NULL;
	}

	Entity* pEntity = const_cast<Entity*>( m_eGroup.front() );
	return pEntity;
}

void Objective::SetEntity( Entity* pEntity )
{
	m_eGroup.clear();
	m_eGroup.push_back( pEntity );
}

void Objective::SetState( Objective::State state )
{
	m_state = state;

	return;
}
