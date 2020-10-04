/////////////////////////////////////////////////////////////////////
// File    : PlayerFOW.cpp
// Desc    :
// Created : Tuesday, January 08, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"

#include "PlayerFOW.h"

#include "../ModObj.h"
#include "RDNWorld.h"
#include "RDNPlayer.h"
#include "WorldFOW.h"

#include "Controllers/ModController.h"

#include "Extensions/SightExt.h"

#include "ExtInfo/SiteExtInfo.h"

#include <Assist/StlExVector.h>

#include <EngineAPI/EntityFactory.h>

#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/BuildingDynamics.h>

#include <Util/PerfBlock.h>
#include <Util/Iff.h>

#include <Util/DebugRender.h>

#pragma inline_depth(16)

////////////////////////////////////////////////////////////////////
//
//
//
//

static void GetEntityFOWPoints(const ControllerBlueprint *pCBP, const Matrix43f &transform, size_t *outcells, const size_t numcells, const WorldFOW *pWorldFOW)
{
	dbAssert(outcells);
	dbAssert(numcells > 1);

	const Vec3f &epos = transform.T;

	size_t i = 0;

	const ECStaticInfo *si = NULL;
	if (pCBP)
		si = ModObj::i()->GetEntityFactory()->GetECStaticInfo(pCBP);

	if (si && QIExtInfo<SiteExtInfo>(si))
	{
		long width, height;
		BuildingDynamics::GetEntityWidthHeight(pCBP, &transform, width, height);

		Vec3f tlpos;
		tlpos.x = epos.x - width;
		tlpos.z = epos.z - height;

		width *= 2;
		height *= 2;

		size_t minx, minz;
		pWorldFOW->RemapPos2Cell(tlpos.x, tlpos.z, minx, minz);

		size_t maxx, maxz;

		width /= 4;
		height /= 4;

		maxx = minx + width;
		maxz = minz + height;

		size_t x, z, cellnum;

		cellnum = minz * pWorldFOW->GetWidth() + minx;
		for (z = minz; z <= maxz; ++z)
		{
			for (x = minx; x <= maxx; ++x)
			{
				dbAssert(z * pWorldFOW->GetWidth() + x == cellnum);

				outcells[i] = cellnum;
				i++;
				// have we run out of storage
				if (i == numcells - 1)
				{
					outcells[i - 1] = size_t(-1);
					dbBreak();
					return;
				}
				cellnum++;
			}
			cellnum += pWorldFOW->GetWidth() - (maxx - minx) - 1;
		}
		outcells[i] = size_t(-1);
	}
	else
	{
		size_t x, z;
		pWorldFOW->RemapPos2Cell(epos.x, epos.z, x, z);

		outcells[0] = z * pWorldFOW->GetWidth() + x;
		outcells[1] = size_t(-1);
	}
}

