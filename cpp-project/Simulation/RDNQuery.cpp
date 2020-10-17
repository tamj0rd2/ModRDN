/////////////////////////////////////////////////////////////////////
// File    : RDNQuery.cpp
// Desc    :
// Created : Friday, February 23, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNQuery.h"

#include "../ModObj.h"

#include "RDNWorld.h"
#include "RDNPlayer.h"
#include "PlayerFow.h"
#include "CommandProcessor.h"
#include "AttackPackage.h"

#include "Controllers/ModController.h"

#include "Extensions/ModifierExt.h"
#include "Extensions/AttackExt.h"
#include "Extensions/ResourceExt.h"
#include "Extensions/HealthExt.h"
#include "Extensions/MovingExt.h"
#include "Extensions/SightExt.h"

#include "ExtInfo/AttackExtInfo.h"
#include "ExtInfo/MovingExtInfo.h"
#include "ExtInfo/SiteExtInfo.h"

#include "States/StateAttack.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityGroup.h>
#include <SimEngine/SimHelperFuncs.h>
#include <SimEngine/EntityUtil.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/Pathfinding/ImpassMap.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <EngineAPI/EntityFactory.h>

/////////////////////////////////////////////////////////////////////
// Filter constructors

FindClosestDetectionFilter::FindClosestDetectionFilter(const Entity *pSelf,
																											 const Player *pPlayer,
																											 const EntityGroup *pIgnoreGroup,
																											 FindClosestFilter *pSecondaryFilter)
		: m_pSelf(pSelf),
			m_pPlayer(pPlayer),
			m_pIgnoreGroup(pIgnoreGroup),
			m_pSecondaryFilter(pSecondaryFilter)
{
	dbAssert(pSelf);
}

FindClosestDetectionFilter::FindClosestDetectionFilter(const Entity *pSelf,
																											 const EntityGroup *pIgnoreGroup,
																											 FindClosestFilter *pSecondaryFilter)
		: m_pSelf(pSelf),
			m_pPlayer(pSelf->GetOwner()),
			m_pIgnoreGroup(pIgnoreGroup),
			m_pSecondaryFilter(pSecondaryFilter)
{
	// moved from InitFindClosestDetectionFilter()
	//

	dbAssert(pSelf);
}

/////////////////////////////////////////////////////////////////////
//

