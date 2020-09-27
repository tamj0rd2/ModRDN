/////////////////////////////////////////////////////////////////////
// File    : RDNGhost.cpp
// Desc    : 
// Created : Tuesday, February 18, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//
#include "pch.h"

#include "RDNGhost.h"

#include "../ModObj.h" 

#include "../Simulation/RDNQuery.h"
#include "../Simulation/RDNWorld.h" 

#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/ExtInfo/UIExtInfo.h"

#include <Assist/StlExVector.h>

///////////////////////////////////////////////////////////////////////////////
//
//	Ghost callbacks
//
static bool isGhosted_Never( Ghost* )
{
	return false;
}

static const RDNPlayer* s_pGhostWatcher = NULL;

static bool isGhosted_Possibly( Ghost* pGhost )
{
	dbAssert( s_pGhostWatcher );

	const Entity* pEntity = pGhost->GetEntity();
	if ( pEntity )
	{
		//	When entities are alive, we need to check if they are revealed (from range attacks)
		return !s_pGhostWatcher->FoWIsVisible( pEntity );
	}
	else
	{
		//	When entities don't exist
		const Player* otherPlayer = pGhost->GetOwner();
		const ControllerBlueprint* pCBP = pGhost->GetControllerBP();
		const Matrix43f& transform = pGhost->GetTransform();

		return !s_pGhostWatcher->FoWIsVisible( otherPlayer, pCBP, transform );
	}
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
RDNGhost::RDNGhost( const RDNPlayer* pWatcher )
:	m_pWatcher( pWatcher ),
	m_LastGameTickUpdate( 0 )
{
	//	addGhost() all the existing entities
	RDNWorld* pRDNWorld = ModObj::i()->GetWorld();
	const std::vector<Entity*>& cEntity = pRDNWorld->GetEntities();
	std::vector<Entity*>::const_iterator i = cEntity.begin();
	std::vector<Entity*>::const_iterator e = cEntity.end();
	for ( ; i!=e; ++i )
	{
		addGhost( *i );
	}

	//	Add our observer to all the RDN players
	size_t p, nPlayer = ModObj::i()->GetWorld()->GetPlayerCount();
	for ( p=0; p!=nPlayer; ++p )
	{
		RDNPlayer* pPlayer = static_cast<RDNPlayer*>( ModObj::i()->GetWorld()->GetPlayerAt( p ) );
		dbAssert( pPlayer );

		pPlayer->AddObserver( this );
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
RDNGhost::~RDNGhost()
{
	//	Add our observer to all the RDN players
	size_t p, nPlayer = ModObj::i()->GetWorld()->GetPlayerCount();
	for ( p=0; p!=nPlayer; ++p )
	{
		RDNPlayer* pPlayer = static_cast<RDNPlayer*>( ModObj::i()->GetWorld()->GetPlayerAt( p ) );
		dbAssert( pPlayer );

		pPlayer->RemoveObserver( this );
	}

	GhostCont::iterator i = m_cGhost.begin();
	GhostCont::iterator e = m_cGhost.end();
	for ( ; i!=e; ++i )
	{
		ModObj::i()->GetGhostInterface()->GhostDelete( *i );
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::OnEntityCreate( const Entity* pEntity )
{
	addGhost( pEntity );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::Update()
{
	const World* pWorld = ModObj::i()->GetWorld();

	if( m_LastGameTickUpdate == pWorld->GetGameTicks() )
	{
		// already been updated this simtick
		return;
	}

	// remember the time
	m_LastGameTickUpdate = pWorld->GetGameTicks();

	GhostInterface::IsGhostedFunc* pIsGhostedFunc;
	if ( (m_pWatcher == NULL) || m_pWatcher->IsPlayerDead() )
		pIsGhostedFunc = isGhosted_Never;
	else
		pIsGhostedFunc = isGhosted_Possibly;

	s_pGhostWatcher = m_pWatcher;
	ModObj::i()->GetGhostInterface()->Update( pIsGhostedFunc );
	s_pGhostWatcher = NULL;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
const std::vector<Ghost*>& RDNGhost::GetGhostContainer() const
{
	return m_cGhost;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::OnIncResourceCash( const RDNPlayer*, float, RDNPlayer::ResourceIncreased )
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::OnDecResourceCash( const RDNPlayer*, float )
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::OnAddEntity( const RDNPlayer* pPlayer, const Entity* entity )
{
	if ( pPlayer == m_pWatcher )
		removeGhost( entity );
	else
		resetGhost( entity );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::OnRemoveEntity( const RDNPlayer* pPlayer, const Entity* entity )
{
	if ( pPlayer == m_pWatcher )
		addGhost( entity );
	else
		resetGhost( entity );
}


/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
bool RDNGhost::isGhostable( const Entity* pEntity )
{
	const ModController* pModController = static_cast<const ModController*>(pEntity->GetController());
	if ( !pModController )
		return false;

	const UIExtInfo* pUIExtInfo = QIExtInfo< UIExtInfo >( pModController );
	return ( pUIExtInfo && pUIExtInfo->bGhostable );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::addGhost( const Entity* pEntity )
{
	//	You don't need ghosts for your own player
	if ( !m_pWatcher || m_pWatcher->CanControlEntity(pEntity) )
		return;

	if ( !isGhostable( pEntity ) )
		return;

	//	Create a ghost for this sucka
	Ghost* pGhost = ModObj::i()->GetGhostInterface()->GhostNew( pEntity );
	if ( pGhost )
	{
		m_cGhost.push_back( pGhost );
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::removeGhost( const Entity* pEntity )
{
	GhostCont::iterator i = findGhost( pEntity );
	if ( i != m_cGhost.end() )
	{
		ModObj::i()->GetGhostInterface()->GhostDelete( *i );
		m_cGhost.erase( i );	//	Remove from place to preserve Ghost list order
	}	
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void RDNGhost::resetGhost( const Entity* pEntity )
{
	GhostCont::iterator i = findGhost( pEntity );
	if ( i != m_cGhost.end() )
	{
		(*i)->Reset();
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
GhostCont::iterator RDNGhost::findGhost( const Entity* pEntity )
{
	GhostCont::iterator i = m_cGhost.begin();
	GhostCont::iterator e = m_cGhost.end();

	if ( !isGhostable( pEntity ) )
		return e;

	for ( ; i!=e; ++i )
	{
		if ( (*i)->GetEntity() == pEntity )
			return i;
	}
	return e;
}