/////////////////////////////////////////////////////////////////////
//
//	Public members
//

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOW::PlayerFOW(WorldFOW *pWorldFOW) : m_pWorldFOW(pWorldFOW),
																						m_bAllRevealed(false),
																						m_bRadarPulseEnabled(false)
{
	m_playerFOWID = pWorldFOW->GetNextPlayerFOWID();
	m_FOWQuerryID = m_playerFOWID;

	SetSize(pWorldFOW->GetWidth(), pWorldFOW->GetHeight());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOW::~PlayerFOW()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::Update(const EntityGroup &playerEntities)
{
	//	Imprint the visual range of all the playerEntities we have here
	EntityGroup::const_iterator ei = playerEntities.begin();
	EntityGroup::const_iterator ee = playerEntities.end();
	for (; ei != ee; ++ei)
	{
		UpdateEntity(*ei);
	}

	UpdateTaggedEntities();

	UpdateRevealList();

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool PlayerFOW::IsVisible(const Entity *entity) const
{
	// all entities are visible (from the point of view of the fow) when the radar pulse is on
	if (m_bRadarPulseEnabled)
	{
		return true;
	}

	// if we have a shared vision with this entities FOWID then we can see it by default
	const RDNPlayer *pPlayer = static_cast<const RDNPlayer *>(entity->GetOwner());
	if (pPlayer)
	{
		if ((pPlayer->GetFogOfWar()->GetPlayerFOWID() & GetPlayerSharedFOWID()) != 0)
			return true;
	}

	if (IsVisible(entity->GetControllerBP(), entity->GetTransform()))
	{
		return true;
	}

	//	Check if this entity is revealed to this player
	RevealEntityList::const_iterator ei = m_revealEntityList.begin();
	RevealEntityList::const_iterator ee = m_revealEntityList.end();
	for (; ei != ee; ++ei)
	{
		if (ei->m_entity.front() == entity)
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool PlayerFOW::IsVisible(const ControllerBlueprint *pEBP, const Matrix43f &transform) const
{
	// all entities are visible (from the point of view of the fow) when the radar pulse is on
	if (m_bRadarPulseEnabled)
	{
		return true;
	}

	size_t entityFOWCells[512];

	GetEntityFOWPoints(pEBP, transform, entityFOWCells, LENGTHOF(entityFOWCells), m_pWorldFOW);

	size_t i;

	for (i = 0; entityFOWCells[i] != -1; ++i)
	{
		if (m_pWorldFOW->GetCell(m_FOWQuerryID, entityFOWCells[i], FOWC_Visible))
		{
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool PlayerFOW::IsVisible(const Vec3f &pos) const
{
	return m_pWorldFOW->GetCell(m_FOWQuerryID, pos.x, pos.z, FOWC_Visible);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool PlayerFOW::IsExplored(const Entity *entity) const
{
	size_t entityFOWCells[512];

	GetEntityFOWPoints(entity->GetControllerBP(), entity->GetTransform(), entityFOWCells, LENGTHOF(entityFOWCells), m_pWorldFOW);

	size_t i;

	for (i = 0; entityFOWCells[i] != -1; ++i)
	{
		if (m_pWorldFOW->GetCell(m_FOWQuerryID, entityFOWCells[i], FOWC_Explored))
		{
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::RevealArea(float x, float z, float radius, float duration, FOWChannelMask channel)
{
	RevealAreaRecord newRecord;

	newRecord.m_x = x;
	newRecord.m_z = z;
	newRecord.m_radius = radius;
	newRecord.m_endTime = duration + ModObj::i()->GetWorld()->GetGameTime();
	newRecord.m_mask = channel;
	newRecord.m_bMarked = false;

	m_revealAreaList.push_back(newRecord);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::UnRevealArea(float x, float z, float radius, FOWChannelMask channel)
{
	// find the record with matching parameters and set the end time to zero;
	// this would remove the revealed area in the next update
	RevealAreaList::iterator ai = m_revealAreaList.begin();
	RevealAreaList::iterator ae = m_revealAreaList.end();

	for (; ai != ae; ai++)
	{
		RevealAreaRecord &record = *ai;

		if ((record.m_x == x) &&
				(record.m_z == z) &&
				(record.m_radius == radius) &&
				(record.m_mask == channel))
		{
			record.m_endTime = 0.0f;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::RevealEntity(const Entity *pEntity, float duration)
{
	dbAssert(duration > 0);

	//	Check if we have this entity already
	RevealEntityList::iterator ei = m_revealEntityList.begin();
	RevealEntityList::iterator ee = m_revealEntityList.end();
	for (; ei != ee; ++ei)
	{
		if (ei->m_entity.front() == pEntity)
		{
			ei->m_endTime = ModObj::i()->GetWorld()->GetGameTime() + duration;
			return;
		}
	}

	RevealEntityRecord newRecord;
	newRecord.m_entity.push_back(const_cast<Entity *>(pEntity));
	newRecord.m_endTime = ModObj::i()->GetWorld()->GetGameTime() + duration;
	m_revealEntityList.push_back(newRecord);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::EnableRadarPulse(bool bEnable)
{
	m_bRadarPulseEnabled = bEnable;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
long PlayerFOW::GetExploredPercent() const
{
	//
	const size_t total = GetWidth() * GetHeight();

	long count = 0;

	for (size_t cell = 0; cell != total; ++cell)
	{
		if (m_pWorldFOW->GetCell(m_playerFOWID, cell, FOWC_Explored))
			count++;
	}

	const float v = float(count) / float(total);

	const float vPercent = v * 100.0f;
	if (vPercent > 0)
	{
		return __max(1, static_cast<int>(floorf(v * 100.0f)));
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::RemoveFromFOW(Entity *pEntity)
{
	if (!m_pWorldFOW)
		return;

	ModController *mc = static_cast<ModController *>(pEntity->GetController());
	if (mc != 0)
	{
		//
		SightExt *sight = static_cast<SightExt *>(mc->QIAll(SightExt::ExtensionID));

		if (sight != 0 && sight->GetInFOW())
		{
			// unburn previous position
			FOWChannelMask channelMask = CreateFOWMask(FOWC_Visible);
			FillCircle(sight->GetLastFOWXPos(), sight->GetLastFOWZPos(), sight->GetLastFOWRad(), channelMask, false);

			sight->SetInFOW(false);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::AddTaggedEntity(Entity *pEntity)
{
	dbAssert(pEntity);
	dbAssert(QIExt<SightExt>(pEntity) != NULL);

	TaggedEntityRecord record;

	record.m_bMarked = false;
	record.m_X = size_t(-1);
	record.m_Z = size_t(-1);
	record.m_prevRad = 0.0f;
	record.m_TaggedEntity.push_back(pEntity);

	m_TaggedEntities.push_back(record);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::RemTaggedEntity(Entity *pEntity)
{
	dbAssert(pEntity);

	TaggedEntityList::iterator iter = m_TaggedEntities.begin();
	TaggedEntityList::iterator eiter = m_TaggedEntities.end();

	for (; iter != eiter; ++iter)
	{
		if (iter->m_TaggedEntity.front() == pEntity)
		{
			// the entity will get removed from the FOW in the next update loop
			iter->m_TaggedEntity.clear();
			break;
		}
	}

	dbAssert(iter != m_TaggedEntities.end());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t PlayerFOW::GetWidth() const
{
	return m_pWorldFOW->GetWidth();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t PlayerFOW::GetHeight() const
{
	return m_pWorldFOW->GetHeight();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float PlayerFOW::GetCellSize() const
{
	return m_pWorldFOW->GetCellSize();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOWID PlayerFOW::GetPlayerFOWID() const
{
	return m_playerFOWID;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOWID PlayerFOW::GetPlayerSharedFOWID() const
{
	return m_FOWQuerryID;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::SetShareVisionWith(const PlayerFOW *shareFOW, bool bShare)
{
	// make or break the sharing bond
	m_pWorldFOW->CreateSharedPlayerFOWID(m_FOWQuerryID, shareFOW->GetPlayerFOWID(), bShare);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::RefreshWorldFOW()
{
	CellRefCount::iterator iter = m_CellRefCount.begin();
	CellRefCount::iterator eiter = m_CellRefCount.end();

	size_t cellnum = 0;
	while (iter != eiter)
	{
		if (*iter > 0)
		{
			m_pWorldFOW->SetCell(GetPlayerFOWID(), cellnum, CreateFOWMask(FOWC_Visible));
		}
		else
		{
			m_pWorldFOW->ClearCell(GetPlayerFOWID(), cellnum, CreateFOWMask(FOWC_Visible));
		}

		++cellnum;
		++iter;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::ShutdownFOW()
{
	m_pWorldFOW = NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::UpdateTaggedEntities()
{
	TaggedEntityList::iterator iter = m_TaggedEntities.begin();

	while (iter != m_TaggedEntities.end())
	{
		if (iter->m_TaggedEntity.front() != NULL)
		{
			TaggedEntityRecord *pRecord = iter;

			//
			SightExt *sight = QIExt<SightExt>(pRecord->m_TaggedEntity.front());

			if (sight != 0)
			{
				const float radius = sight->GetSightRadius();

				size_t cX, cZ;
				m_pWorldFOW->RemapPos2Cell(pRecord->m_TaggedEntity.front()->GetPosition().x, pRecord->m_TaggedEntity.front()->GetPosition().z, cX, cZ);

				FOWChannelMask channelMask = 0;

				if (!pRecord->m_bMarked)
				{
					channelMask = CreateFOWMask(FOWC_Visible);
				}
				else if (pRecord->m_X != cX || pRecord->m_Z != cZ || pRecord->m_prevRad != radius)
				{
					// unburn previous position
					channelMask = CreateFOWMask(FOWC_Visible);
					FillCircle(pRecord->m_X, pRecord->m_Z, pRecord->m_prevRad, channelMask, false);
				}

				if (channelMask != 0)
				{
					FillCircle(cX, cZ, radius, channelMask, true);

					pRecord->m_bMarked = true;
					pRecord->m_X = cX;
					pRecord->m_Z = cZ;
					pRecord->m_prevRad = radius;
				}
			}

			++iter;
		}
		else
		{
			if (iter->m_bMarked)
			{
				FillCircle(iter->m_X, iter->m_Z, iter->m_prevRad, CreateFOWMask(FOWC_Visible), false);
			}

			// remove the record
			iter = m_TaggedEntities.erase(iter);
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void PlayerFOW::PruneRevealList(RevealEntityList &revealList, const float currentTime)
{
	RevealEntityList::iterator ei = revealList.begin();
	for (; ei != revealList.end();)
	{
		if (ei->m_endTime < currentTime)
		{
			ei = std::vector_eraseback(revealList, ei);
		}
		else
		{
			++ei;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::UpdateRevealList()
{
	float currentTime = ModObj::i()->GetWorld()->GetGameTime();

	// reveal additional areas
	RevealAreaList::iterator ai = m_revealAreaList.begin();
	RevealAreaList::iterator ae = m_revealAreaList.end();
	for (; ai != ae;)
	{
		RevealAreaRecord &record = *ai;

		FOWChannelMask channelMask = 0;

		if (!record.m_bMarked && TestFOWMask(record.m_mask, FOWC_Visible))
		{
			channelMask = CreateFOWMask(FOWC_Visible);
			record.m_bMarked = true;
		}

		if (channelMask != 0)
			FillCircle(record.m_x, record.m_z, record.m_radius, channelMask, true);

		// remove expired areas
		if (record.m_endTime < currentTime)
		{
			// remove from FOW
			if (record.m_bMarked)
			{
				FillCircle(record.m_x, record.m_z, record.m_radius, CreateFOWMask(FOWC_Visible), false);
				record.m_bMarked = false;
			}

			ai = m_revealAreaList.erase(ai);
		}
		else
		{
			ai++;
		}
	}

	// prune done entities
	PruneRevealList(m_revealEntityList, currentTime);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::SetSize(size_t width, size_t height)
{
	m_CellRefCount.resize(width * height);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::UpdateEntity(Entity *pEntity)
{
	if (!pEntity->GetEntityFlag(EF_IsVisible))
		return;

	ModController *mc = static_cast<ModController *>(pEntity->GetController());
	if (mc != 0)
	{
		//
		SightExt *sight = QIExt<SightExt>(mc);

		if (sight != 0)
		{
			const float radius = sight->GetSightRadius();

			size_t cX, cZ;
			m_pWorldFOW->RemapPos2Cell(pEntity->GetPosition().x, pEntity->GetPosition().z, cX, cZ);

			FOWChannelMask channelMask = 0;

			if (!sight->GetInFOW())
			{
				channelMask = CreateFOWMask(FOWC_Visible);
			}
			else if (!sight->SameAsLastFOWPos(cX, cZ) || sight->GetLastFOWRad() != radius)
			{
				// unburn previous position
				channelMask = CreateFOWMask(FOWC_Visible);
				FillCircle(sight->GetLastFOWXPos(), sight->GetLastFOWZPos(), sight->GetLastFOWRad(), channelMask, false);
			}

			if (channelMask != 0)
			{
				FillCircle(cX, cZ, radius, channelMask, true);

				sight->SetLastFOWPos(cX, cZ);
				sight->SetLastFOWRad(radius);
				sight->SetInFOW(true);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::FillCircle(float x, float y, float radius, FOWChannelMask mask, bool Set)
{
	size_t cX, cZ;
	m_pWorldFOW->RemapPos2Cell(x, y, cX, cZ);

	FillCircle(cX, cZ, radius, mask, Set);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::FillCircle(size_t cX, size_t cZ, float radius, FOWChannelMask mask, bool Set)
{
	dbAssert(mask != 0);

#if !defined(RELIC_RTM)
	// crc it for sync checking
	m_crc.AddVar(cX);
	m_crc.AddVar(cZ);
	m_crc.AddVar(radius);
	m_crc.AddVar(mask);
	m_crc.AddVar(Set);
#endif

	// this allows us to have creatures that don't affect the FOW
	if (radius == 0.0f)
		return;

	//	Bresenham's circle based around the given cell
	int r = int(radius / m_pWorldFOW->GetCellSize());
	dbAssert(r < 128);
	unsigned char width[0x100];
	int X = 0;
	int Z = r;
	float D = 3.f * 2.f - (radius / m_pWorldFOW->GetCellSize()); //	This could be fixed point math

	//	Mark the three start points of the circle
	width[r - Z] = unsigned char(X);
	width[r - X] = unsigned char(Z);
	width[r + Z] = unsigned char(X);

	while (Z > X)
	{
		if (D < 0)
		{
			D += 2.f * X + 3.f;
		}
		else
		{
			D += 2.f * (X - Z) + 5.f;
			--Z;
		}
		++X;

		width[r - Z] = unsigned char(X);
		width[r - X] = unsigned char(Z);
		width[r + X] = unsigned char(Z);
		width[r + Z] = unsigned char(X);
	}

	//	Iterator the widths filling the spans
	int i;

	//	Clip the top and bottom of the circle
	Z = cZ - r;
	int H = r * 2 + 1;
	int start = 0;
	if (Z < 0)
	{
		start = -Z;
		Z = 0;
	}
	if (static_cast<unsigned long>(Z + H) >= m_pWorldFOW->GetHeight())
	{
		H = m_pWorldFOW->GetHeight() - Z;
	}

	//	Fill this clipped circle for this channel
	for (i = start; i < H; ++i)
	{
		//	Left-right clip circle
		X = cX - int(width[i]);
		int W = (width[i] * 2) + 1;
		if (X < 0)
		{
			W += X;
			X = 0;
		}
		if (static_cast<unsigned long>(X + W) >= m_pWorldFOW->GetWidth())
		{
			W = m_pWorldFOW->GetWidth() - X;
		}

		FillSpan(X, Z, W, mask, Set);
		++Z;
	}
}

namespace
{
	class IncVisibleExplored
	{
	public:
		__forceinline void ProcessCell(size_t cellnum, std::vector<unsigned char> &cells, const PlayerFOWID &fowID, WorldFOW *pWorldFOW)
		{
			dbAssert(cells[cellnum] < 255);

			if (cells[cellnum] == 0)
			{
				pWorldFOW->SetCell(fowID, cellnum, FOWC_Explored | FOWC_Visible);
			}

			cells[cellnum]++;
		}
	};

	class DecVisible
	{
	public:
		__forceinline void ProcessCell(size_t cellnum, std::vector<unsigned char> &cells, const PlayerFOWID &fowID, WorldFOW *pWorldFOW)
		{
			dbAssert(cells[cellnum] != 0);

			if (cells[cellnum] == 1)
			{
				pWorldFOW->ClearCell(fowID, cellnum, FOWC_Visible);
			}

			cells[cellnum]--;
		}
	};

	template <class T>
	void DoFillSpan(size_t cellnum, size_t cellend, std::vector<unsigned char> &cells, const PlayerFOWID &fowID, WorldFOW *pWorldFOW, T pred)
	{
		while (cellnum < cellend)
		{
			pred.ProcessCell(cellnum, cells, fowID, pWorldFOW);

			++cellnum;
		}
	}

} // namespace

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::FillSpan(size_t x, size_t y, size_t width, FOWChannelMask mask, bool Set)
{
	size_t cellnum = x + y * GetWidth();
	size_t cellend = cellnum + width; // one past the last cell

	if (!TestFOWMask(mask, FOWC_Visible))
	{
		// nothing to do
		return;
	}

	if (Set)
	{
		if (TestFOWMask(mask, FOWC_Visible))
		{
			DoFillSpan(cellnum, cellend, m_CellRefCount, m_playerFOWID, m_pWorldFOW, IncVisibleExplored());
		}
		else
		{
			dbBreak();
		}
	}
	else
	{
		if (TestFOWMask(mask, FOWC_Visible))
		{
			DoFillSpan(cellnum, cellend, m_CellRefCount, m_playerFOWID, m_pWorldFOW, DecVisible());
		}
		else
		{
			dbBreak();
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::Save(IFF &iff) const
{
	iff.PushChunk(Type_NormalVers, 'SFOW', 1);

	IFFWrite(iff, m_bRadarPulseEnabled);

	IFFWrite(iff, m_revealAreaList.size());
	RevealAreaList::const_iterator iter = m_revealAreaList.begin();
	RevealAreaList::const_iterator eiter = m_revealAreaList.end();
	for (; iter != eiter; ++iter)
	{
		IFFWrite(iff, iter->m_x);
		IFFWrite(iff, iter->m_z);
		IFFWrite(iff, iter->m_radius);
		IFFWrite(iff, iter->m_endTime);
		IFFWrite(iff, iter->m_mask);
	}

	unsigned long temp = m_playerFOWID;
	IFFWrite(iff, temp);

	temp = m_FOWQuerryID;
	IFFWrite(iff, temp);

	// revealed entities
	{
		IFFWrite(iff, m_revealEntityList.size());
		RevealEntityList::const_iterator rei = m_revealEntityList.begin();
		RevealEntityList::const_iterator ree = m_revealEntityList.end();
		for (; rei != ree; rei++)
		{
			const RevealEntityRecord &reRec = *rei;

			reRec.m_entity.SaveEmbedded(iff);
			IFFWrite(iff, reRec.m_endTime);
		}
	}

	iff.PopChunk();

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void PlayerFOW::Load(IFF &iff)
{
	iff.AddParseHandler(HandleSFOW, Type_NormalVers, 'SFOW', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long PlayerFOW::HandleSFOW(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	PlayerFOW *pPlayerFOW = static_cast<PlayerFOW *>(pContext1);
	dbAssert(pPlayerFOW);

	IFFRead(iff, pPlayerFOW->m_bRadarPulseEnabled);

	size_t count;
	IFFRead(iff, count);

	for (size_t i = 0; i != count; ++i)
	{
		RevealAreaRecord record;

		IFFRead(iff, record.m_x);
		IFFRead(iff, record.m_z);
		IFFRead(iff, record.m_radius);
		IFFRead(iff, record.m_endTime);
		IFFRead(iff, record.m_mask);
		record.m_bMarked = false;

		pPlayerFOW->m_revealAreaList.push_back(record);
	}

	// NOTE: at this point the playerFOWID can be loaded as a different value as
	// the ID's are first assigned in player creation order, if a player slot is closed out
	// then the creation order in a savegame will be different.
	unsigned long temp;
	IFFRead(iff, temp);
	pPlayerFOW->m_playerFOWID = temp;

	IFFRead(iff, temp);
	pPlayerFOW->m_FOWQuerryID = temp;

	// revealed entities
	pPlayerFOW->m_revealEntityList.clear();

	const EntityFactory *pEFac = ModObj::i()->GetEntityFactory();

	size_t numRevealed;
	IFFRead(iff, numRevealed);

	RevealEntityRecord reRec;
	for (size_t i = 0; i < numRevealed; i++)
	{
		reRec.m_entity.LoadEmbedded(iff, pEFac);
		IFFRead(iff, reRec.m_endTime);
		pPlayerFOW->m_revealEntityList.push_back(reRec);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long PlayerFOW::GetSyncToken() const
{
#if !defined(RELIC_RTM)
	return m_crc.GetCRC();
#else
	return 0;
#endif
}
