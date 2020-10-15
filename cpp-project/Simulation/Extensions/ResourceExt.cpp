/////////////////////////////////////////////////////////////////////
// File    : HealthExtension.cpp
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "ResourceExt.h"

#include "../RDNTuning.h"
#include "../GameEventDefs.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/ResourceExtInfo.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/Pathfinding/ImpassMap.h>

#include <Util/Iff.h>
#include <Util/Biff.H>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ResourceExt::ResourceExt()
		: m_numResources(0)
{
}

float ResourceExt::DecResources(float amount)
{
	// validate parm
	dbAssert(amount >= 0);

	// If i have none, you get none bwahahaha
	if (m_numResources == 0)
	{
		return 0;
	}
	//
	if (amount > m_numResources)
		amount = m_numResources;

	//
	m_numResources -= amount;

	//
	const float progress = m_numResources / m_maxResources;

	OnResourceProgress(progress);

	// check for zero
	if (m_numResources == 0)
	{
		// generate event
		GameEventSys::Instance()->PublishEvent(GameEvent_ResourceGatherDepleted(GetSelf()->GetEntity(), m_gatherers));

		// delegate
		OnZeroResources();
	}

	return amount;
}

float ResourceExt::GetResources() const
{
	return m_numResources;
}

void ResourceExt::SetResources(float amount)
{
	// validate parm
	dbAssert(amount > 0);

	//
	m_numResources = amount;
	m_maxResources = amount;

	const float progress = m_numResources / m_maxResources;

	OnResourceProgress(progress);

	return;
}

void ResourceExt::OnResourceProgress(float progress)
{
	GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Resource", progress);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::SaveExt(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	biff.StartChunk(Type_NormalVers, 'ERCE', "Extension: Resource", 1);

	IFFWrite(iff, m_numResources);
	IFFWrite(iff, m_maxResources);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleERCE, Type_NormalVers, 'ERCE', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long ResourceExt::HandleERCE(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	ResourceExt *pResourceExt = static_cast<ResourceExt *>(pContext1);
	dbAssert(pResourceExt);

	IFFRead(iff, pResourceExt->m_numResources);
	IFFRead(iff, pResourceExt->m_maxResources);

	const float progress = pResourceExt->m_numResources / pResourceExt->m_maxResources;

	pResourceExt->OnResourceProgress(progress);

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::BurnInCantBuild(TerrainCellMap *pTCMap)
{
	UNREF_P(pTCMap);
	/***
	// Burn in an extra perimmeter around myself so that no structures can be built close to me
	long cellx, cellz;

	cellx = pTCMap->WorldXToCellX( GetSelf()->GetEntity()->GetPosition().x );
	cellz = pTCMap->WorldZToCellZ( GetSelf()->GetEntity()->GetPosition().z );

	long width, height;
	width = height = pTCMap->WorldToCellDim( RDNTuning::Instance()->GetBuildingInfo().resourceNoBuildSize );

	cellx -= width/2;  cellz -= height/2;

	pTCMap->BlitCells( cellx, cellz, width, height, eCantBuild, TerrainCellMap::OP_OR );
***/
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::UnBurnCantBuild(TerrainCellMap *pTCMap)
{
	UNREF_P(pTCMap);
	/***
	// Un-Burn in the extra perimmeter around myself so that no structures can be built close to me
	long cellx, cellz;

	cellx = pTCMap->WorldXToCellX( GetSelf()->GetEntity()->GetPosition().x );
	cellz = pTCMap->WorldZToCellZ( GetSelf()->GetEntity()->GetPosition().z );

	long width, height;
	width = height = pTCMap->WorldToCellDim( RDNTuning::Instance()->GetBuildingInfo().resourceNoBuildSize );

	cellx -= width/2;  cellz -= height/2;

	unsigned char mask = (unsigned char)(~eCantBuild);
	pTCMap->BlitCells( cellx, cellz, width, height, mask, TerrainCellMap::OP_AND );
***/
}

void ResourceExt::GathererAdd(const Entity *p_Entity)
{
	m_gatherers.push_back(const_cast<Entity *>(p_Entity));
}
