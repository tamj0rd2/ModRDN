/////////////////////////////////////////////////////////////////////
// File    : GameEventDefs.h
// Desc    : 
// Created : Friday June 01, 2001
// Author  : Shelby
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "GameEventSys.h"

#include "AttackTypes.h"

#include <EngineAPI/ControllerBlueprint.h>

#include <SimEngine/EntityGroup.h>

///////////////////////////////////////////////////////////////////// 
// 

// Enum of all event ids. Could use dynamic cast instead of these types 
// -more type safe and less prone to human error- but I have never used it 
// and am not familiar with its performance or how to debug it.

//------------------------------------------
// Definitions of the event names below.
// 
//   __Command: this event is triggered when a command is received on the opposite side of the network before
//				it has been processed. Sometimes the processing prevents a command from being 'start'ed.
//   __Start:   The said command has begun to be processed.
//   __Complete:The said command has finished processing.
//   __Cancel:  The said command has been canceled.
//
//------------------------------------------

enum {
	GE_BuildUnitCommand,
	GE_BuildUnitStart,
	GE_BuildUnitComplete,
	GE_BuildUnitCancel,

	GE_HenchmanEntityEntityCmd,
	GE_HenchmanEntityEntityCancel,
	GE_HenchmanEntityPointCmd,
	GE_HenchmanEntityPointCancel,
	GE_HenchmanEntityCmd,
	GE_HenchmanIdle,
	GE_HenchmanCantReachBuilding,

	GE_ResourceGatherDepleted,

	GE_EntityKilled,

	GE_EntityDigStateChange,

	GE_EntityToBeGarrisoned,
	GE_EntityUngarrisoned,

	GE_EBPOverride,

	GE_RallyPointSet,

	GE_Garrison,

	GE_PlayerBeingAttacked,
	GE_PlayerKilled,
	GE_PlayerCheat,

	GE_PlayerAllianceBegin,
	GE_PlayerAllianceEnd,
	GE_PlayerDonation,
	GE_PlayerNameChanged,

	GE_PlayerDropped,
	GE_PlayerHostMigrated,

	GE_UIHenchmanBuild,
	GE_UIStartBuildUnit,
	GE_UIObjectiveDlgShown,

	GE_GameStart,
	GE_GameOver,

	GE_AttackBlocked,

	GE_BuildUnitBlocked,
};

/////////////////////////////////////////////////////////////////////
// GameEvent_BuildUnitCommand
//

class GameEvent_BuildUnitCommand : public GameEventSys::Event
{
public:
	GameEvent_BuildUnitCommand( const RDNPlayer* p, const Entity* e, const ControllerBlueprint* pCBP ) : 
		GameEventSys::Event(GE_BuildUnitCommand, p),
		m_pEntity(e), m_pCBP(pCBP) {}

	const Entity*				m_pEntity;
	const ControllerBlueprint*	m_pCBP;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_BuildUnitStart
//

class GameEvent_BuildUnitStart : public GameEventSys::Event
{
public:
	GameEvent_BuildUnitStart( const RDNPlayer* p, const Entity* e, const ControllerBlueprint* pCBP ) : 
		GameEventSys::Event(GE_BuildUnitStart, p),
		m_pSpawner(e), m_pCBP(pCBP) {}

	const Entity*				m_pSpawner;
	const ControllerBlueprint*	m_pCBP;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_BuildUnitComplete
//

class GameEvent_BuildUnitComplete : public GameEventSys::Event
{
public:
	GameEvent_BuildUnitComplete( const RDNPlayer* p, const Entity* spawner, const Entity* e ) : 
		GameEventSys::Event(GE_BuildUnitComplete, p),
		m_pNewEntity(e), m_pSpawner(spawner) {}

	const Entity*				m_pSpawner;
	const Entity*				m_pNewEntity;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_BuildUnitCancel
//

class GameEvent_BuildUnitCancel : public GameEventSys::Event
{
public:
	GameEvent_BuildUnitCancel( const RDNPlayer* p, const Entity* spawner, const ControllerBlueprint* pCBP ) : 
		GameEventSys::Event(GE_BuildUnitCancel, p),
		m_pSpawner(spawner), m_pCBP(pCBP)  {}

