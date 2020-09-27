/////////////////////////////////////////////////////////////////////
// File    : RDNWorld.h
// Desc    : 
// Created : Tuesday, March 06, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <SimEngine/SimWorld.h>

///////////////////////////////////////////////////////////////////// 
//	This constant is the number of sim ticks that occur in 1 second
//	It can be used to convert Value/sec to Value/tick etc.
const float k_SimStepsPerSecond = 8;

///////////////////////////////////////////////////////////////////// 
// Forward Declarations

class Hull2d;
class RDNPlayer;
class WorldFOW;
class ImpassEditArray;

///////////////////////////////////////////////////////////////////// 
// RDNWorld 

class RDNWorld : public SimWorld
{
// types
public:

	struct CantPlaceInfo
	{
		enum PosPlaceInfo
		{
			PPI_CanPlace,
			PPI_CantPlace,
			PPI_CantPlaceFOW,
		};

		Vec3f			pos;
		unsigned char	placerestriction;
	};

	typedef std::smallvector< CantPlaceInfo, 32 > CantPlaceList;

// construction
private:
	RDNWorld( SimEngineInterface *pSimInt, bool bMissionEd );

public:
	virtual ~RDNWorld();

	static RDNWorld*	GetWorld();
	static void			CreateWorld( SimEngineInterface *pSimInt, bool bMissionEd );

// inherited -- World
public:
	virtual void Simulate();
	virtual void SimulatePre();
	virtual void SimulatePost();

	virtual void SetTerrain( TerrainHMBase* pTerrain );

	virtual bool IsEntityVisible( const Player* localPlayer, const Entity* entity ) const;

	virtual void GetSyncToken( std::vector< std::pair< unsigned long, const char* > >& crcArray ) const;

	virtual void DoSpawnEntity(Entity *e);

	virtual void DeSpawnEntity(Entity *e);

	virtual void DoRestoreEntity(Entity *e);

	virtual void PreLoadEBPS( );

	virtual bool CBPHasReference( long CBPNetID ) const;

	virtual void SetGameOver();

	virtual bool QueryRelationship( const Player* p1, const Player* p2, PlayerRelationship rel ) const;

	virtual int	 CalculatePathfindingSize( float xscale, float zscale ) const;

	virtual void SyncLogEntity( LogFile& logfile, const Entity *pEntity ) const;

// inherited -- World
// ME Interface
public:

	// Caled when you want dbWarning messages generated on invalid world state (used for the MissionEditor warnings Dialog)
	virtual void				GenerateWarnings( ) const;

// Save and Load functions.
public:
	virtual void Save( BiFF& biff ) const;
	virtual void Load( IFF& iff );

// 
public:

	void				ClampPointToWorld( Vec3f& point ) const;

	RDNPlayer*			CreateNewPlayer( );

	void				EnableFowUpdate( bool enable );
	const WorldFOW*		GetWorldFOW( ) const;
	
	size_t				GetPlayerSlotCount() const;
	const Vec3f&		GetPlayerSlotAt( size_t ) const;

	void				CumulateStateTimeBegin();
	void				CumulateStateTimeEnd();

	void				SaveStaticData( IFF&, const ImpassEditArray* );
	void				LoadStaticData( IFF& );
	
	const EntityGroup&	GetCashPileList() const;

	long				GetPlayerIDWon() const;

// Chunk Handlers
private:
	static unsigned long HandleMWLD( IFF&, ChunkNode*, void*, void* );

	static unsigned long HandleMSTC( IFF&, ChunkNode*, void*, void* );

	static unsigned long HandleSLCM( IFF&, ChunkNode*, void*, void* );

// fields
private:
	class Data;
	Data* m_pimpl;

// implementation
private:

	friend class GroupCommandProcessor;

	void PreRemoveEmptyPlayers();

	void PreSetPlayerInfo();
	
	void PreloadEBPs();

	void UpdateFoW();

	void SpawnEntityInternal( Entity *e );
};
