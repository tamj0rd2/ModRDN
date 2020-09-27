/////////////////////////////////////////////////////////////////////
// File    : RDNNISletInterface.cpp
// Desc    : 
// Created : Thursday, Jan 03, 2002
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNNISletInterface.h"

#include "../ModObj.h"

#include "../Simulation/CommandTypes.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNPlayer.h"

#include <SimEngine/Pathfinding/Pathfinding.h>

///////////////////////////////////////////////////////////////////// 
// 

static RDNNISletInterface* s_instance = NULL; 

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNNISletInterface::RDNNISletInterface()
{

}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNNISletInterface::Initialize( void )
{
	// already initialized 
	dbAssert(!s_instance);

	s_instance = new RDNNISletInterface;

	return true;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNNISletInterface::Shutdown( void )
{
	// never initialized 
	dbAssert(s_instance);

	DELETEZERO(s_instance);
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNNISletInterface* RDNNISletInterface::Instance(void)
{
	// never initialized 
	dbAssert(s_instance);

	return s_instance;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNNISletInterface::Teleport( const EntityGroup& eg, const Vec2f& destination )
{
	// order group to stop
	if (!eg.empty())
	{
		const Entity* ent = eg.front();
		const Player* player = ent->GetOwner();
		unsigned long playerID = Player::InvalidID;
		if (player)
		{
			playerID = player->GetID();
		}

		std::vector< EntityIDNumber >	a;

		// call the simengine to convert this group to an array of ints
		EntityGroupToArray( a, eg ); 
		
		ModObj::i()->GetWorld()->DoCommandEntity
			(
				CMD_Stop,
				0, 
				false,
				playerID,
				&a[0],
				a.size()
			);
	}

	// teleport group to new location
	ModObj::i()->GetWorld()->GetPathfinder()->TeleportGroup( eg, destination );
}