static bool CanAttackType(const Entity *attacker, const Entity *target)
{
	// validate parms
	dbAssert(attacker);
	dbAssert(target);

	//
	unsigned long attackerControllerType = NULL_EC;

	if (attacker)
	{
		attackerControllerType = attacker->GetControllerBP()->GetControllerType();
	}

	// can self attack?
	const AttackExtInfo *attack = QIExtInfo<AttackExtInfo>(attacker->GetController());

	if (attack == NULL)
		return false;

	// close combat guys...
	const MovingExtInfo *movSelf = QIExtInfo<MovingExtInfo>(attacker->GetController());
	const MovingExtInfo *movTarget = QIExtInfo<MovingExtInfo>(target->GetController());

	const MovingExt *pMovExtSelf = QIExt<MovingExt>(attacker->GetController());
	const MovingExt *pMovExtTarget = QIExt<MovingExt>(target->GetController());

	if ((movSelf == 0) || (pMovExtSelf == 0))
	{
		// entity may be dead
		return false;
	}
	else if ((movTarget == 0) || (pMovExtTarget == 0))
	{
		// is building - never return true in this block
	}
	else
	{
		// NOTE: flyers can now attack anything and everything
		// this means ground units are attackable by everything, hence
		// the removal of the last 'else if'

		// if a flyer and the other guy isn't
		if (movTarget->IsFlyer() && !movSelf->IsFlyer())
			return false;
		else
				// if you are a water creature and the other guy is not a swimmer or flyer
				if (pMovExtTarget->IsSwimmer() && !movTarget->IsGround() && (!pMovExtSelf->IsSwimmer() && !movSelf->IsFlyer()))
			return false;
		else
				// if you are a ground creature and the target is not
				if (movTarget->IsGround() && !pMovExtTarget->IsSwimmer() && (!movSelf->IsGround() && !movSelf->IsFlyer()))
			return false;
	}

	if (!movSelf->IsFlyer())
	{
		// can I reach this target
		const PathfinderQuery *pq = ModObj::i()->GetWorld()->GetPathfinder()->Query();
		if (!pq->IsReachableFrom(
						Vec2f(attacker->GetPosition().x, attacker->GetPosition().z),
						Vec2f(target->GetPosition().x, target->GetPosition().z),
						movSelf->movingType == movSelf->MOV_AMPHIBIOUS))
		{
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
// If you need find enemy units to cause damage to, meaning they
// have a health extension - use this filter
//-------------------------------------------------------------------------------------

bool FindClosestEnemyFilter::Check(const Entity *pEntity)
{
	// check relationship, don't check FOW or visibility
	return RDNQuery::CanBeAttacked(pEntity, m_pPlayer, true, false, false);
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool FindClosestTargetFilter::Check(const Entity *target)
{
	return RDNQuery::CanAttack(m_pAttacker, target);
}

//-------------------------------------------------------------------------------------
// If you need to find friendly and enemy units to cause damage to, meaning they
// have a health extension - use this filter.
//-------------------------------------------------------------------------------------

bool FindClosestUnitWithHealthFilter::Check(const Entity *pEntity)
{
	// does this entity have an owner
	if (pEntity->GetOwner())
	{
		if (pEntity->GetController())
		{
			if (QIExt<HealthExt>(pEntity->GetController()))
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------
// Detection queries
//
//  These functions search for creatures to attack, as in attack move, guard and patrol. This
//  should filter out stealth creatures or things you cannot SEE.
//
//------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
// These should only be used internally for finding no flyers and no stealth.
// Don't use this if you are searching for things to cause damage to. Only used
// when you are seeking for things that are VISIBLE.
//-------------------------------------------------------------------------------------

bool FindClosestDetectionFilter::Check(const Entity *pEntity)
{
	// is pEntity in our ignoreGroup?
	if (m_pIgnoreGroup && m_pIgnoreGroup->find(pEntity) != m_pIgnoreGroup->end())
	{ // ignoring
		return false;
	}

	// check that it can be attacked
	dbAssert(m_pSelf);
	if (!RDNQuery::CanAttack(m_pSelf, pEntity, true))
		return false;

	if (m_pSecondaryFilter)
		return m_pSecondaryFilter->Check(pEntity);

	return true;
}

//-------------------------------------------------------------------------------------
// Used for searching/targeting. Do not target stealth or buildings. This is used
// currently by the soundbeam tower and by the AI for determining area of threat.
//-------------------------------------------------------------------------------------

bool FindClosestEnemyNoStealthNoBuildingFilter::Check(const Entity *pEntity)
{
	// check that it can be attacked
	if (!RDNQuery::CanBeAttacked(pEntity, m_pPlayer))
		return false;

	// is building
	const SiteExtInfo *site = QIExtInfo<SiteExtInfo>(pEntity);
	if (site)
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
// Used for searching/targeting. Only target flying creatures. This is used
// currently by the anti-air tower.
//-------------------------------------------------------------------------------------

bool FindClosestEnemyFlyerFilter::Check(const Entity *pEntity)
{
	// check that it can be attacked
	if (!RDNQuery::CanBeAttacked(pEntity, m_pPlayer))
		return false;

	// is flyer
	const MovingExtInfo *moving = QIExtInfo<MovingExtInfo>(pEntity);
	if (moving && moving->IsFlyer())
		return true;

	return false;
}

//-------------------------------------------------------------------------------------
// Used for searching/targeting. Only Entities of the desired type.
//-------------------------------------------------------------------------------------

bool FindClosestEnemyOfType::Check(const Entity *pEntity)
{
	dbAssert(pEntity);

	// check that it can be attacked
	if (!RDNQuery::CanAttack(m_pSearcher, pEntity))
		return false;

	if (pEntity->GetControllerBP() && pEntity->GetControllerBP()->GetControllerType() == m_ControllerType)
	{
		// this is the type we are looking for
		return true;
	}

	return false;
}

// Used for finding the closest foundry/workshop
bool FindClosestResourceDepsoit::Check(const Entity *pEntity)
{
	const unsigned long controllerType = pEntity->GetControllerBP()->GetControllerType();
	return controllerType == Workshop_EC || controllerType == Lab_EC;
}

// Used for finding the cloest entity that matches the given controller type
bool FindClosestEntityOfType::Check(const Entity *pEntity)
{
	const unsigned long controllerType = pEntity->GetControllerBP()->GetControllerType();
	return controllerType == m_ControllerType;
}

//-------------------------------------------------------------------------------------
// Prioritize an enemy entity according to how threatening it is.
//	An entity with a priortity <= -1 will never be chosen
//-------------------------------------------------------------------------------------

namespace
{
	struct ControllerPriority
	{
		// a Controller with this priority will be ignored
		enum
		{
			IGNORE_ME = -1
		};

		ControllerType type;
		int priority;
	};

	ControllerPriority CONTROLLER_PRIORITY[] =
			{
					{Lab_EC, 100},
					{Henchmen_EC, 100},
					{Coal_EC, 0},
	};
} // namespace

static int ThreatPrioritizeExecute(const Entity *pEntity, const Player *pPlayer, ControllerPriority *controllerList, int length)
{
	int priority = 0;

	// priority base on controller types
	if (pEntity->GetControllerBP())
	{
		unsigned long type = pEntity->GetControllerBP()->GetControllerType();

		for (int i = 0; i < length; i++)
		{
			if (type == (unsigned long)(controllerList[i].type))
			{
				priority = controllerList[i].priority;

				if (priority <= ControllerPriority::IGNORE_ME)
				{
					// never choose this type of controller
					continue;
				}

				if (pEntity->GetController())
				{
					const ModController *pController = static_cast<const ModController *>(pEntity->GetController());

					for (;;)
					{
						// make it a high priority if the entity's attacking us
						const int AttackPriorityBonus = 1000;
						const StateAttack *pStateAttack =
								static_cast<const StateAttack *>(pController->QIActiveState(State::SID_Attack));
						if (pStateAttack)
						{
							if (pStateAttack->GetTargetEntity() && pStateAttack->GetTargetEntity()->GetOwner() == pPlayer)
							{
								// increase priority
								priority += AttackPriorityBonus;
								break;
							}
						}

						// no more checks for increased priority
						break;
					}
				}

				break;
			}
		}
	}

	return priority;
}

//-------------------------------------------------------------------------------------
// prioritize all controller types
//-------------------------------------------------------------------------------------

int ThreatPrioritizerAll::Prioritize(const Entity *pEntity)
{
	return ThreatPrioritizeExecute(pEntity, m_pPlayer, CONTROLLER_PRIORITY, LENGTHOF(CONTROLLER_PRIORITY));
}

//-------------------------------------------------------------------------------------
// Given a position, a radius and a filter find the closest unit
//-------------------------------------------------------------------------------------

const Entity *RDNQuery::FindClosestPrioritize(const Vec3f &apos, float SearchRad, FindClosestFilter &filter, ThreatPrioritizer &prioritizer, const Entity *pIgnore)
{
	EntityGroup tempGroup;
	// find closest enemy
	ModObj::i()->GetWorld()->FindClosest(tempGroup, filter, 0, apos, SearchRad, pIgnore);

	// prioritize enemies
	EntityGroup::iterator ei = tempGroup.begin();
	EntityGroup::iterator ee = tempGroup.end();

	Entity *pEntity = NULL;
	int entityPrio = ControllerPriority::IGNORE_ME;

	for (; ei != ee; ei++)
	{
		int prio = prioritizer.Prioritize(*ei);
		if (prio > entityPrio)
		{
			pEntity = *ei;
			entityPrio = prio;
		}
	}

	return pEntity;
}

//-------------------------------------------------------------------------------------
// Given a position, a radius and a filter find the closest unit
//-------------------------------------------------------------------------------------

const Entity *RDNQuery::FindClosestPrioritize(const Entity *pME, float SearchRad, FindClosestFilter &filter, ThreatPrioritizer &prioritizer, const Entity *pIgnore)
{
	EntityGroup tempGroup;
	// find closest enemy
	ModObj::i()->GetWorld()->FindClosest(tempGroup, filter, 0, pME, SearchRad, pIgnore);

	// prioritize enemies
	EntityGroup::iterator ei = tempGroup.begin();
	EntityGroup::iterator ee = tempGroup.end();

	Entity *pEntity = NULL;
	int entityPrio = ControllerPriority::IGNORE_ME;

	for (; ei != ee; ei++)
	{
		int prio = prioritizer.Prioritize(*ei);
		if (prio > entityPrio)
		{
			pEntity = *ei;
			entityPrio = prio;
		}
	}

	return pEntity;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
const Entity *RDNQuery::FindClosestEnemy(const Entity *pEntity, float SearchRad, FindClosestFilter &secondaryfilter, const EntityGroup *pIgnoreGroup)
{
	dbAssert(pEntity);

	// eventually we could move this filter out of here and layer it with other filters
	FindClosestDetectionFilter filter(pEntity, pIgnoreGroup, &secondaryfilter);

	//
	ThreatPrioritizerAll prioritizer(pEntity->GetOwner());

	return FindClosestPrioritize(pEntity->GetPosition(), SearchRad, filter, prioritizer);
}

//-------------------------------------------------------------------------------------
// Given am entity and a search radius find an enemy that it can attack
//-------------------------------------------------------------------------------------

const Entity *RDNQuery::FindClosestEnemy(const Entity *pME, float SearchRad, const EntityGroup *pIgnoreGroup)
{
	return FindClosestEnemy(pME, pME->GetPosition(), SearchRad, pIgnoreGroup);
}

const Entity *RDNQuery::FindClosestEnemy(const Entity *pME, const Vec3f &SearchPos, float SearchRad, const EntityGroup *pIgnoreGroup)
{
	dbAssert(pME);

	// eventually we could move this filter out of here and layer it with other filters
	FindClosestDetectionFilter filter(pME, pIgnoreGroup, NULL);

	//
	ThreatPrioritizerAll prioritizer(pME->GetOwner());

	return FindClosestPrioritize(SearchPos, SearchRad, filter, prioritizer);
}

const Entity *RDNQuery::FindClosestEnemy(const Entity *pME, const Vec3f &SearchPos, float SearchRad, ThreatPrioritizer &prioritizer, const EntityGroup *pIgnoreGroup)
{
	dbAssert(pME);

	// eventually we could move this filter out of here and layer it with other filters
	FindClosestDetectionFilter filter(pME, pIgnoreGroup, NULL);

	return FindClosestPrioritize(SearchPos, SearchRad, filter, prioritizer);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Finds the closest enemy that has attacked pEntity, and can be attacked back.  Takes type and FoW into account.
//	Result	: return the attacker
//	Param.	: pEntity - the Entity we are searching for attackers
//			  searchRad - the found attacker must be within this radius
//			  memoryLength - only remember back this many ticks
//	Author	: dswinerd
//
const Entity *RDNQuery::FindRetaliationEnemy(const Entity *pEntity, float searchRad, long memoryLength, const EntityGroup *pIgnoreGroup)
{
	const HealthExt *pHealthExt = QIExt<HealthExt>(pEntity);
	if (!pHealthExt)
	{
		return 0;
	}

	// get the EntityGroup that contains all the Entities that we remember attacking us
	EntityGroup attackers;
	pHealthExt->GetAttackMemory().GetAttackers(attackers, ModObj::i()->GetWorld()->GetGameTicks() - memoryLength);

	if (!attackers.empty())
	{ // find the closest one of these (that we can see) that is within searchRad

		float bestDistSqr = FLT_MAX;
		Entity *pBestEntity = 0;

		EntityGroup::iterator ib = attackers.begin();
		EntityGroup::iterator ie = attackers.end();
		for (; ib != ie; ++ib)
		{
			Entity *pAttacker = *ib;

			if (pIgnoreGroup && pIgnoreGroup->find(pAttacker) != pIgnoreGroup->end())
			{
				// ignoring him
				continue;
			}

			if (!RDNQuery::CanAttack(pEntity, pAttacker))
				// pEntity can't attack pAttacker
				continue;

			float distSqr = EntityUtil::DistSqrDirCalcEntity(pEntity, pAttacker, 0, NULL);

			if (distSqr < bestDistSqr && distSqr < searchRad * searchRad)
			{ // new closest previous attacker
				bestDistSqr = distSqr;
				pBestEntity = pAttacker;
			}
		}

		if (pBestEntity)
		{ // found a target
			return pBestEntity;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
#define ENTITYQUERY(n)                                       \
	bool RDNQuery::n(const Entity *self, const Entity *target) \
	{                                                          \
		/* validate parm */                                      \
		if (self == 0)                                           \
		{                                                        \
			dbBreak();                                             \
			return 0;                                              \
		}                                                        \
                                                             \
		EntityGroup g;                                           \
		g.push_back(const_cast<Entity *>(self));                 \
                                                             \
		return n(g, target);                                     \
	}

//	ENTITYQUERY( CanBuild			)

#undef ENTITYQUERY

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if the given Player can see the given Entity
//	Result	: returns true if visible
//	Param.	: e - the Entity we are testing
//			  p - the Player doing the viewings
//
bool RDNQuery::CanBeSeen(const Entity *e, const Player *p, bool bCheckFOW)
{
	// validate parm
	if (e == 0 || p == 0)
		return false;

	// entities without controller are always visible: trees, rocks, waterfalls, ...
	if (e->GetController() == 0)
		return true;

	// entities that the player control are always visible
	if (p->CanControlEntity(e))
		return true;

	//
	const PlayerFOW *fow = static_cast<const RDNPlayer *>(p)->GetFogOfWar();

	// standard unit
	return (!bCheckFOW || fow->IsVisible(e));
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines if the given Entity is a friend
//	Result	: returns true iff a friend
//	Param.	: e - the Entity we are testing
//			  p - the Player we are testing against
//
bool RDNQuery::IsFriend(const Entity *e, const Player *p)
{
	// validate parms
	if (e == 0)
		return false;

	if (p == 0)
		return false;

	// check self
	if (p->CanControlEntity(e))
		return true;

	// never friend with nature object
	if (e->GetOwner() == 0)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines if the given Entity is an enemy
//	Result	: returns true iff an enemy
//	Param.	: e - the Entity we are testing
//			  p - the Player we are testing against
//	Author	: dswinerd
//
bool RDNQuery::IsEnemy(const Entity *e, const Player *p)
{
	// validate parms
	if (e == 0)
		return false;

	if (p == 0)
		return false;

	// check self
	if (p->CanControlEntity(e))
		return false;

	// never enemy with nature object
	if (e->GetOwner() == 0)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines if the given Entity can be attacked by the given Player
//	Result	: returns true or false
//	Param.	: e - the Entity we are testing
//			  p - the Player wanting to attack
//			  bCheckRelationship - will be true if we want to check alliances, etc.
//
bool RDNQuery::CanBeAttacked(const Entity *e, const Player *p, bool bCheckRelationship, bool bCheckFOW, bool bCheckVis)
{
	UNREF_P(bCheckRelationship); //	used for alliance system

	// validate parm
	if (e == 0)
		return false;

	if (e->GetOwner() == 0)
		// nature object -- can't be attacked
		return false;

	// can target be attacked
	const HealthExt *health = QIExt<HealthExt>(e);

	if (health == 0 || health->GetHealth() == 0.0f)
		return false;

	// can't attack own units
	if (e->GetOwner() == p)
		return false;

	// make sure target is visible
	if (bCheckVis)
	{
		if (RDNQuery::CanBeSeen(e, p, bCheckFOW) == 0)
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines if we can rally to the given entity
//	Result	:
//	Param.	:
//
bool RDNQuery::CanRallyTo(const Entity *e, const Player *p)
{
	UNREF_P(p);

	if (!e)
	{ // rally at a point
		return (true);
	}

	// right now can rally to any entity, but that will probably change

	return (true);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Determines if a Creature can guard the given entity
//	Result	: returns true or false
//	Param.	: selection - current creature selection
//	          e - the entity to guard
//			  p - the player controlling the creature
//
bool RDNQuery::CanGuard(const EntityGroup &selection, const Entity *e, const Player *p)
{
	// can guard just a point
	if (e == 0)
		return true;

	// cannot guard trees & rocks
	if (e->GetOwner() == 0)
		return false;

	// p cannot guard un-allied entities
	if (RDNQuery::IsFriend(e, p) == 0)
		return false;

	return !selection.empty();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNQuery::CanAttack(const EntityGroup &g, const Entity *target, bool bCheckRelationship, bool bCheckFOW)
{
	// validate parm
	if (g.empty())
	{
		dbBreak();
		return false;
	}

	//
	if (!CanBeAttacked(target, g.front()->GetOwner(), bCheckRelationship, bCheckFOW))
		return false;

	// check if any dude in the group can do it
	EntityGroup::const_iterator i = g.begin();
	EntityGroup::const_iterator e = g.end();

	for (; i != e; ++i)
	{
		if (CanAttackType(*i, target))
			break;
	}

	if (i == e)
		// nobody
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool RDNQuery::CanAttack(const Entity *self, const Entity *target, bool bCheckRelationship, bool bCheckFOW)
{
	/* validate parm */
	if (self == 0)
	{
		dbBreak();
		return 0;
	}

	EntityGroup g;
	g.push_back(const_cast<Entity *>(self));

	return CanAttack(g, target, bCheckRelationship, bCheckFOW);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool RDNQuery::CanDoCommand(const EntityGroup &selection, const EntityGroup &targets, unsigned char command, unsigned long param)
{
	UNREF_P(command);
	UNREF_P(param);

	if (selection.empty() || targets.empty())
	{
		// nothing can't do anything
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNQuery::CanDoCommand(const Entity *entity, const Entity *target, unsigned long command, unsigned long param)
{
	if (target == NULL)
	{
		return CommandProcessor::CanDoCommand(entity, command, param);
	}
	else
	{
		return CommandProcessor::CanDoCommand(entity, target, command, param);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool FindClosestUnitWithOwnerFilter::Check(const Entity *pEntity)
{
	return pEntity->GetOwner() == m_pPlayer;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines the given entity has been attacked in the last numTicks
//	Result	: return true if has been attacked
//	Param.	: pEntity - the Entity we are checking
//			  numTicks - the number of ticks in the past we are concerned about
//	Note	: pEntity - needs a HealthExt to have a memory
//	Author	: dswinerd
//
bool RDNQuery::HasBeenAttackedRecently(Entity *pEntity, long numTicks)
{
	// validate parms
	if (!pEntity)
	{
		return false;
	}

	HealthExt *pHealthExt = QIExt<HealthExt>(pEntity);
	if (!pHealthExt)
	{
		return true;
	}

	return pHealthExt->GetAttackMemory().HasBeenAttackedSince(ModObj::i()->GetWorld()->GetGameTicks() - numTicks);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if pEntity was attacked by pAttacker in the last numTicks
//	Result	: return if attacked
//	Param.	: pEntity - the Entity that was attacked?
//			  pAttacker - the Entity that did the attacking?
//			  numTicks - the number of ticks in the past we are concerned about
//	Author	: dswinerd
//
bool RDNQuery::WasAttackedBy(Entity *pEntity, Entity *pAttacker, long numTicks)
{
	// validate parms
	if (!pEntity || !pAttacker)
	{
		return false;
	}

	HealthExt *pHealthExt = QIExt<HealthExt>(pEntity);
	if (!pHealthExt)
	{
		return false;
	}

	return pHealthExt->GetAttackMemory().WasAttackedBy(pAttacker, ModObj::i()->GetWorld()->GetGameTicks() - numTicks);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if pEntity was attacked by entities owned by pAttackerOwner in the last numTicks
//	Result	: return true if attacked
//	Param.	: pEntity - the Entity that was attacked?
//			  pAttackerOwner - the Player whose entities did the attacking?
//			  numTicks - the number of ticks in the past we are concerned about
//	Author	: dswinerd
//
bool RDNQuery::WasAttackedBy(Entity *pEntity, Player *pAttackerOwner, long numTicks)
{
	// validate parms
	if (!pEntity || !pAttackerOwner)
	{
		return false;
	}

	HealthExt *pHealthExt = QIExt<HealthExt>(pEntity);
	if (!pHealthExt)
	{
		return false;
	}

	return pHealthExt->GetAttackMemory().WasAttackedBy(pAttackerOwner, ModObj::i()->GetWorld()->GetGameTicks() - numTicks);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if the given entity can see a position
//	Result	: returns true if can see
//	Param.	: self - the entity looking
//			  position
//
bool RDNQuery::CanSeePosition(const Entity *self, const Vec3f &position)
{
	const RDNPlayer *pOwner = static_cast<const RDNPlayer *>(self->GetOwner());
	if (!pOwner)
	{
		// must have an owner!
		return true;
	}

	if (!pOwner->GetFogOfWar())
	{
		return true;
	}

	return pOwner->GetFogOfWar()->IsVisible(position);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines if it is possible for one moving object to touch another. ie. to get within melee range
//	Result	: returns true if can touch
//
bool RDNQuery::CanTouch(const MovingExtInfo *movSelf, const MovingExtInfo *movTarget)
{
	if (!movSelf || !movTarget)
	{
		// can't touch nothing
		return false;
	}

	// if here self and target are both on land/water.  It's possible they may touch.
	//   Note: land exclusive and water exclusive may touch if they are both on the shore.
	return true;
}

// Desc: checks if an entity can gather from another entity
bool RDNQuery::CanGather(const Entity *self, const Entity *target, bool bCheckFOW)
{
	if (self == 0)
	{
		dbBreak();
		return 0;
	}

	dbTracef("RDNQuery::CanGather | checking if %s can gather from %s",
					 self->GetControllerBP()->GetFileName(), target->GetControllerBP()->GetFileName());

	EntityGroup g;
	g.push_back(const_cast<Entity *>(self));

	return CanGather(g, target, bCheckFOW);
}

bool RDNQuery::CanGather(const EntityGroup &group, const Entity *target, bool bCheckFOW)
{
	if (group.empty())
	{
		dbTracef("Group is somehow empty");
		dbBreak();
		return false;
	}

	if (!CanBeGathered(target, group.front()->GetOwner(), bCheckFOW))
	{
		dbTracef("Coal cannot be gathered :(");
		return false;
	}

	// check if any dude in the group can do it
	EntityGroup::const_iterator entityIterator = group.begin();
	EntityGroup::const_iterator iteratorEnd = group.end();

	for (; entityIterator != iteratorEnd; ++entityIterator)
	{
		Entity *theEntity = *entityIterator;
		if (theEntity->GetControllerBP()->GetControllerType() == Henchmen_EC)
			return true;
	}

	return false;
}

bool RDNQuery::CanBeGathered(const Entity *entity, const Player *player, bool bCheckFOW)
{
	if (entity == 0)
		return false;

	if (entity->GetOwner() != 0)
	{
		// can only gather from nature resources, unowned by players
		dbTracef("Entity owner is not 0");
		return false;
	}

	const ResourceExt *resource = QIExt<ResourceExt>(entity);

	if (resource == 0 || resource->GetResources() <= 0.0f)
	{
		dbTracef("Did not get resource using QI thingy, or no resources left within entity");
		return false;
	}

	if (RDNQuery::CanBeSeen(entity, player, bCheckFOW) == 0)
	{
		dbTracef("Item cannot be seen");
		return false;
	}

	return true;
}
