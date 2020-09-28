/////////////////////////////////////////////////////////////////////
// File    : WorldFOW.cpp
// Desc    :
// Created : Tuesday, January 08, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//
#include "pch.h"

#include "WorldFOW.h"

#include "../ModObj.h"
#include "RDNWorld.h"
#include "Controllers/ModController.h"
#include "Extensions/SightExt.h"

#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/EntityGroup.h>

#include <Util/PerfBlock.h>
#include <Util/Iff.h>

/////////////////////////////////////////////////////////////////////
//
//	Constants/Definitions
//

const unsigned long k_AllPlayers = 0xffffffff;

/////////////////////////////////////////////////////////////////////
//
//	Global/Static functions
//

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
WorldFOW::WorldFOW(float islandwidth, float islandheight) : m_nextPlayer(0),
																														m_widthBy2(islandwidth / 2.0f),
																														m_heightBy2(islandheight / 2.0f)
{
	m_FowStatus.SetSize((unsigned long)(islandwidth / GetCellSize()),
											(unsigned long)(islandheight / GetCellSize()));

	m_FowStatus.FillValue(0);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
WorldFOW::~WorldFOW()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOWID WorldFOW::GetNextPlayerFOWID()
{
	dbAssert(m_nextPlayer < 32);

	unsigned long playerMask = 0x0000000f;

	playerMask <<= m_nextPlayer;

	m_nextPlayer += 4;

	return playerMask;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void WorldFOW::CreateSharedPlayerFOWID(PlayerFOWID &dest, const PlayerFOWID &share, bool bShare) const
{
	if (bShare)
	{
		dest |= share;
	}
	else
	{
		dest &= ~share;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void WorldFOW::RevealAll()
{
	unsigned long mask = CreateCellMask(k_AllPlayers, CreateFOWMask(FOWC_Visible, FOWC_Explored));

	unsigned long *pStart = m_FowStatus.GetData();
	unsigned long *pEnd = m_FowStatus.GetData() + m_FowStatus.GetSize();

	for (; pStart != pEnd; ++pStart)
	{
		(*pStart) |= mask;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void WorldFOW::Save(IFF &iff) const
{
	iff.PushChunk(Type_NormalVers, 'WFOW');

	IFFWrite(iff, GetWidth());
	IFFWrite(iff, GetHeight());

	IFFWriteArray(iff, m_FowStatus.GetData(), m_FowStatus.GetSize());

	iff.PopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void WorldFOW::Load(IFF &iff)
{
	iff.AddParseHandler(HandleWFOW, Type_NormalVers, 'WFOW', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long WorldFOW::HandleWFOW(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	WorldFOW *pWorldFOW = static_cast<WorldFOW *>(pContext1);

	unsigned long width;
	IFFRead(iff, width);
	dbAssert(width == pWorldFOW->GetWidth());

	unsigned long height;
	IFFRead(iff, height);
	dbAssert(height == pWorldFOW->GetHeight());

	IFFReadArray(iff, pWorldFOW->m_FowStatus.GetData(), pWorldFOW->m_FowStatus.GetSize());

	return 0;
}
