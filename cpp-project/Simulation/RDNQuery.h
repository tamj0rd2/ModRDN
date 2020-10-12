/////////////////////////////////////////////////////////////////////
// File    : RDNQuery.h
// Desc    :
// Created : Tuesday, February 27, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

//
// * this is a list of functions that help the AI and controllers its a bunch of
// * statistics and query functions that look at the world ...
// * stuff like: find closest dung pile, or find nearest resource or how many of
// * these are there? you know, that shit.

#pragma once

#include "Controllers/ControllerTypes.h"
#include "AttackTypes.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityGroup.h>
#include <SimEngine/FindClosestFilter.h>

#include <Math/Vec3.h>

// forward declaration
class RDNPlayer;
class RDNWorld;
class ControllerBlueprint;
class ThreatPrioritizer;
class MovingExtInfo;
struct AttackPackage;

enum CommandArgumentType;

/////////////////////////////////////////////////////////////////////
// RDNQuery

class RDNQuery
{
	// types
public:
	enum TransportAction
	{
		TA_Pickup,
		TA_DropOff,
	};

	// find enemy - used in idle, guard, attackmove, and patrol (checks for ground, air, swim rules)
public:
	//------------------------------------------------------------------------------------------------------------
	// Given a position, a radius and a filter find the closest unit that satisfies filter, and the use the prioritizer
	//------------------------------------------------------------------------------------------------------------
	static const Entity *FindClosestPrioritize(const Vec3f &apos, float SearchRad, FindClosestFilter &filter, ThreatPrioritizer &prioritizer, const Entity *pIgnore = NULL);

	//------------------------------------------------------------------------------------------------------------
	// Given an entity, a radius and a filter find the closest unit that satisfies filter, and the use the prioritizer
	//------------------------------------------------------------------------------------------------------------
	static const Entity *FindClosestPrioritize(const Entity *pME, float SearchRad, FindClosestFilter &filter, ThreatPrioritizer &prioritizer, const Entity *pIgnore = NULL);

	//------------------------------------------------------------------------------------------------------------
	// Given a position, a radius and an Additional filter find the closest Enemy unit, that also satisfies filter
	//------------------------------------------------------------------------------------------------------------
	static const Entity *FindClosestEnemy(const Entity *pME, const Vec3f &SearchPos, float SearchRad, ThreatPrioritizer &prioritizer, const EntityGroup *pIgnoreGroup = NULL);

	//------------------------------------------------------------------------------------------------------------
	// Given a position, a radius and an Additional filter find the closest Enemy unit, that also satisfies filter
	//------------------------------------------------------------------------------------------------------------
	static const Entity *FindClosestEnemy(const Entity *pEntity, float SearchRad, FindClosestFilter &filter, const EntityGroup *pIgnoreGroup = NULL);

	//-------------------------------------------------------------------------------------
	// Given am entity and a search radius find an enemy that it can attack
	//-------------------------------------------------------------------------------------
	static const Entity *FindClosestEnemy(const Entity *pEntity, float SearchRad, const EntityGroup *pIgnoreGroup = NULL);

	//-------------------------------------------------------------------------------------
	// Given am entity, a position and a search radius find an enemy that it can attack
	//-------------------------------------------------------------------------------------
	static const Entity *FindClosestEnemy(const Entity *pEntity, const Vec3f &SearchPos, float SearchRad, const EntityGroup *pIgnoreGroup = NULL);

	//-------------------------------------------------------------------------------------
	// Given an entity, a position and a max radius find an enemy that has attacked us.
	//	Note this is only valid (right now) for Entities that have an HealthExt
	//-------------------------------------------------------------------------------------
	static const Entity *FindRetaliationEnemy(const Entity *pEntity, float SearchRad, long memoryLength, const EntityGroup *pIgnoreGroup = NULL);

	// determines properties of entities
public:
	// is the entity from same owner, an ally, or an enemy
	static bool IsFriend(const Entity *e, const Player *p);

	// is Entity an enemy of Player
	static bool IsEnemy(const Entity *e, const Player *p);

	// can the Entity be attacked by the Player
	static bool CanBeAttacked(const Entity *e, const Player *p, bool bCheckRelationship = true, bool bCheckFOW = true, bool bCheckVis = true);

	// can the Player rally at the given Entity
	static bool CanRallyTo(const Entity *e, const Player *p);

	// can a Creature guard the Entity
	static bool CanGuard(const EntityGroup &selection, const Entity *e, const Player *p);

	static bool CanBeSeen(const Entity *e, const Player *p, bool bCheckFOW = true);

	static bool CanAttack(const Entity *self, const Entity *target, bool bCheckRelationship = true, bool bCheckFOW = true);

	static bool CanAttack(const EntityGroup &g, const Entity *target, bool bCheckRelationship = true, bool bCheckFOW = true);

	static bool CanGather(const Entity *self, const Entity *target, bool bCheckFOW = true);

	static bool CanGather(const EntityGroup &g, const Entity *target, bool bCheckFOW = true);

	// checks if an entity can be gathered by the given player
	static bool CanBeGathered(const Entity *entity, const Player *player, bool bCheckFOW = true);

	// has this entity been attacked in the last numTicks?
	static bool HasBeenAttackedRecently(Entity *pEntity, long numTicks);

