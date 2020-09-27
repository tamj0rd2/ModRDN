/////////////////////////////////////////////////////////////////////
// File    : UnitSpawnerExt.h
// Desc    : 
// Created : Tuesday, February 20, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h" 
#include "ExtensionTypes.h" 

#include <EngineAPI/EntityFactory.h>
#include "SimEngine/EntityGroup.h"


///////////////////////////////////////////////////////////////////// 
// Forward Declarations
class RDNPlayer;

///////////////////////////////////////////////////////////////////// 
// UnitSpawnerExt

// all buildings that spawns units support this extension

class UnitSpawnerExt : public Extension
{
// types
public:
	enum
	{
		ExtensionID = EXTID_UnitSpawner
	};

	enum
	{
		MAXQUEUELENGTH = 8
	};

	enum RallyType
	{
		RALLY_NoPoint,
		RALLY_AtPoint,
		RALLY_Structure,
		RALLY_Creature,
		RALLY_ResourceGather,
	};

	enum BuildType
	{
		BT_Spawn,
		BT_Construct,
	};


// interface
public:
	// returns the position where the next unit should be spawned
	virtual bool	UnitSpawnPos( const ControllerBlueprint* type, Vec3f& output ) const;

	// list of units that can be spawned at this building
	virtual void	UnitList ( ControllerBPList& ) const;
	
	// return true if specified type of unit can be built at this structure
	bool			UnitListFilter( unsigned long ebpid ) const;

	// default implementation check 'SpawnerExtInfo'
	virtual bool	UnitListFilter( const ControllerBlueprint* ) const;

	// size of build queue
	virtual size_t	BuildQueueSize() const;

	virtual const ControllerBlueprint* 
					BuildQueueAt  ( size_t index ) const;

	// call every simsteps
	virtual void	UnitBuild();

	// returns the type of unit currently being built, 
		// along with the current progress (0.0-1.0)
	virtual std::pair< const ControllerBlueprint*, float >
					UnitInProgress() const;

	// will this extension allow a unit spawn right 'NOW'
	bool			IsAvailable( ) const;

	// is the spawner blocked from spawning because the surrounding area is occupied?
	bool			IsBlocked( ) const;

	// set the rally point
		// follow friendly/enemy creature/structure
	void			SetRallyTarget( const Entity* pEntity );
		// go to specified point
	void			SetRallyTarget( const Vec3f& point );

	// methods to query the rally point
	RallyType		GetRallyType() const;										// returns the type of rally point, if any
	bool			GetRallyPosition( Vec3f& position ) const;					// get the position of the rally point, return false if no rally point
	const Entity*	GetRallyTarget() const;										// returns the rally target entity, if there is one

	// called when a new entity is built, to give it a move order
	virtual void	Rally( const EntityGroup& eg );

	virtual void	OnDead();

protected:
	// add specified unit to build queue
		// resources are spent immediately
	virtual bool	BuildQueueAdd ( const unsigned long ebpid );
	virtual bool	BuildQueueAdd ( const ControllerBlueprint* cbp );

	// remove specified unit from build queue
	virtual bool	BuildQueueRmv ( size_t index );

	// called when removing a unit from the construct build queue
	virtual void	OnUnitBuildQueueRmvConstruct( Entity* pEntity );

	// call to add the construction after its paid for.  Can call this directly, if the entity doesn't need to be paid for.
	virtual bool	AddConstruct( const ControllerBlueprint* cbp );

private:
	// called 
	virtual void	OnUnitSpawn( Entity* ) = 0;

	virtual void	OnUnitBuild( size_t tickcount );

	virtual void	OnUnitSpawnLocation( const Vec3f& pos );

	virtual void	OnUnitBuildByConstructing( Entity* pEntity, float percentage );

	virtual void	OnUnitDoneByConstructing( Entity* pEntity );

// inherited interface: Extension
private:

	virtual void SaveExt( BiFF& ) const;
	virtual void LoadExt( IFF& );

	static unsigned long HandleEUSP( IFF&, ChunkNode*, void*, void* );

// construction
protected:
	UnitSpawnerExt( BuildType buildtype );

// fields
private:

	typedef std::smallvector< const ControllerBlueprint*, MAXQUEUELENGTH > UnitQueueList;

	UnitQueueList
				m_unitQueue;

	const BuildType	m_buildType;

	size_t		m_unitInProgressTicks;
	size_t		m_unitInProgressCount;

	EntityGroup	m_unitConstructing;

	RallyType	m_rallyType;						// the type of rally

	float		m_rallyTheta;
	Vec3f		m_rallyPoint;						// the point to rally to if !m_bRallyToEntity
	EntityGroup m_rallyEntity;						// will contain the entity to rally to
	Vec3f		m_rallyLastSeen;					// the last point the enemy creature was visible at

	bool		m_bWaitingForSpace;
	long		m_waitFinishTime;

// implementation
private:

	bool		BuildPay( const ControllerBlueprint* cbp );
	bool		BuildRefund( RDNPlayer* pPlayer, const ControllerBlueprint* cbp );

	RallyType	ClassifyRallyEntity( const Entity* pEntity );

	bool		IsEntityVisible(const Entity *pEntity) const;

	// rallying at different entity types, called by Rally.  
	//	These could be made virtual if we ever want to override the behaviour in the controller.
	void		RallyNoPoint( const EntityGroup& eg );
	void		RallyAtPoint( const EntityGroup& eg );
	void		RallyAtFriendlyStructure( const EntityGroup& eg );
	void		RallyAtEnemyStructure( const EntityGroup& eg );
	void		RallyAtFriendlyCreature( const EntityGroup& eg );
	void		RallyAtFriendlyCreatureGrouped( const EntityGroup& eg );
	void		RallyAtEnemyCreature( const EntityGroup& eg );
	void		RallyAtResourceGather( const EntityGroup& eg );
	void		RallyAtEntityGeneric( const EntityGroup& eg, unsigned long command );

	bool		UpdateRallyTarget();

	void		UnitBuildSpawner();
	void		UnitBuildConstruct();

	bool		TryToSpawn( );

	bool		BuildQueueAddSpawn( const ControllerBlueprint* cbp );
	bool		BuildQueueAddConstruct( const ControllerBlueprint* cbp );

	bool		BuildQueueRmvSpawn( size_t index );
	bool		BuildQueueRmvConstruct( size_t index );

	Entity *	UnitSpawn( const ControllerBlueprint* cbp );
	void		UnitNext ();
};
