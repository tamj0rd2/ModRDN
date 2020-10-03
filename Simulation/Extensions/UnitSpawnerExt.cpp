/////////////////////////////////////////////////////////////////////
// File    : UnitSpawnerExt.cpp
// Desc    :
// Created : Thursday, February 22, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "UnitSpawnerExt.h"

#include "../../ModObj.h"

#include "../CommandTypes.h"
#include "../RDNPlayer.h"
#include "../RDNWorld.h"
#include "../RDNQuery.h"
#include "../RDNEBPs.h"
#include "../GameEventDefs.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/MovingExtInfo.h"
#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/CostExtInfo.h"

#include <SimEngine/EntityAnimator.h>

#include <SimEngine/Entity.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/SpatialBucket.h>

#include <SimEngine/EntityDynamics.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <SurfVol/OBB3.h>

#include <Util/Biff.h>
#include <Util/Iff.h>
#include <Util/IffMath.h>

/////////////////////////////////////////////////////////////////////
//

static bool IsAtUnitCap(const Player *player)
{
	//
	const RDNPlayer *splayer = static_cast<const RDNPlayer *>(player);

	if (player == 0)
		return true;

	return splayer->PopulationTotal() >= splayer->PopulationMax();
}

static void RallyEvent(const Entity *unitSpawner)
{
	GameEventSys::Instance()->PublishEvent(
			GameEvent_RallyPointSet(static_cast<const RDNPlayer *>(unitSpawner->GetOwner()), unitSpawner));
}

/////////////////////////////////////////////////////////////////////
// UnitSpawnerExt

UnitSpawnerExt::UnitSpawnerExt(BuildType buildtype)
		: m_unitInProgressTicks(0),
			m_unitInProgressCount(0),
			m_rallyType(RALLY_NoPoint),
			m_rallyTheta(0.0f),
			m_rallyPoint(0, 0, 0),
			m_rallyLastSeen(0, 0, 0),
			m_buildType(buildtype),
			m_bWaitingForSpace(false),
			m_waitFinishTime(0)
{
	m_rallyEntity.SetFlag(EF_CanCollide);
}

bool UnitSpawnerExt::UnitListFilter(unsigned long ebpid) const
{
	//
	const ControllerBlueprint *cbp =
			ModObj::i()->GetEntityFactory()->GetControllerBP(ebpid);

	if (cbp == 0)
	{
		dbBreak();
		return false;
	}

	return UnitListFilter(cbp);
}

bool UnitSpawnerExt::UnitListFilter(const ControllerBlueprint *cbp) const
{
	// check player
	const RDNPlayer *player =
			static_cast<const RDNPlayer *>(const_cast<UnitSpawnerExt *>(this)->GetSelf()->GetEntity()->GetOwner());

	if (player == 0)
		return false;

	//
	const RDNPlayer::BuildResult br = player->BlueprintCanBuild(cbp);

	return br == RDNPlayer::BR_AllowBuild ||
				 br == RDNPlayer::BR_NeedResourceCash ||
				 br == RDNPlayer::BR_NeedPrerequisite;
}

void UnitSpawnerExt::UnitList(ControllerBPList &l) const
{
	// init out parm
	l.clear();

	/***
	// validate object state
	const RDNPlayer* player = 
		static_cast< const RDNPlayer* >( const_cast< UnitSpawnerExt* >( this )->GetSelf()->GetEntity()->GetOwner() );

	if( player == 0 )
	{
		dbBreak(); return;
	}

	// add henchman
	const EntityFactory* ef = ModObj::i()->GetEntityFactory();

	const ControllerBlueprint* henchman = const_cast< EntityFactory* >( ef )->GetControllerBP
		( 
		RDNEBP::Henchman.folder,
		RDNEBP::Henchman.file
		);

	if( UnitListFilter( henchman ) )
	{
		l.push_back( henchman );
	}

	// add army
	std::vector<long>::const_iterator i = player->GetArmy().begin();
	std::vector<long>::const_iterator e = player->GetArmy().end();

	for( ; i != e; ++i )
	{
		const ControllerBlueprint *pCBP = const_cast< EntityFactory* >( ef )->GetControllerBP( *i );

		if( UnitListFilter( pCBP ) )
		{
			l.push_back( pCBP );
		}
	}
***/

	return;
}

