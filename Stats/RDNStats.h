/////////////////////////////////////////////////////////////////////
// File    : RDNStats.h
// Desc    : 
// Created : Thursday, April 04, 2003
// Author  : 
// 
// (c) 2003 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/DllInterface.h>

#include "../Simulation/RDNWorld.h"
#include "../Simulation/GameEventSys.h" 

// forward declaration
class IFF;

/////////////////////////////////////////////////////////////////////
// RDNStats

// logs a bunch of stats about the current simulation
// these stats are used for the postgame screen
// this class is necessary because the RDNWorld object dies 
//   when the simulation ends, before the stats are displayed.

class RDNStats : 
	private GameEventSys::Listener
{
// singleton
private:
	 RDNStats();
	~RDNStats();

public:
	static RDNStats* Instance();

	static void Initialize();
	static void Shutdown  ();

// fields
public:
	
// inherited - GameEventSys::Listener
public:
	virtual void OnEvent( const GameEventSys::Event& );

// interface 
public:
	// recording 
	void RecordStart( const RDNWorld* pWorld );
	void RecordFrame( const RDNWorld* pWorld );
	void RecordStop( const RDNWorld* pWorld );
	void Reset();
	
	// io
	void Save( IFF& iff ) const;
	void Load( IFF& iff );


	// game stats
	long			TotalDuration() const;
	
	// player stats
	DLLScoreInterface::PlayerState 
					PlayerFinalState ( unsigned long idplayer ) const;
	const wchar_t*	PlayerName( unsigned long idplayer ) const;
	
// fields
private:
	class Data;
	Data* m_pimpl;

// implementation
private:
	void RecordInit( const RDNWorld* pWorld );

	static unsigned long HandleSLOG( IFF& iff, ChunkNode* node, void* ctx1, void* ctx2 );
	static unsigned long HandleSLGS( IFF& iff, ChunkNode* node, void* ctx1, void* ctx2 );
	static unsigned long HandleSLPS( IFF& iff, ChunkNode* node, void* ctx1, void* ctx2 );

// non-copyable
private:
	RDNStats( const RDNStats& );
	RDNStats& operator= ( const RDNStats& );	
};