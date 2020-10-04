/////////////////////////////////////////////////////////////////////
// File    : WorldFOW.h
// Desc    :
// Created : Tuesday, January 08, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include <Assist/Array2d.h>
#include "FOWTypes.h"

#include <Math/FastMath.h>

class IFF;
class ChunkNode;

/////////////////////////////////////////////////////////////////////
//
class WorldFOW
{
public:
	WorldFOW(float islandwidth, float islandheight);
	~WorldFOW();

	// Interface
public:
	// Player FOW ID
	PlayerFOWID GetNextPlayerFOWID();

	// Map dimension info
	// width, height in Cell dimensions
	inline unsigned long GetWidth() const;
	inline unsigned long GetHeight() const;
	// CellSize in meters
	inline float GetCellSize() const;

	// Cell Status access
	inline bool GetCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel) const;
	inline bool GetCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel) const;

	inline void SetCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel);
	inline void SetCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel);

	inline void ClearCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel);
	inline void ClearCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel);

	inline const Array2D<unsigned long> &
	GetCellData() const;

	inline unsigned long CreateCellMask(const PlayerFOWID &playerID, FOWChannelMask channel) const;

	inline void RemapPos2Cell(float x, float y, size_t &cx, size_t &cz) const;

	void CreateSharedPlayerFOWID(PlayerFOWID &dest, const PlayerFOWID &share, bool bShare) const;

	void RevealAll();

	void Save(IFF &iff) const;
	void Load(IFF &iff);

	// Data
private:
	unsigned char m_nextPlayer;

	Array2D<unsigned long>
			m_FowStatus;

	// cached info
	float m_widthBy2;
	float m_heightBy2;

	// Chunk Handlers
private:
	static unsigned long HandleWFOW(IFF &, ChunkNode *, void *, void *);

	// copy -- do not define
private:
	WorldFOW(const WorldFOW &);
	WorldFOW &operator=(const WorldFOW &);
};

/////////////////////////////////////////////////////////////////////
// Inline functions

inline unsigned long WorldFOW::CreateCellMask(const PlayerFOWID &playerID, FOWChannelMask channel) const
{
	unsigned long longmask;

	// replicate the bits from the lowest nible all the way up
	longmask = channel;
	longmask |= longmask << 4;
	longmask |= longmask << 8;
	longmask |= longmask << 16;

	longmask &= playerID;

	return longmask;
}

// must come before it's usage in functions below
inline void WorldFOW::RemapPos2Cell(float x, float z, size_t &cX, size_t &cZ) const
{
	// NOTE: entities can move outside the world which means we have to do this range check shit
	// the plus minus 1.0f is to make sure the result is less than m_width*2.0f
	if (x > m_widthBy2)
		x = m_widthBy2 - 1.0f;

	if (x < -m_widthBy2)
		x = -m_widthBy2 + 1.0f;

	if (z > m_heightBy2)
		z = m_heightBy2 - 1.0f;

	if (z < -m_heightBy2)
		z = -m_heightBy2 + 1.0f;

	x = (x + m_widthBy2) / GetCellSize();
	z = (z + m_heightBy2) / GetCellSize();

	int cellX;
	_FTOL_POS(x, cellX);
	cX = cellX;

	int cellZ;
	_FTOL_POS(z, cellZ);
	cZ = cellZ;

	dbAssert(cX < GetWidth());
	dbAssert(cZ < GetHeight());
}

// Map dimension info
// width, height in Cell dimensions
inline unsigned long WorldFOW::GetWidth() const
{
	return m_FowStatus.GetWidth();
}

inline unsigned long WorldFOW::GetHeight() const
{
	return m_FowStatus.GetHeight();
}

// CellSize in meters
inline float WorldFOW::GetCellSize() const
{
	return 4.0f;
}

// Cell Status access
__forceinline bool WorldFOW::GetCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel) const
{
	size_t cX, cZ;
	RemapPos2Cell(x, z, cX, cZ);

	unsigned long mask = CreateCellMask(playerID, channel);

	return (m_FowStatus.GetValue(cX, cZ) & mask) != 0;
}

__forceinline bool WorldFOW::GetCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel) const
{
	dbAssert(cellnum < m_FowStatus.GetSize());

	unsigned long mask = CreateCellMask(playerID, channel);

	return (*(m_FowStatus.GetData() + cellnum) & mask) != 0;
}

inline void WorldFOW::SetCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel)
{
	size_t cX, cZ;
	RemapPos2Cell(x, z, cX, cZ);

	unsigned long mask = CreateCellMask(playerID, channel);

	m_FowStatus.GetValue(cX, cZ) |= mask;
}

inline void WorldFOW::SetCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel)
{
	dbAssert(cellnum < m_FowStatus.GetSize());

	unsigned long mask = CreateCellMask(playerID, channel);

	*(m_FowStatus.GetData() + cellnum) |= mask;
}

inline void WorldFOW::ClearCell(const PlayerFOWID &playerID, float x, float z, FOWChannelMask channel)
{
	size_t cX, cZ;
	RemapPos2Cell(x, z, cX, cZ);

	unsigned long mask = CreateCellMask(playerID, channel);

	m_FowStatus.GetValue(cX, cZ) &= ~mask;
}

inline void WorldFOW::ClearCell(const PlayerFOWID &playerID, size_t cellnum, FOWChannelMask channel)
{
	dbAssert(cellnum < m_FowStatus.GetSize());

	unsigned long mask = CreateCellMask(playerID, channel);

	*(m_FowStatus.GetData() + cellnum) &= ~mask;
}

inline const Array2D<unsigned long> &WorldFOW::GetCellData() const
{
	dbAssert(m_FowStatus.GetSize() != 0);

	return m_FowStatus;
}