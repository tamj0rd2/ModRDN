#include "pch.h"
#include "BuildingExt.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../RDNPlayer.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/CostExtInfo.h"

#include <SimEngine/EntityAnimator.h>

#include <Util/Iff.h>
#include <Util/Biff.H>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//

BuildingExt::BuildingExt(const SiteExtInfo *pSiteExtInfo, const CostExtInfo *costExtInfo)
		: m_constructionTicks(NULL)
{
	m_buildCompletion = 0;

	if (costExtInfo)
	{
		m_constructionTicks = costExtInfo->constructionTicks;
	}
}

void BuildingExt::IncrementCompletion()
{
	m_buildCompletion = m_buildCompletion + m_buildersOnSite.size();
	float progress = (float)m_buildCompletion / m_constructionTicks;
	GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Build", progress);
}

void BuildingExt::AddBuilder(const Entity *builder)
{
	m_buildersOnSite.push_back(const_cast<Entity *>(builder));
}

void BuildingExt::RemoveBuilder(const Entity *builder)
{
	m_buildersOnSite.remove(builder);
}

bool BuildingExt::IsBuilt() const
{
	return m_buildCompletion >= m_constructionTicks;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingExt::SaveExt(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	biff.StartChunk(Type_NormalVers, 'EBLD', "Extension: Building", 0);

	// TODO: save variables here
	// IFFWrite(iff, m_sightRadius);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleEBLD, Type_NormalVers, 'EBLD', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long BuildingExt::HandleEBLD(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	BuildingExt *pBuildingExt = static_cast<BuildingExt *>(pContext1);
	dbAssert(pBuildingExt);

	// TODO: load variables here
	// IFFRead(iff, pBuildingExt->m_sightRadius);

	return 0;
}