void UnitSpawnerExt::OnUnitBuild(size_t)
{
}

void UnitSpawnerExt::OnUnitSpawnLocation(const Vec3f &)
{
}

bool UnitSpawnerExt::BuildQueueAdd(const unsigned long ebpid)
{
	const ControllerBlueprint *cbp =
			ModObj::i()->GetEntityFactory()->GetControllerBP(ebpid);

	return BuildQueueAdd(cbp);
}

bool UnitSpawnerExt::IsAvailable() const
{
	//
	const ModController *thisModC = const_cast<UnitSpawnerExt *>(this)->GetSelf();

	// check owner
	if (thisModC->GetEntity()->GetOwner() == 0)
		return false;

	// check queues
	switch (m_buildType)
	{
	case BT_Spawn:
		// check maximum queue length
		if (m_unitQueue.size() >= MAXQUEUELENGTH)
			return false;
		break;
	case BT_Construct:
		// are we constructing anything already?
		if (!m_unitConstructing.empty())
			return false;
		break;
	default:
		// should never hit this
		dbBreak();
		break;
	}

	return true;
}

bool UnitSpawnerExt::IsBlocked() const
{
	return m_bWaitingForSpace;
}

bool UnitSpawnerExt::BuildPay(const ControllerBlueprint *cbp)
{
	// validate parm
	if (cbp == 0)
	{
		dbBreak();
		return false;
	}

	// make sure we can build this unit
	if (UnitListFilter(cbp) == 0)
	{
		dbBreak();
		return false;
	}

	// is this building ready to build a unit
	if (IsAvailable() == false)
		return false;

	// check player
	RDNPlayer *player = static_cast<RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());

	if (player == 0)
		return false;

	//
	if (player->BlueprintCanBuild(cbp) != RDNPlayer::BR_AllowBuild)
		return false;

	// if either of these attributes don't exist then we can't build this
	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

	if (cost == 0)
	{
		dbBreak();
		return false;
	}

	// pay the piper
	player->DecResourceCash(cost->costCash * player->GetRaceBonusCost(cbp));

	return true;
}

bool UnitSpawnerExt::BuildRefund(RDNPlayer *pPlayer, const ControllerBlueprint *cbp)
{
	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);
	dbAssert(si != 0);

	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

	dbAssert(cost != 0);

	pPlayer->IncResourceCash(
			cost->costCash * pPlayer->GetRaceBonusCost(cbp),
			pPlayer->RES_Refund);

	return true;
}

bool UnitSpawnerExt::BuildQueueAdd(const ControllerBlueprint *cbp)
{
	switch (m_buildType)
	{
	case BT_Spawn:
		return BuildQueueAddSpawn(cbp);
	case BT_Construct:
		return BuildQueueAddConstruct(cbp);
	default:
		dbBreak();
		return false;
	}
}

bool UnitSpawnerExt::BuildQueueAddSpawn(const ControllerBlueprint *cbp)
{
	dbAssert(m_buildType == BT_Spawn);

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitCommand(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), cbp));

	if (!BuildPay(cbp))
	{ // couldn't build or pay for
		return false;
	}

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitStart(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), cbp));

	// add to end of build queue
	m_unitQueue.push_back(cbp);

	//
	if (m_unitQueue.size() == 1)
	{
		// start building this one
		UnitNext();
	}

	return true;
}

bool UnitSpawnerExt::BuildQueueAddConstruct(const ControllerBlueprint *cbp)
{
	dbAssert(m_buildType == BT_Construct);

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitCommand(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), cbp));

	if (!BuildPay(cbp))
	{
		return false;
	}

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitStart(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), cbp));

	// paid for, now spawn
	return AddConstruct(cbp);
}

bool UnitSpawnerExt::AddConstruct(const ControllerBlueprint *cbp)
{
	Entity *pEntity = UnitSpawn(cbp);
	if (!pEntity)
	{
		dbBreak();
		return false;
	}

	m_unitConstructing.clear();
	m_unitConstructing.push_back(pEntity);

	// make sure the entity in 'unbuilt'
	OnUnitBuildByConstructing(m_unitConstructing.front(), 0.0f);

	// ticks needed
	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

	m_unitInProgressCount = 0;
	const CostExtInfo *pCost = QIExtInfo<CostExtInfo>(si);
	if (pCost)
	{
		m_unitInProgressCount = pCost->constructionTicks;
	}

	m_unitInProgressTicks = m_unitInProgressCount;

	return true;
}

