/////////////////////////////////////////////////////////////////////
// File    : PlayerFOW.h
// Desc    : 
// Created : Tuesday, January 08, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include "FOWTypes.h"

#include <SimEngine/EntityGroup.h>

#include <Filesystem/Crc.h>

class Entity;
class IFF;
class ChunkNode;
class WorldFOW;
class ControllerBlueprint;

///////////////////////////////////////////////////////////////////// 
// 
class PlayerFOW
{
public:
	 PlayerFOW( WorldFOW* );
	~PlayerFOW();

// interface
public:

	void			Update				( const EntityGroup& playerEntities );
					
	bool			IsVisible			( const Entity* ) const;
	bool			IsVisible			( const ControllerBlueprint*, const Matrix43f& ) const;
	bool			IsVisible			( const Vec3f&  ) const;
	bool			IsExplored			( const Entity* ) const;
					
	void			RevealArea			( float x, float z, float radius, float duration, FOWChannelMask channel );
	void			UnRevealArea		( float x, float z, float radius, FOWChannelMask channel  );

	void			RevealEntity		( const Entity*, float duration );
					
	void			EnableRadarPulse	( bool bEnable );
					
	long			GetExploredPercent	( ) const;

	void			RemoveFromFOW		( Entity* );

	//
	void			AddTaggedEntity		( Entity* );
	void			RemTaggedEntity		( Entity* );

	// Map dimension info
	// width, height in Cell dimensions
	size_t			GetWidth( ) const;
	size_t			GetHeight( ) const;

	// CellSize in meters
	float			GetCellSize( ) const;

	PlayerFOWID		GetPlayerFOWID( ) const;
	PlayerFOWID		GetPlayerSharedFOWID( ) const;

	//
	unsigned long	GetSyncToken() const;
	
	// vision share
	void			SetShareVisionWith( const PlayerFOW*, bool bShare );

	void			RefreshWorldFOW( );

	void			ShutdownFOW();
	
	// Save Load Code
	void			Save( IFF& iff ) const;
	void			Load( IFF& iff );

// types
private:
	struct RevealAreaRecord
	{
		float			m_x;
		float			m_z;
		float			m_radius;
		float			m_endTime;
		bool			m_bMarked;
		FOWChannelMask 	m_mask;
	};
	typedef std::list<RevealAreaRecord> RevealAreaList;

	struct RevealEntityRecord
	{
		// Note because this expires we can use an Entity* we don't access this entity's pointer
		// so if it dies on use it doesn't matter
		EntityGroup		m_entity;
		float			m_endTime;
	};
	typedef std::vector<RevealEntityRecord> RevealEntityList;

	struct TaggedEntityRecord
	{
		float			m_prevRad;
		size_t			m_X, m_Z;
		bool			m_bMarked;
		EntityGroup		m_TaggedEntity;
	};
	typedef std::smallvector< TaggedEntityRecord, 10 > TaggedEntityList;

	typedef std::vector< unsigned char > CellRefCount;

// fields
private:

	RevealAreaList		m_revealAreaList;
	RevealEntityList	m_revealEntityList;
	bool				m_bRadarPulseEnabled;

	PlayerFOWID			m_playerFOWID;
	PlayerFOWID			m_FOWQuerryID;

	WorldFOW*			m_pWorldFOW;	// weak

	CellRefCount		m_CellRefCount;

	bool				m_bAllRevealed;

	TaggedEntityList	m_TaggedEntities;

	CRC					m_crc;

// Implementation
private:

	void	UpdateTaggedEntities( );
	void	UpdateRevealList( );

	void	SetSize( size_t width, size_t height );

	void	UpdateEntity( Entity* pEntity );

	void	FillCircle( float x, float y, float radius, FOWChannelMask mask, bool Set );

	void	FillCircle( size_t cX, size_t cY, float radius, FOWChannelMask mask, bool Set );

	void	FillSpan( size_t x, size_t y, size_t width, FOWChannelMask mask, bool Set );

	void	PruneRevealList( RevealEntityList& list, const float currentTime );

	// Chunk Handlers for the loading code
	static unsigned long HandleSFOW( IFF&, ChunkNode*, void*, void* );

// copy -- do not define
private:
	PlayerFOW( const PlayerFOW& );
	PlayerFOW& operator= ( const PlayerFOW& );
};

//	eof