	const Entity*				m_pSpawner;
	const ControllerBlueprint*	m_pCBP;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_BuildUnitBlocked
//

class GameEvent_BuildUnitBlocked : public GameEventSys::Event
{
public:
	GameEvent_BuildUnitBlocked( const RDNPlayer* p, const Entity* spawner ) : 
		GameEventSys::Event(GE_BuildUnitBlocked, p),
		m_pSpawner(spawner) {}

	const Entity*				m_pSpawner;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_HenchmanEntityEntityCmd
//

class GameEvent_HenchmanEntityEntityCmd : public GameEventSys::Event
{
public:
	GameEvent_HenchmanEntityEntityCmd( const RDNPlayer* p, const Entity* e, const Entity* target ) : 
		GameEventSys::Event(GE_HenchmanEntityEntityCmd, p),
		m_pEntity(e), m_pTarget(target) {}

	const Entity*					m_pEntity;
	const Entity*					m_pTarget;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_HenchmanEntityEntityCancel
//

//  This lets the listener know that a cmd of theirs has been
//  canceled. This happens when there is lag or the logic that sent
//  the cmd is different then the logic that processes the cmd.
//  We should make sure this logic is always similar.
// 

class GameEvent_HenchmanEntityEntityCancel : public GameEventSys::Event
{
public:
	GameEvent_HenchmanEntityEntityCancel( const RDNPlayer* p, const EntityGroup& entities) : 
		GameEventSys::Event(GE_HenchmanEntityEntityCancel, p),
		m_pERef( entities ) {}

	const EntityGroup&				m_pERef;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_HenchmanEntityEntityCmd
//

class GameEvent_HenchmanEntityPointCmd : public GameEventSys::Event
{
public:
	GameEvent_HenchmanEntityPointCmd( const RDNPlayer* p, const Entity* e ) : 
		GameEventSys::Event(GE_HenchmanEntityPointCmd, p),
		m_pEntity(e){}

	const Entity*					m_pEntity;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_HenchmanEntityPointCancel
//

//  This lets the listener know that a cmd of theirs has been
//  canceled. This happens when there is lag or the logic that sent
//  the cmd is different then the logic that processes the cmd.
//  We should make sure this logic is always similar.
// 

class GameEvent_HenchmanEntityPointCancel : public GameEventSys::Event
{
public:
	GameEvent_HenchmanEntityPointCancel( const RDNPlayer* p, const EntityGroup& entities) : 
		GameEventSys::Event(GE_HenchmanEntityPointCancel, p),
		m_pERef( entities ) {}

	const EntityGroup&				m_pERef;
};

class GameEvent_HenchmanEntityCmd : public GameEventSys::Event
{
public:
	GameEvent_HenchmanEntityCmd( const RDNPlayer* p, const Entity* e) : 
		GameEventSys::Event(GE_HenchmanEntityCmd, p),
		m_pEntity(e){}

	const Entity*					m_pEntity;
};


/////////////////////////////////////////////////////////////////////
// GameEvent_HenchmanCanReachBuilding
//
//  This lets the listerer know that a henchman did not make it to 
//  then building it had targeted cuz a) its not there anymore because
//  it was killed or canceled b) pathfinding error.
//

class GameEvent_HenchmanCantReachBuilding : public GameEventSys::Event
{
public:
	GameEvent_HenchmanCantReachBuilding( const RDNPlayer* p, const Entity* pHenchman, const Entity* pTarget) : 
	  GameEventSys::Event(GE_HenchmanCantReachBuilding, p),
	  m_pHenchman( pHenchman ), m_pTarget( pTarget ) {}

  	const Entity*					m_pHenchman;
	const Entity*					m_pTarget;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerBeingAttacked
//

class GameEvent_PlayerBeingAttacked: public GameEventSys::Event
{
public:
	GameEvent_PlayerBeingAttacked( const RDNPlayer* p, const Entity* attacker, const Entity* victim, float dmg, DamageType type = DT_None ) 
		: GameEventSys::Event(GE_PlayerBeingAttacked, p),
		  m_attacker( attacker ), m_victim( victim ), m_damage( dmg ), m_damagetype( type )
	{
		dbAssert( m_victim );
		dbAssert( m_damage >= 0.0f );
	}

public:
	const Entity*	m_attacker;
	const Entity*	m_victim;