bool UnitSpawnerExt::BuildQueueRmv(size_t index)
{
	switch (m_buildType)
	{
	case BT_Spawn:
		// delegate
		return BuildQueueRmvSpawn(index);
	case BT_Construct:
		// delegate
		return BuildQueueRmvConstruct(index);
	default:
		dbBreak();
		return false;
	}
}

bool UnitSpawnerExt::BuildQueueRmvSpawn(size_t index)
{
	dbAssert(m_buildType == BT_Spawn);

	// validate parm
	if (index >= m_unitQueue.size())
		return false;

	// find that unit in the build queue
	UnitQueueList::iterator it = m_unitQueue.begin();
	std::advance(it, index);

	// refund
	RDNPlayer *player = static_cast<RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());

	if (player != 0)
	{
		BuildRefund(player, *it);
	}

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitCancel(player, GetSelf()->GetEntity(), *it));

	// remove from build queue
	m_unitQueue.erase(it);

	//
	if (index == 0)
	{
		//
		m_unitInProgressTicks = 0;
		m_unitInProgressCount = 0;
		m_bWaitingForSpace = false;

		//
		UnitNext();
	}

	return true;
}

bool UnitSpawnerExt::BuildQueueRmvConstruct(size_t index)
{
	dbAssert(m_buildType == BT_Construct);

	// validate parm
	if (index >= (size_t)m_unitConstructing.size())
		return false;

	// find that unit in the build queue
	EntityGroup::iterator it = m_unitConstructing.begin();
	std::advance(it, index);

	Entity *pEntity = *it;
	dbAssert(pEntity != 0);

	// refund
	RDNPlayer *player = static_cast<RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());

	if (player != 0)
	{
		BuildRefund(player, pEntity->GetControllerBP());
	}

	// event
	GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitCancel(player, GetSelf()->GetEntity(), pEntity->GetControllerBP()));

	// remove from build queue
	m_unitConstructing.remove(pEntity);

	//
	OnUnitBuildQueueRmvConstruct(pEntity);

	//
	if (index == 0)
	{
		//
		m_unitInProgressTicks = 0;
		m_unitInProgressCount = 0;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: called for an Entity when it is removed from the construct build queue
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void UnitSpawnerExt::OnUnitBuildQueueRmvConstruct(Entity *pEntity)
{
	// remove from world
	ModObj::i()->GetWorld()->DeleteEntity(pEntity);
}

size_t UnitSpawnerExt::BuildQueueSize() const
{
	switch (m_buildType)
	{
	case BT_Spawn:
		return m_unitQueue.size();
	case BT_Construct:
		return m_unitConstructing.size();
	default:
		dbBreak();
		return 0;
	}
}

const ControllerBlueprint *UnitSpawnerExt::BuildQueueAt(size_t index) const
{
	switch (m_buildType)
	{
	case BT_Construct:
	{
		const ControllerBlueprint *cbp = 0;
		if (!m_unitConstructing.empty())
		{
			cbp = m_unitConstructing.front()->GetControllerBP();
		}
		return cbp;
	}
	case BT_Spawn:
	{

		// validate parm
		if (index >= m_unitQueue.size())
			return 0;

		// find that unit in the build queue
		UnitQueueList::const_iterator it = m_unitQueue.begin();
		std::advance(it, index);

		return *it;
	}
	default:
		dbBreak();
		return 0;
	}
}

std::pair<const ControllerBlueprint *, float> UnitSpawnerExt::UnitInProgress() const
{
	switch (m_buildType)
	{
	case BT_Spawn:
	{
		if (!m_unitQueue.empty())
		{
			const float progress = 1.0f - (float(m_unitInProgressTicks) / m_unitInProgressCount);
			return std::make_pair(m_unitQueue.front(), progress);
		}
		else
		{
			return std::make_pair((const ControllerBlueprint *)0, 0.0f);
		}
	}
	case BT_Construct:
	{
		if (!m_unitConstructing.empty())
		{
			const float progress = 1.0f - (float(m_unitInProgressTicks) / m_unitInProgressCount);
			return std::make_pair(m_unitConstructing.front()->GetControllerBP(), progress);
		}
		else
		{
			return std::make_pair((const ControllerBlueprint *)0, 0.0f);
		}
	}
	default:
	{
		// should never hit this
		dbBreak();
		return std::make_pair((const ControllerBlueprint *)0, 0.0f);
	}
	}
}

void UnitSpawnerExt::UnitBuild()
{
	// make sure the rally type is up-to-date
	if (UpdateRallyTarget())
	{
		RallyEvent(GetSelf()->GetEntity());
	}

	// pump it
	switch (m_buildType)
	{
	case BT_Spawn:
		// delegate
		UnitBuildSpawner();
		break;
	case BT_Construct:
		// delegate
		UnitBuildConstruct();
		break;
	default:
		// should never happen
		dbBreak();
		break;
	}

	return;
}

bool UnitSpawnerExt::TryToSpawn()
{
	// get unit in queue
	const ControllerBlueprint *cbp = m_unitQueue.front();

	// search for a spot
	Vec3f spawnPos;
	bool bGotPos = UnitSpawnPos(cbp, spawnPos);

	if (bGotPos)
	{
		// pop front of vector
		m_unitQueue.erase(m_unitQueue.begin());

		Entity *pEntity = UnitSpawn(cbp);

		GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitComplete(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), pEntity));

		// next
		UnitNext();

		// success
		return true;
	}

	// failed
	return false;
}

