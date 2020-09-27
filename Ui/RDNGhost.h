/////////////////////////////////////////////////////////////////////
// File    : RDNGhost.h
// Desc    : 
// Created : Tuesday, February 18, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include <EngineAPI/GhostInterface.h>
#include <SimEngine/EntityGroup.h>

#include "../Simulation/RDNPlayer.h"

///////////////////////////////////////////////////////////////////// 
// Forward Declarations
class Entity;
class RDNPlayer;

///////////////////////////////////////////////////////////////////////////////
//
//	RDNGhost
//
class RDNGhost : public RDNPlayer::Observer
{
// construction
public:
	 RDNGhost( const RDNPlayer* );
	~RDNGhost();

// interface
public:
	void	OnEntityCreate( const Entity* );

	void	Update();

	const std::vector<Ghost*>&
			GetGhostContainer() const;

// interface : RDNPlayer::Observer
	virtual void	OnIncResourceCash	( const RDNPlayer*, float amount, RDNPlayer::ResourceIncreased reason );
	virtual void	OnDecResourceCash	( const RDNPlayer*, float amount );

	virtual void	OnAddEntity			( const RDNPlayer*, const Entity* entity );
	virtual void	OnRemoveEntity		( const RDNPlayer*, const Entity* entity );

// fields
private:
	typedef std::vector<Ghost*> GhostCont;

	const RDNPlayer*	m_pWatcher;
	GhostCont			m_cGhost;

	long				m_LastGameTickUpdate;

// implementation
private:
	bool	isGhostable( const Entity* );
	void	addGhost( const Entity* );
	void	removeGhost( const Entity* );
	void	resetGhost( const Entity* );
	GhostCont::iterator	findGhost( const Entity* );
};
