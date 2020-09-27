/////////////////////////////////////////////////////////////////////
// File    : ResourceExt.h
// Desc    : 
// Created : Tuesday, September 18, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

#include <SimEngine/EntityGroup.h>

///////////////////////////////////////////////////////////////////// 
// Forward Declarations
class TerrainCellMap;

///////////////////////////////////////////////////////////////////////////////
// ResourceExt 

class ResourceExt : public Extension
{
// types
public:
	enum
	{
		ExtensionID = EXTID_Resource,
	};

// interface
public:
	virtual float		GetResources() const;
	virtual void		SetResources( float amount );

	// return amount decreased
	virtual float		DecResources( float amount );

	// list of henchmen actively gathering this site
	const EntityGroup&	Gatherers() const;
	void				GathererAdd( const Entity* );
	void				GathererRmv( const Entity* );

	// list of henchmen currently pickup up stuff from this site
	const EntityGroup&	GatherersOnSite() const;
	bool				GatherersOnSiteIsAtMax() const;
	size_t				GatherersOnSiteMax() const;
	void				GatherersOnSiteAdd( const Entity* );
	void				GatherersOnSiteRmv( const Entity* );

	void				BurnInCantBuild	( TerrainCellMap* );
	void				UnBurnCantBuild	( TerrainCellMap* );

protected:
	virtual void OnZeroResources() = 0;

private:
	virtual void OnResourceProgress( float amount );

// inherited interface: Extension
private:

	virtual void SaveExt( BiFF& ) const;
	virtual void LoadExt( IFF& );

	// Chunk Handlers
	static unsigned long HandleERCE( IFF&, ChunkNode*, void*, void* );

// construction
public:
	ResourceExt();

// fields
private:
	float		m_numResources;
	float		m_maxResources;

	// these are transient
	// - they are only modified by entity states and will be re-established at load time
	EntityGroup	m_gatherers;	
	EntityGroup	m_gatherersOnSite;	
};
