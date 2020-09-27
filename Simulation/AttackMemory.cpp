/////////////////////////////////////////////////////////////////////
// File    : AttackMemory.cpp
// Desc    : 
// Created : Thursday, February 14, 2002
// Author  : dswinerd
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "AttackMemory.h"

#include <SimEngine/Player.h>

/////////////////////////////////////////////////////////////////////
//	Desc.	: AttackMemory constructor
//	Author	: dswinerd
//
AttackMemory::AttackMemory()
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: AttackMemory destructor
//	Author	: dswinerd
//
AttackMemory::~AttackMemory()
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: adds an attack memory
//	Param.	: pEntity - the attacking entity
//			  time - the time of the attack.  in GameTicks
//	Author	: dswinerd
//	
void AttackMemory::SetAttackedBy( Entity* pEntity, long time )
{
	m_memory.AddMemory( pEntity, time );

	return;
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if we were attacked by the given entity
//	Result	: returns true if we remember being attacked
//	Param.	: pEntity - did this Entity attack us
//			  time - we want attacks after this time
//	Author	: dswinerd
//
bool AttackMemory::WasAttackedBy( Entity *pEntity, long time ) const
{
	return m_memory.HaveMemory( pEntity, time );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if we were attacked by the given player
//	Result	: returns true if we remember being attacked
//	Param.	: pPlayer - did this Player attack us
//			  time - we want attacks after this time
//	Author	: clee
//
bool AttackMemory::WasAttackedBy( Player *pPlayer, long time ) const
{
	return m_memory.HaveMemory( pPlayer, time );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if we have a memory of being attacked since a given time
//	Result	: returns true if attacked
//	Param.	: time - the time we remember be attacked since
//	Author	: dswinerd
//
bool AttackMemory::HasBeenAttackedSince( long time ) const
{
	return m_memory.HaveMemoriesSince( time );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns an EntityGroup of all the guys that I remember attacking me since the given time
//	Result	: 
//	Param.	: output EntityGroup& attackers
//			  time - we want attacks after this time.  In GameTicks.
//	Author	: dswinerd
//
void AttackMemory::GetAttackers( EntityGroup& attackers, long time ) const
{
	m_memory.GetMemories( attackers, time );

	return;
}
