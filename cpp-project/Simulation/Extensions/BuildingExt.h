#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

#include <SimEngine/EntityGroup.h>

class SiteExtInfo;
class CostExtInfo;

///////////////////////////////////////////////////////////////////////////////
// BuildingExt

class BuildingExt : public Extension
{
	// types
public:
	enum
	{
		ExtensionID = EXTID_Build,
	};

	// interface
public:
	void AddBuilder(const Entity *);
	void RemoveBuilder(const Entity *);
	bool BuildingExt::IsBuilt() const;
	void IncrementCompletion();

private:
	virtual void SaveExt(BiFF &) const;
	virtual void LoadExt(IFF &);

	// Call this once per pick/simstep

	// Chunk Handlers
	static unsigned long HandleEBLD(IFF &, ChunkNode *, void *, void *);

	// construction
protected:
	BuildingExt(const SiteExtInfo *, const CostExtInfo *);

	// fields
private:
	long m_buildCompletion;

	EntityGroup m_buildersOnSite;
	long m_constructionTicks;
};