	// was this entity attacked in the last numTicks?
	static bool WasAttackedBy(Entity *pEntity, Entity *pAttacker, long numTicks);
	static bool WasAttackedBy(Entity *pEntity, Player *pAttackerOwner, long numTicks);

	static bool CanDoCommand(const EntityGroup &selection, const EntityGroup &targets, unsigned char command, unsigned long param);
	static bool CanDoCommand(const Entity *entity, const Entity *target, unsigned long command, unsigned long param);

	static bool CanSeePosition(const Entity *self, const Vec3f &position);

	// determines if self can get within melee range of target
	static bool CanTouch(const MovingExtInfo *self, const MovingExtInfo *target);
};

/////////////////////////////////////////////////////////////////////
// FindClosest callbacks

//-------------------------------------------------------------------------------------
// Damage filters - damage everyone or just your enemies. Use this for radial damage,
//  or conical damage functions. (eg. stink attack, attack ground, soundbeam tower)
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// If you need find a valid target - use this filter
//-------------------------------------------------------------------------------------

class FindClosestTargetFilter : public FindClosestFilter
{
public:
	FindClosestTargetFilter(const Entity *pAttacker)
			: m_pAttacker(pAttacker)
	{
		dbAssert(pAttacker);
	};

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Entity *m_pAttacker;
};

//-------------------------------------------------------------------------------------
// If you need find enemy units to cause damage to, meaning they
// have a health extension - use this filter
//-------------------------------------------------------------------------------------

class FindClosestEnemyFilter : public FindClosestFilter
{
public:
	FindClosestEnemyFilter(const Player *pPlayer)
			: m_pPlayer(pPlayer)
	{
		dbAssert(pPlayer);
	};

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Player *m_pPlayer;
};

//-------------------------------------------------------------------------------------
// If you need to find friendly and enemy units to cause damage to, meaning they
// have a health extension - use this filter.
//-------------------------------------------------------------------------------------

class FindClosestUnitWithHealthFilter : public FindClosestFilter
{
public:
	virtual bool Check(const Entity *pEntity);
};

//-------------------------------------------------------------------------------------
// Search filters - searches for visible creatures (filters out unwanted targets)
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// These should only be used internally for finding enemies that can be SEEN
// Don't use this if you are searching for things to cause damage to. Only used
// when you are seeking for things that are VISIBLE.
//-------------------------------------------------------------------------------------

class FindClosestDetectionFilter : public FindClosestFilter
{
public:
	FindClosestDetectionFilter(const Entity *pSelf,
														 const Player *pPlayer,
														 const EntityGroup *pIgnoreGroup,
														 FindClosestFilter *pSecondaryFilter);

	FindClosestDetectionFilter(const Entity *pSelf,
														 const EntityGroup *pIgnoreGroup,
														 FindClosestFilter *pSecondaryFilter);

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Entity *m_pSelf; // The entity that is looking for a target
	const Player *m_pPlayer;
	const EntityGroup *m_pIgnoreGroup; // We will ignore any entities in this group if not NULL

	FindClosestFilter *m_pSecondaryFilter;
};

//-------------------------------------------------------------------------------------
// Special filter: for use by the soundbeam tower since it don't want buildings
//-------------------------------------------------------------------------------------

class FindClosestEnemyNoStealthNoBuildingFilter : public FindClosestFilter
{
public:
	FindClosestEnemyNoStealthNoBuildingFilter(const Player *pPlayer)
			: m_pPlayer(pPlayer)
	{
		dbAssert(pPlayer);
	};

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Player *m_pPlayer;
};

//-------------------------------------------------------------------------------------
// Special filter: for use by the antiair tower since it only wants to attack flyers
//-------------------------------------------------------------------------------------

class FindClosestEnemyFlyerFilter : public FindClosestFilter
{
public:
	FindClosestEnemyFlyerFilter(const Player *pPlayer)
			: m_pPlayer(pPlayer)
	{
		dbAssert(pPlayer);
	}

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Player *m_pPlayer;
};

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------

class FindClosestUnitWithOwnerFilter : public FindClosestFilter
{
public:
	FindClosestUnitWithOwnerFilter(const Player *pPlayer)
			: m_pPlayer(pPlayer)
	{
		dbAssert(pPlayer);
	};

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Player *m_pPlayer;
};

//-------------------------------------------------------------------------------------
// Special filter: for finding all enemies of a given controller type
//-------------------------------------------------------------------------------------

class FindClosestEnemyOfType : public FindClosestFilter
{
public:
	FindClosestEnemyOfType(const Entity *pSearcher, unsigned long ControllerType)
			: m_pSearcher(pSearcher),
				m_ControllerType(ControllerType)
	{
		dbAssert(pSearcher);
	};

	// Check to see if this entity is an enemy
	virtual bool Check(const Entity *pEntity);

private:
	const Entity *m_pSearcher;
	unsigned long m_ControllerType;
};

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------

class ThreatPrioritizer
{
public:
	ThreatPrioritizer() {}
	virtual ~ThreatPrioritizer() {}

	// prioritize an enemy
	virtual int Prioritize(const Entity *pEntity) = 0;
};

//-------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------

class ThreatPrioritizerAll : public ThreatPrioritizer
{
public:
	ThreatPrioritizerAll(const Player *player) : m_pPlayer(player)
	{
	}

	// prioritize an enemy
	virtual int Prioritize(const Entity *pEntity);

private:
	const Player *m_pPlayer;
};
