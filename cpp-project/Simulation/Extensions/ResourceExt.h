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
class ResourceExtInfo;

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

	// construction
public:
	ResourceExt(const ResourceExtInfo *);

	// interface
public:
	virtual float GetResources() const;
	bool IsDepleted() const;

	// return amount decreased
	float DecResources(float amount);

	const EntityGroup &Gatherers() const;

	size_t GetGathererCount() const;
	bool HasNoOtherGatherers(const Entity *pEntity) const;
	void GathererAdd(const Entity *);
	void GathererRmv(const Entity *);

	const EntityGroup &GatherersOnSite() const;
	void GatherersOnSiteAdd(const Entity *);
	void GatherersOnSiteRmv(const Entity *);
	bool CanGatherResourcesOnSiteNow(const Entity *) const;

	size_t GatherersOnSiteMax() const;
	bool GatherersOnSiteIsAtMax() const;

	void BurnInCantBuild(TerrainCellMap *);
	void UnBurnCantBuild(TerrainCellMap *);

protected:
	void InitLooks();
	virtual void OnZeroResources() = 0;

private:
	virtual void OnResourceProgress(float amount);
	bool HasSpaceForGathererOnSite(const Entity *pEntity) const;

	// inherited interface: Extension
private:
	virtual void SaveExt(BiFF &) const;
	virtual void LoadExt(IFF &);

	// Chunk Handlers
	static unsigned long HandleERCE(IFF &, ChunkNode *, void *, void *);

	// fields
private:
	float m_numResources;
	float m_maxResources;
	int m_maxGatherersOnSite;

	// these are transient
	// - they are only modified by entity states and will be re-established at load time
	EntityGroup m_gatherers;
	EntityGroup m_gatherersOnSite;
};
