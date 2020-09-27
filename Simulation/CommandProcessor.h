/////////////////////////////////////////////////////////////////////
// File    : CommandProcessor.h
// Desc    : 
// Created : Monday, January 28, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//
#pragma once

#include "States/State.h"

//#include "TagTypes.h"
#include "AttackTypes.h"

///////////////////////////////////////////////////////////////////// 
// Forward Declarations

class ModController;
class EntityCommand;
class Entity;
class EntityGroup;

///////////////////////////////////////////////////////////////////// 
// CommandProcessor

class CommandProcessor
{
// construction
public:
	CommandProcessor();

	~CommandProcessor();

	void Init( ModController* pMC );

// interface
public:

	inline bool				IsDead() const;

	void					MakeDead();

	// call these functions to setup the default state
	void					OnSpawnEntity();
	void					OnDeSpawnEntity();

	// commands issued to the entity controllers.
	bool					CommandDoProcessNow( const EntityCommand* );
	bool					CommandIsClearQueue( const EntityCommand* ) const;

	bool					Update ( const EntityCommand* currentCommand );

// Save and Load functions
public:

	void					Save( IFF& iff ) const;

	void					Load( IFF& iff );

public:

	//
	static unsigned long			GetDefaultEntityEntityCommand( const Entity* pMe, const Entity* pTe );

	//
	static bool						CanDoCommand( const Entity* pMe, unsigned long command, unsigned long param );
	static bool						CanDoCommand( const Entity* pMe, const Entity* pTarget, unsigned long command, unsigned long param );

// fields
private:

	bool					m_bIsDead;

	ModController*			m_pMC;

// Loader
private:

	// Chunk Handlers for the Save Game code
	static unsigned long			HandleCMDP( IFF&, ChunkNode*, void*, void* );

// implementation
private:

	bool							HasState( unsigned char StateID );
	
	void							SelfDestruct( );

	bool							ProcessQueuedCommandNow ( const EntityCommand* );
	
	bool							EatUpdateCommand( const EntityCommand* );

	bool							ValidateCommand( const EntityCommand* pEntCmd );

	//
	void							ToStateIdle				( );
	void							ToStateMove				( const Vec3f&, const EntityGroup&, const EntityGroup&, unsigned long flags = 0 );
	void							ToStateMove				( Entity*, unsigned long flags = 0 );
	void							ToStateAttack			( Entity* );
	void							ToStateAttackMove		( const Vec3f& );
	void							ToStateAttackMove		( Entity* );
	void							ToStatePause			( bool bIgnoreCmds );

	void							LeaveStatePause			( );
};

inline bool CommandProcessor::IsDead() const
{
	return m_bIsDead;
}