void UnitSpawnerExt::UnitBuildSpawner()
{
	dbAssert(m_buildType == BT_Spawn);

	if (m_unitQueue.empty())
		return;

	if (m_bWaitingForSpace)
	{
		// a previous spawn failed because there was no space for the entity

		long currentTime = ModObj::i()->GetWorld()->GetGameTicks();

		// is it time to search again?
		if (currentTime >= m_waitFinishTime)
		{
			// try to spawn again

			if (TryToSpawn())
			{
				// spawn succeeded
				m_bWaitingForSpace = false;
			}
			else
			{
				// spawn failed
				m_waitFinishTime = currentTime + 8 * 4;
			}
		}

		return;
	}

	// continue building
	dbAssert(m_unitInProgressTicks > 0);

	// is this the first tick of building
	if (m_unitInProgressTicks == m_unitInProgressCount)
	{
		// check pop cap
		if (IsAtUnitCap(GetSelf()->GetEntity()->GetOwner()))
			return;
	}

	//
	if (--m_unitInProgressTicks == 0)
	{
		// unit is ready

		if (!TryToSpawn())
		{
			// spawn failed
			m_bWaitingForSpace = true;

			// set retry time
			m_waitFinishTime = ModObj::i()->GetWorld()->GetGameTicks() + 8 * 4;

			// publish an event saying "blocked"
			GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitBlocked(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity()));
		}
	}
	else
	{
		// call this twice, the tick of construction and 1 second before spawning
		if (m_unitInProgressTicks == 8 || (m_unitInProgressTicks == (m_unitInProgressCount - 1)))
		{
			const ControllerBlueprint *cbp = m_unitQueue.front();

			// find spawn spot.
			Vec3f spawnPos;
			UnitSpawnPos(cbp, spawnPos);

			OnUnitSpawnLocation(spawnPos);
		}
	}

	return;
}

void UnitSpawnerExt::UnitBuildConstruct()
{
	dbAssert(m_buildType == BT_Construct);

	if (m_unitConstructing.empty())
		return;

	// continue building
	dbAssert(m_unitInProgressTicks > 0);

	// is this the first tick of building
	if (m_unitInProgressTicks == m_unitInProgressCount)
	{
		// check pop cap
		if (IsAtUnitCap(GetSelf()->GetEntity()->GetOwner()))
			return;
	}

	float percentage = 1.0f - (m_unitInProgressTicks / (float)m_unitInProgressCount);

	//
	OnUnitBuildByConstructing(m_unitConstructing.front(), percentage);

	//
	if (--m_unitInProgressTicks == 0)
	{
		// unit is ready
		Entity *pEntity = m_unitConstructing.front();

		m_unitConstructing.clear();

		OnUnitDoneByConstructing(pEntity);

		GameEventSys::Instance()->PublishEvent(GameEvent_BuildUnitComplete(static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner()), GetSelf()->GetEntity(), pEntity));
	}

	return;
}