	const float		m_damage;
	const DamageType m_damagetype;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_EntityKilled
//

class GameEvent_EntityKilled: public GameEventSys::Event
{
public:
	GameEvent_EntityKilled( const RDNPlayer* p, const Entity* victim, const Entity* killer ) 
		: GameEventSys::Event(GE_EntityKilled, p),
		  m_victim( victim ),
		  m_killer( killer )
	{
	}

public:
	const Entity*	m_victim;
	const Entity*	m_killer;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_EntityToBeGarrisoned
//

class GameEvent_EntityToBeGarrisoned: public GameEventSys::Event
{
public:
	GameEvent_EntityToBeGarrisoned( const Entity* entity ) 
		: GameEventSys::Event(GE_EntityToBeGarrisoned, 0),
		  m_entity( entity )
	{
	}

public:
	const Entity*	m_entity;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_EntityUngarrisoned
//

class GameEvent_EntityUngarrisoned: public GameEventSys::Event
{
public:
	GameEvent_EntityUngarrisoned( const Entity* entity ) 
		: GameEventSys::Event(GE_EntityUngarrisoned, 0),
		  m_entity( entity )
	{
	}

public:
	const Entity*	m_entity;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_ResourceGatherDepleted
//

class GameEvent_ResourceGatherDepleted : public GameEventSys::Event
{
public:
	GameEvent_ResourceGatherDepleted( const Entity* resource, const EntityGroup& gatherers ) 
		: GameEventSys::Event( GE_ResourceGatherDepleted, 0 ),
		  m_resource	( resource ),
		  m_gatherers	( gatherers )
	{
	}

public:
	const Entity*		m_resource;
	const EntityGroup&	m_gatherers;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_EntityDigStateChange
//

class GameEvent_EntityDigStateChange : public GameEventSys::Event
{
public:
	GameEvent_EntityDigStateChange( const RDNPlayer* p, const Entity* unit, bool toUnderground ) 
		: GameEventSys::Event( GE_EntityDigStateChange, p ),
		  m_pEntity( unit ), m_toUnderground( toUnderground )
	{
	}

public:
	const Entity*	m_pEntity;
	bool			m_toUnderground;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_EBPOverride
//

class GameEvent_EBPOverride : public GameEventSys::Event
{
public:
	GameEvent_EBPOverride( const RDNPlayer* p, int controllerType, bool locked ) 
		: GameEventSys::Event( GE_EBPOverride, p ),
		  m_controllerType( controllerType ),
		  m_locked( locked )
	{
	}

public:
	bool	m_locked;
	int		m_controllerType;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_RallyPointSet
//

class GameEvent_RallyPointSet : public GameEventSys::Event
{
public:
	GameEvent_RallyPointSet( const RDNPlayer* p, const Entity* pBuilding) 
		: GameEventSys::Event( GE_RallyPointSet, p ),
		  m_pBuilding( pBuilding )
	{
	}

public:
	const Entity*	m_pBuilding;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_Garrison
//

class GameEvent_Garrison : public GameEventSys::Event
{
public:
	GameEvent_Garrison( const RDNPlayer* p, const Entity* building ) 
		: GameEventSys::Event( GE_Garrison, p ),
		  m_pEntity( building )
	{
	}

public:
	const Entity*	m_pEntity;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerDropped
//

class GameEvent_PlayerDropped : public GameEventSys::Event
{
public:
	GameEvent_PlayerDropped( const RDNPlayer* dropped ) 
		: GameEventSys::Event( GE_PlayerDropped, 0 ),
		  m_dropped( dropped )
	{
	}

public:
	const RDNPlayer* m_dropped;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_HostMigrated
//

class GameEvent_HostMigrated : public GameEventSys::Event
{
public:
	GameEvent_HostMigrated( const RDNPlayer* migrated ) 
		: GameEventSys::Event( GE_PlayerHostMigrated, 0 ),
		  m_migrated( migrated )
	{
	}

public:
	const RDNPlayer* m_migrated;
};


///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerKilled
//

class GameEvent_PlayerKilled : public GameEventSys::Event
{
public:
	GameEvent_PlayerKilled( const RDNPlayer* killed, int reason ) 
		: GameEventSys::Event( GE_PlayerKilled, 0 ),
		  m_killed( killed ), m_reason( reason )
	{
	}

public:
	const RDNPlayer*	m_killed;
	const int			m_reason;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerCheat
//

class GameEvent_PlayerCheat : public GameEventSys::Event
{
public:
	GameEvent_PlayerCheat( const RDNPlayer* cheater ) 
		: GameEventSys::Event( GE_PlayerCheat, 0 ),
		  m_cheater( cheater )
	{
	}

public:
	const RDNPlayer* m_cheater;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerStartedAlliance
//

class GameEvent_PlayerStartedAlliance : public GameEventSys::Event
{
public:
	GameEvent_PlayerStartedAlliance( const RDNPlayer* p, const RDNPlayer* sender ) 
		: GameEventSys::Event( GE_PlayerAllianceBegin, p ),
		  m_pSender( sender )
	{
	}

public:
	const RDNPlayer*	m_pSender;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerEndedAlliance
//

class GameEvent_PlayerEndedAlliance : public GameEventSys::Event
{
public:
	GameEvent_PlayerEndedAlliance( const RDNPlayer* p, const RDNPlayer* sender, bool war ) 
		: GameEventSys::Event( GE_PlayerAllianceEnd, p ),
		  m_pSender( sender ), m_bWar( war )
	{
	}

public:
	const RDNPlayer*	m_pSender;
	bool				m_bWar;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerReceivedDonation
//

class GameEvent_PlayerReceivedDonation : public GameEventSys::Event
{
public:
	GameEvent_PlayerReceivedDonation
		( 
		const RDNPlayer* p, 
		const RDNPlayer* sender, 
		float gather, 
		float renew 
		) 
		: GameEventSys::Event( GE_PlayerDonation, p ),
		  m_sender(sender),
		  m_gather(gather),
		  m_renew (renew)
	{
	}

public:
	const RDNPlayer*	m_sender;
	float				m_gather;
	float				m_renew;
};

///////////////////////////////////////////////////////////////////// 
// GameEvent_PlayerNameChanged
//

class GameEvent_PlayerNameChanged : public GameEventSys::Event
{
public:
	GameEvent_PlayerNameChanged( const RDNPlayer* p )
		: GameEventSys::Event( GE_PlayerNameChanged, p )
	{
	}
};

/////////////////////////////////////////////////////////////////////
// GameEvent_UIHenchmanBuild
//

//  This lets the listener know that the henchman-build button was 
//  pressed.

class GameEvent_UIHenchmanBuild : public GameEventSys::Event
{
public:
	GameEvent_UIHenchmanBuild() : 
	  GameEventSys::Event(GE_UIHenchmanBuild, 0) {}
};

/////////////////////////////////////////////////////////////////////
// GameEvent_UIStartBuildUnit
//

//  This lets the listener know that a build unit button was 
//  pressed.

class GameEvent_UIStartBuildUnit : public GameEventSys::Event
{
public:
	GameEvent_UIStartBuildUnit(long ebpid) : 
	  GameEventSys::Event(GE_UIStartBuildUnit, 0), m_ebpid( ebpid ) {}

public:
	long	m_ebpid;
};

/////////////////////////////////////////////////////////////////////
// GameEvent_UIObjectiveDlgShown
//

//  This lets the listener know that the objective dialog was shown.

class GameEvent_UIObjectiveDlgShown : public GameEventSys::Event
{
public:
	GameEvent_UIObjectiveDlgShown() : 
	  GameEventSys::Event(GE_UIObjectiveDlgShown, 0) {}
};

/////////////////////////////////////////////////////////////////////
// GameEvent_GameStart
//

//  This lets the listener know that the game just started

class GameEvent_GameStart : public GameEventSys::Event
{
public:
	GameEvent_GameStart() : 
	  GameEventSys::Event(GE_GameStart, 0) {}
};

/////////////////////////////////////////////////////////////////////
// GameEvent_GameOver
//

//  This lets the listener know that the game is over

class GameEvent_GameOver : public GameEventSys::Event
{
public:
	GameEvent_GameOver() : 
	  GameEventSys::Event(GE_GameOver, 0) {}
};


/////////////////////////////////////////////////////////////////////
// GameEvent_AttackBlocked
//

class GameEvent_AttackBlocked : public GameEventSys::Event
{
public:

	enum Blockage
	{
		BLOCK_Entity,
		BLOCK_Building,
		BLOCK_Terrain,
	};

	GameEvent_AttackBlocked( const RDNPlayer* p, const Entity* e, Blockage blockage ) :
		GameEventSys::Event( GE_AttackBlocked, p ), 
		m_pEntity(e),
		m_blockage(blockage) {}

	const Entity*				m_pEntity;
	Blockage					m_blockage;
};