void UnitSpawnerExt::UnitNext()
{
	// validate object state
	dbAssert(m_unitInProgressTicks == 0);

	if (m_unitQueue.empty())
		// no next unit
		return;

	// ticks needed
	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(m_unitQueue.front());

	m_unitInProgressCount =
			QIExtInfo<CostExtInfo>(si)->constructionTicks;

	dbAssert(m_unitInProgressCount > 0);

	// ticks current
	m_unitInProgressTicks = m_unitInProgressCount;

	OnUnitBuild(m_unitInProgressTicks);
}

Entity *UnitSpawnerExt::UnitSpawn(const ControllerBlueprint *cbp)
{
	m_unitInProgressTicks = 0;
	m_unitInProgressCount = 0;

	// find spawn spot.
	Vec3f spawnPos;
	UnitSpawnPos(cbp, spawnPos);

	// owner
	RDNPlayer *player = static_cast<RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());

	if (player == 0)
		// ???
		return 0;

	// create creature
	Entity *creature = ModObj::i()->GetEntityFactory()->CreateEntity(cbp, spawnPos.x, spawnPos.z);
	ModObj::i()->GetWorld()->SpawnEntity(creature);
	dbAssert(creature);

	player->AddEntity(creature);

	// delegate
	OnUnitSpawn(creature);

	// send the creature to it's initial position/order
	EntityGroup eg;
	eg.push_back(creature);

	Rally(eg);

	return creature;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool UnitSpawnerExt::UnitSpawnPos(const ControllerBlueprint *type, Vec3f &output) const
{
	const Entity *pEntity = GetSelf()->GetEntity();

	// check blueprint attributes
	const ECStaticInfo *si = ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(type);
	const MovingExtInfo *info = QIExtInfo<MovingExtInfo>(si);

	if (si && info && (info->IsGround() || info->IsSwimmer()))
	{
		// we have MoveExtInfo and its a ground unit
		TCMask movementMask = info->GetMovementMask();

		Vec3f wantedPosition = pEntity->GetPosition();

		Vec3f rallyPosition = wantedPosition;
		GetRallyPosition(rallyPosition);

		const PathfinderQuery *pQuery = ModObj::i()->GetWorld()->GetPathfinder()->Query();

		long cellX, cellZ, width, height;
		GetSelf()->GetEntityDynamics()->GetCellTopLeft(cellX, cellZ);
		GetSelf()->GetEntityDynamics()->GetCellWidthHeight(width, height);

		Vec2i minCell, maxCell;
		minCell.Set(cellX, cellZ);
		maxCell.Set(cellX + width - 1, cellZ + height - 1);

		/*
		bool bGotPosition = pQuery->GiveClosestFreePosition( Vec2f( wantedPosition.x, wantedPosition.z ), Vec2f( rallyPosition.x, rallyPosition.z ),
															 minCell, maxCell,
															 movementMask, 0, type->GetShrunkenOBB(), output );
*/
		bool bGotPosition = pQuery->GiveClosestFreePositionWithStraightLine(Vec2f(wantedPosition.x, wantedPosition.z), Vec2f(rallyPosition.x, rallyPosition.z),
																																				static_cast<const SimEntity *>(GetSelf()->GetEntity()),
																																				movementMask, 0, type->GetShrunkenOBB(), output);

		return bGotPosition;
	}

	output = pEntity->GetPosition();
	return true;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: sets the rally point at an entity
//	Result	:
//	Param.	: pEntity - the entity to rally to
//	Author	: dswinerd
//
void UnitSpawnerExt::SetRallyTarget(const Entity *pEntity)
{
	// validate params
	if (pEntity == 0)
	{
		dbBreak();
		return;
	}

	if (!pEntity->GetEntityFlag(EF_CanCollide))
	{
		// entity must be collidable to be a rally target
		return;
	}

	//
	m_rallyEntity.clear();
	m_rallyEntity.push_back(const_cast<Entity *>(pEntity));
	m_rallyLastSeen = pEntity->GetPosition();

	// evaluate the rally point type
	m_rallyType = ClassifyRallyEntity(m_rallyEntity.front());

	// force update
	UpdateRallyTarget();

	// game event
	RallyEvent(GetSelf()->GetEntity());

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: sets the rally point at a point
//	Result	:
//	Param.	: point - the point to rally to
//	Author	: dswinerd
//
void UnitSpawnerExt::SetRallyTarget(const Vec3f &point)
{
	//
	m_rallyType = RALLY_AtPoint;
	m_rallyPoint = point;
	m_rallyLastSeen = point;
	m_rallyEntity.clear();

	// force update
	UpdateRallyTarget();

	// game event
	RallyEvent(GetSelf()->GetEntity());

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines it the entity is visible
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool UnitSpawnerExt::IsEntityVisible(const Entity *pEntity) const
{
	// validate parm
	if (pEntity == 0)
		// this can happen if the entity is destroyed
		return false;

	// check owner
	const Player *pPlayer = GetSelf()->GetEntity()->GetOwner();

	if (pPlayer == 0)
		return false;

	//
	return (ModObj::i()->GetWorld()->IsEntityVisible(pPlayer, pEntity));
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: call every simstep to re-evaluate to rally point target
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool UnitSpawnerExt::UpdateRallyTarget()
{
	bool changed = false;

	//
	switch (m_rallyType)
	{
	case RALLY_NoPoint:
	case RALLY_AtPoint:
		// no updating necessary
		break;

	case RALLY_Structure:
	case RALLY_Creature:
		if (IsEntityVisible(m_rallyEntity.front()))
		{
			// update the last visible point
			m_rallyLastSeen = m_rallyEntity.front()->GetPosition();
		}
		else
		{
			// not visible anymore
			m_rallyType = RALLY_AtPoint;
			m_rallyPoint = m_rallyLastSeen;

			changed = true;
		}
		break;

	case RALLY_ResourceGather:
		// check if resource still exists
		if (m_rallyEntity.empty())
		{
			// doesn't exists anymore
			m_rallyType = RALLY_AtPoint;
			m_rallyPoint = m_rallyLastSeen;

			changed = true;
		}
		break;

	default:
		// huh?
		dbBreak();
	}

	return changed;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: given a rally target, determines what RallyType it is
//	Result	: changes m_rallyType
//	Param.	: pTarget - the Entity to classify
//	Author	: dswinerd
//
UnitSpawnerExt::RallyType UnitSpawnerExt::ClassifyRallyEntity(const Entity *pTarget)
{
	// validate parm
	if (pTarget == 0)
	{
		dbBreak();
		return RALLY_NoPoint;
	}

	//
	if (pTarget->GetControllerBP()->GetControllerType() == Henchmen_EC)
	{
		return RALLY_Creature;
	}
	else if (QIExtInfo<SiteExtInfo>(pTarget))
	{
		return RALLY_Structure;
	}

	// not a recognized rally target type
	return RALLY_NoPoint;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: given the rally target and the newly created pEntity, calls
//				the appropriate Rally function
//	Result	:
//	Param.	: eg - the EntityGroup containing the fresh entities
//	Author	: dswinerd
//
void UnitSpawnerExt::Rally(const EntityGroup &eg)
{
	// validate parm
	if (eg.size() != 1)
	{
		dbBreak();
		return;
	}

	// call the appropriate rally function
	switch (m_rallyType)
	{
	case RALLY_AtPoint:
		RallyAtPoint(eg);
		break;

	case RALLY_Structure:
		if (RDNQuery::IsFriend(m_rallyEntity.front(), GetSelf()->GetEntity()->GetOwner()))
			RallyAtFriendlyStructure(eg);
		else
			RallyAtEnemyStructure(eg);
		break;

	case RALLY_Creature:
		if (RDNQuery::IsFriend(m_rallyEntity.front(), GetSelf()->GetEntity()->GetOwner()))
			RallyAtFriendlyCreature(eg);
		else
			RallyAtEnemyCreature(eg);
		break;

	case RALLY_ResourceGather:
		RallyAtResourceGather(eg);
		break;

	case RALLY_NoPoint:
	default:
		RallyNoPoint(eg);
		break;
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: Rally to no point
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyNoPoint(const EntityGroup &eg)
{
	const unsigned long idEntity = eg.front()->GetID();
	const unsigned long idOwner =
			(GetSelf()->GetEntity()->GetOwner()) ? GetSelf()->GetEntity()->GetOwner()->GetID() : 0;

	ModObj::i()->GetWorld()->DoCommandEntity(
			CMD_Stop,
			0,
			false,
			idOwner,
			&idEntity,
			1);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: sends the EntityGroup to the rally point
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtPoint(const EntityGroup &eg)
{
	//
	const unsigned long idEntity = eg.front()->GetID();
	const unsigned long idOwner =
			(GetSelf()->GetEntity()->GetOwner()) ? GetSelf()->GetEntity()->GetOwner()->GetID() : 0;

	ModObj::i()->GetWorld()->DoCommandEntityPoint(
			CMD_DefaultAction,
			0,
			false,
			idOwner,
			&idEntity,
			1,
			&m_rallyPoint,
			1);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtEntityGeneric(const EntityGroup &eg, unsigned long command)
{
	//
	const unsigned long idEntity = eg.front()->GetID();
	const unsigned long idOwner =
			(GetSelf()->GetEntity()->GetOwner()) ? GetSelf()->GetEntity()->GetOwner()->GetID() : 0;

	const unsigned long idTarget = m_rallyEntity.front()->GetID();

	ModObj::i()->GetWorld()->DoCommandEntityEntity(
			command,
			0,
			false,
			idOwner,
			&idEntity,
			1,
			&idTarget,
			1);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at a friendly structure
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtFriendlyStructure(const EntityGroup &eg)
{
	if (eg.empty() || m_rallyEntity.empty())
	{
		return;
	}

	// default action on a friendly structure
	unsigned long command = CMD_Move;

	RallyAtEntityGeneric(eg, command);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at an enemy structure
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtEnemyStructure(const EntityGroup &eg)
{
	RallyAtEntityGeneric(eg, CMD_DefaultAction);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at a friendly creature
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtFriendlyCreature(const EntityGroup &eg)
{
	RallyAtEntityGeneric(eg, CMD_Move);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at a friendly creature and join that group
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtFriendlyCreatureGrouped(const EntityGroup &eg)
{
	RallyAtEntityGeneric(eg, CMD_Move);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at an enemy creature
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtEnemyCreature(const EntityGroup &eg)
{
	RallyAtEntityGeneric(eg, CMD_DefaultAction);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: rally the group at an gather resource pocket
//	Result	:
//	Param.	: eg - the group to rally
//	Author	: dswinerd
//
void UnitSpawnerExt::RallyAtResourceGather(const EntityGroup &eg)
{
	RallyAtEntityGeneric(eg, CMD_DefaultAction);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns the rally point type
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
UnitSpawnerExt::RallyType UnitSpawnerExt::GetRallyType() const
{
	// special case for anything with an entity
	if (m_rallyType == RALLY_Structure ||
			m_rallyType == RALLY_Creature ||
			m_rallyType == RALLY_ResourceGather)
	{
		if (m_rallyEntity.empty())
		{
			return RALLY_AtPoint;
		}
	}

	// default
	return m_rallyType;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: gets the rally point position, returns false if no rally point
//	Result	:
//	Param.	: position - the returned position
//	Author	: dswinerd
//
bool UnitSpawnerExt::GetRallyPosition(Vec3f &position) const
{
	switch (m_rallyType)
	{
	case RALLY_NoPoint:
		// no rally point
		return false;

	case RALLY_AtPoint:
		position = m_rallyPoint;
		return true;

	case RALLY_Structure:
	case RALLY_Creature:
	case RALLY_ResourceGather:
		// get the position of the rally entity
		position = m_rallyLastSeen;
		return true;

	default:
		dbBreak();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: return the entity rally target, if there is one
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
const Entity *UnitSpawnerExt::GetRallyTarget() const
{
	switch (m_rallyType)
	{
	case RALLY_NoPoint:
		// no rally point
		return 0;

	case RALLY_AtPoint:
		// not an entity
		return 0;

	case RALLY_Structure:
	case RALLY_Creature:
	case RALLY_ResourceGather:
		return m_rallyEntity.front();

	default:
		dbBreak();
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void UnitSpawnerExt::SaveExt(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	biff.StartChunk(Type_NormalVers, 'EUSP', "Extension: Unit Spawner", 3);

	unsigned long count = m_unitQueue.size();
	IFFWrite(iff, count);

	UnitQueueList::const_iterator iter = m_unitQueue.begin();
	UnitQueueList::const_iterator eiter = m_unitQueue.end();
	for (; iter != eiter; ++iter)
	{
		const ControllerBlueprint *pCBP = (*iter);

		IFFWrite(iff, pCBP->GetEBPNetworkID());
	}

	IFFWrite(iff, m_unitInProgressTicks);
	IFFWrite(iff, m_unitInProgressCount);

	unsigned long rally = m_rallyType;
	IFFWrite(iff, rally);

	IFFWrite(iff, m_rallyTheta);
	IFFWrite(iff, m_rallyPoint);
	IFFWrite(iff, m_rallyLastSeen);
	m_rallyEntity.SaveEmbedded(iff);

	// new at version 3
	m_unitConstructing.SaveEmbedded(iff);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void UnitSpawnerExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleEUSP, Type_NormalVers, 'EUSP', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long UnitSpawnerExt::HandleEUSP(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	unsigned long version = iff.GetNormalVersion();

	UnitSpawnerExt *pUnitSpawnerExt = static_cast<UnitSpawnerExt *>(pContext1);
	dbAssert(pUnitSpawnerExt);

	unsigned long count;
	IFFRead(iff, count);

	for (unsigned long i = 0; i != count; ++i)
	{
		unsigned long CBPID;
		IFFRead(iff, CBPID);

		const ControllerBlueprint *pCBP = ModObj::i()->GetEntityFactory()->GetControllerBP(CBPID);
		dbAssert(pCBP);
		pUnitSpawnerExt->m_unitQueue.push_back(pCBP);
	}

	IFFRead(iff, pUnitSpawnerExt->m_unitInProgressTicks);
	IFFRead(iff, pUnitSpawnerExt->m_unitInProgressCount);

	unsigned long rally;
	IFFRead(iff, rally);
	pUnitSpawnerExt->m_rallyType = (RallyType)rally;

	IFFRead(iff, pUnitSpawnerExt->m_rallyTheta);
	IFFRead(iff, pUnitSpawnerExt->m_rallyPoint);
	IFFRead(iff, pUnitSpawnerExt->m_rallyLastSeen);
	pUnitSpawnerExt->m_rallyEntity.LoadEmbedded(iff, ModObj::i()->GetEntityFactory());

	if (version >= 3)
	{
		pUnitSpawnerExt->m_unitConstructing.LoadEmbedded(iff, ModObj::i()->GetEntityFactory());

		EntityGroup::iterator ib = pUnitSpawnerExt->m_unitConstructing.begin();
		EntityGroup::iterator ie = pUnitSpawnerExt->m_unitConstructing.end();
		for (; ib != ie; ++ib)
		{
			// won't be in the pathfinding or spatial buckets until fully created
			ModObj::i()->GetWorld()->RemEntityPathfinding(*ib);
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void UnitSpawnerExt::OnUnitDoneByConstructing(Entity *)
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void UnitSpawnerExt::OnUnitBuildByConstructing(Entity *pEntity, float percentage)
{
	if (pEntity->GetAnimator())
	{
		pEntity->GetAnimator()->SetMotionVariable("Build", percentage);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: called when the spawner dies.
//	Author	: dswinerd
//
void UnitSpawnerExt::OnDead()
{
	EntityGroup::iterator it = m_unitConstructing.begin();

	for (; it != m_unitConstructing.end();)
	{
		Entity *pEntity = *it;

		it = m_unitConstructing.erase(it);

		ModObj::i()->GetWorld()->DeleteEntity(pEntity);
	}
}