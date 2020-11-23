/////////////////////////////////////////////////////////////////////
// File    : RDNPlayer.cpp
// Desc    :
// Created : Thursday, February 22, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNPlayer.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "RDNWorld.h"
#include "GameEventDefs.h"
#include "CommandTypes.h"
#include "RDNQuery.h"
#include "RDNTuning.h"
#include "PlayerFOW.h"
#include "RDNEBPs.h"

#include "Controllers/ModController.h"

#include "Extensions/ResourceExt.h"
#include "Extensions/SightExt.h"
#include "Extensions/HealthExt.h"
#include "Extensions/UnitSpawnerExt.h"
#include "Extensions/ResourceExt.h"
#include "Extensions/MovingExt.h"
#include "Extensions/AttackExt.h"

#include "ExtInfo/AttackExtInfo.h"
#include "ExtInfo/ResourceExtInfo.h"
#include "ExtInfo/CostExtInfo.h"
#include "ExtInfo/SiteExtInfo.h"

#include "Simulation/States/StateBuild.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <EngineAPI/ControllerBlueprint.h>
#include <EngineAPI/DecalInterface.h>

#include <ModInterface/ECStaticInfo.h>

#include "GameEventDefs.h"
#include "GameEventSys.h"

#include <Util/IFF.h>
#include <Util/IFFMath.h>

/////////////////////////////////////////////////////////////////////
//
namespace
{
	class FindAnyGuy : public FindClosestFilter
	{
	public:
		FindAnyGuy(const RDNPlayer *pPlayer)
				: m_pPlayer(pPlayer),
					m_bFound(false)
		{
		}

	private:
		virtual bool Check(const Entity *pEntity)
		{
			//	Quick abort if we have found at least one
			if (m_bFound)
				return false;

			//	Only check against this player
			if (pEntity->GetOwner() != m_pPlayer)
				return false;

			//	Only check for 'Guys'
			if (pEntity->GetControllerBP()->GetControllerType() != Henchmen_EC)
				return false;

			m_bFound = true;
			return true;
		}

		const RDNPlayer *m_pPlayer;
		bool m_bFound;
	};
}; // namespace

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
RDNPlayer::RDNPlayer(WorldFOW *pWorldFOW)
{
	//
	m_population = 0;

	for (int i = 0; i < MAX_EC; ++i)
	{
		m_groupController[i].ClearFlag(EF_IsSpawned);
	}

	m_pFOW = new PlayerFOW(pWorldFOW);

	// initialize lab position incase there never is a lab
	m_hqPosition = Vec3f(0, 0, 0);

	m_resourceCash = RDNTuning::Instance()->GetPlayerInfo().startingGatherRes;

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
RDNPlayer::~RDNPlayer()
{
	DELETEZERO(m_pFOW);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::SetName(const wchar_t *name)
{
	// delegate
	Player::SetName(name);

	// generate event
	GameEventSys::Instance()->PublishEvent(GameEvent_PlayerNameChanged(this));
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::AddEntity(Entity *e)
{
	// inherited
	Player::AddEntity(e);

	int ctype = e->GetControllerBP()->GetControllerType();

	dbTracef("Adding entity %d", ctype);

	// add this entity to this controller group
	if (ctype < MAX_EC)
		m_groupController[ctype].push_back(e);

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnAddEntity(this, e);
	}

	switch (ctype)
	{
	case Lab_EC:
		// save starting position
		m_hqPosition = e->GetPosition();
		return;

	case Henchmen_EC:
		++m_population;
		return;

	case ResourceRenew_EC:
	case RemoteChamber_EC:
	case WaterChamber_EC:
	case Aviary_EC:
	case ElectricGenerator_EC:
	case BrambleFence_EC:
	case Foundry_EC:
	case SoundBeamTower_EC:
		dbTracef("RDNPlayer::AddEntity can handle extra steps for buildings later");
		return;
	}

	dbFatalf("RDNPlayer::AddEntity do not know how to add entty %d", ctype);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::RemoveEntity(Entity *e)
{
	//
	int ctype = e->GetControllerBP()->GetControllerType();
	if (ctype < MAX_EC)
		m_groupController[ctype].remove(e);

	switch (ctype)
	{
	case Henchmen_EC:
		--m_population;
		break;
	}

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnRemoveEntity(this, e);
	}

	// remove this entity from my FOW
	m_pFOW->RemoveFromFOW(e);

	// inherited
	// NOTE: this must be called last
	Player::RemoveEntity(e);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
size_t RDNPlayer::GetNumEntity(int type) const
{
	dbAssert(type >= 0 && type < MAX_EC);
	return m_groupController[type].size();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
size_t RDNPlayer::GetNumEntityTotal(int type) const
{
	dbAssert(type >= 0 && type < MAX_EC);
	return m_groupController[type].size();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
size_t RDNPlayer::GetNumLiveEntity(int type) const
{
	dbAssert(type >= 0 && type < MAX_EC);
	size_t count = m_groupController[type].size();

	if (count > 0)
	{
		EntityGroup::const_iterator ei = m_groupController[type].begin();
		EntityGroup::const_iterator ee = m_groupController[type].end();

		// ignore entities that are dead or have zero health
		for (; ei != ee; ei++)
		{
			const HealthExt *healthExt = QIExt<HealthExt>(*ei);
			if (!healthExt || (healthExt->GetHealth() <= 0.0f))
			{
				count--;
			}
		}
	}

	return count;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
bool RDNPlayer::CanControlEntity(const Entity *pEntity) const
{
	if (pEntity == 0)
		return false;

	return pEntity->GetOwner() == this;
}

////////////////////////////////////////////////////////////////////////////////
//	Desc.	: This tests to see if we can build this blueprint
//			  based on many factors ( resources, pop limit, research, ...)
//	Result	:
//	Param.	:
//	Author	:
//
RDNPlayer::BuildResult RDNPlayer::BlueprintCanBuild(const ControllerBlueprint *cbp) const
{
	// validate parm
	if (cbp == 0)
	{
		dbFatalf("RDNPlayer::BlueprintCanBuild cbp id 0");
		return BR_Never;
	}

	// check cost
	// NOTE: cost should be checked LAST
	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

	if (cost != 0)
	{
		if (GetResourceCash() < (cost->costCash))
			return BR_NeedResourceCash;
	}

	return BR_AllowBuild;
}

const EntityGroup &RDNPlayer::GetEntityGroup(int type) const
{
	dbAssert(type >= 0 && type < MAX_EC);

	return m_groupController[type];
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::PreFirstSimulate()
{
	// inherited
	Player::PreFirstSimulate();

	// refresh the FOW status
	GetFogOfWar()->RefreshWorldFOW();

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
float RDNPlayer::GetResourceCash() const
{
	return m_resourceCash;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
float RDNPlayer::DecResourceCash(float amount)
{
	// validate parm
	dbAssert(amount >= 0);

	// clip
	if (amount > m_resourceCash)
		amount = m_resourceCash;

	//
	m_resourceCash -= amount;
	dbAssert(m_resourceCash >= 0);

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnDecResourceCash(this, amount);
	}

	return m_resourceCash;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
float RDNPlayer::IncResourceCash(float amount, ResourceIncreased reason)
{
	// validate parm
	dbAssert(amount >= 0 && amount < 100000.0f);

	//
	m_resourceCash += amount;
	dbAssert(m_resourceCash >= 0);

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnIncResourceCash(this, amount, reason);
	}

	return m_resourceCash;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
const Entity *RDNPlayer::GetLabEntity() const
{
	return const_cast<RDNPlayer *>(this)->GetLabEntity();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
Entity *RDNPlayer::GetLabEntity()
{
	if (m_groupController[Lab_EC].size() == 0)
		return 0;

	return *m_groupController[Lab_EC].begin();
}

const Vec3f &RDNPlayer::GetStartingPosition() const
{
	// if the lab is dead return the position of where it was
	if (GetLabEntity() == NULL)
		return m_hqPosition;

	return GetLabEntity()->GetPosition();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
bool RDNPlayer::FoWIsVisible(const Entity *entity) const
{
	return RDNQuery::CanBeSeen(entity, this);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
bool RDNPlayer::FoWIsVisible(const Player *player, const ControllerBlueprint *pCBP, const Matrix43f &transform) const
{
	if (player == this)
		return false;

	const PlayerFOW *pFoW = GetFogOfWar();

	return pFoW->IsVisible(pCBP, transform);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::FoWUpdate(const RDNWorld *)
{
	// update fogmap
	if (!IsPlayerDead())
	{
		GetFogOfWar()->Update(GetEntities());
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::CommandDoProcess(
		const unsigned int cmd,
		const unsigned long param,
		const unsigned int flags,
		Player *sender)
{
	dbTracef("RDNPlayer::CommandDoProcess");

	UNREF_P(flags);

	switch (cmd)
	{
	case PCMD_CheatCash:
		return CmdCheatCash(sender, param);

	case PCMD_CheatKillSelf:
		return CmdCheatKillSelf(sender);
	}

	dbFatalf("RDNPLayer::CommandDoProcess unhandled PlayerPlayer case %d", cmd);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::CommandDoProcess(
		const unsigned int cmd,
		const unsigned long param,
		const unsigned int flags,
		Player *sender,
		const EntityGroup &entities,
		const Vec3f *pos,
		const size_t posCount)
{
	// no commands
	UNREF_P(cmd);
	UNREF_P(param);
	UNREF_P(flags);
	UNREF_P(sender);
	UNREF_P(entities);
	UNREF_P(pos);
	UNREF_P(posCount);

	switch (cmd)
	{
	case CMD_BuildBuilding:
		if (!param)
			dbFatalf("RDNPlayer::CommandDoProcess CMD_BuildBuilding no ebpid given");

		return CmdBuildBuilding(sender, param, entities, pos);

	default:
		dbFatalf("RDNPLayer::CommandDoProcess unhandled PlayerPlayer case %d", cmd);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::Save(IFF &iff) const
{
	// Call the Base Class First
	Player::Save(iff);

	//
	iff.PushChunk(Type_Form, 'SPFC', 1);

	iff.PushChunk(Type_NormalVers, 'SPDT', 1);

	IFFWrite(iff, m_resourceCash);
	IFFWrite(iff, m_hqPosition);

	iff.PopChunk();

	GetFogOfWar()->Save(iff);

	iff.PopChunk();

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::Load(IFF &iff)
{
	// Call the Base Class First
	Player::Load(iff);

	iff.AddParseHandler(HandleSPFC, Type_Form, 'SPFC', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long RDNPlayer::HandleSPFC(IFF &iff, ChunkNode *, void *pContext1, void *pContext2)
{
	RDNPlayer *pPlayer = static_cast<RDNPlayer *>(pContext1);

	iff.AddParseHandler(HandleSPDT, Type_NormalVers, 'SPDT', pContext1, pContext2);

	pPlayer->GetFogOfWar()->Load(iff);

	return iff.Parse();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long RDNPlayer::HandleSPDT(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	RDNPlayer *pPlayer = static_cast<RDNPlayer *>(pContext1);

	IFFRead(iff, pPlayer->m_resourceCash);
	IFFRead(iff, pPlayer->m_hqPosition);

	return 0;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: determines how many of the given type of buildings we can afford
//	Result	: returns the number of buildings we can afford
//	Param.	: cbp - the ControllerBlueprint of the building
//	Author	: dswinerd
//
int RDNPlayer::GetStructureBudget(const ControllerBlueprint *cbp) const
{
	//
	const ECStaticInfo *si = ModObj::i()->GetEntityFactory()->GetECStaticInfo(cbp);

	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

	if (cost == 0)
	{
		return 0;
	}

	// determines how many segments we can afford
	int budget = INT_MAX;

	if (cost->costCash > 0)
	{
		budget = (int)(GetResourceCash() / (cost->costCash));
	}

	return budget;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
int RDNPlayer::PopulationCurrent() const
{
	return m_population;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
int RDNPlayer::PopulationMax() const
{
	return RDNDllSetup::Instance()->GetUnitCap();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
int RDNPlayer::PopulationTotal() const
{
	// count unit in construction
	EntityGroup::const_iterator i = GetEntities().begin();
	EntityGroup::const_iterator e = GetEntities().end();

	int popInConstruction = 0;

	for (; i != e; ++i)
	{
		const UnitSpawnerExt *spawn = QIExt<UnitSpawnerExt>(*i);

		if (spawn)
		{
			if (spawn->UnitInProgress().second > 0.0f)
				popInConstruction++;
		}
	}

	return PopulationCurrent() + popInConstruction;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::SpawnEntity(Entity *pEntity)
{
	UNREF_P(pEntity);

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnReSpawnEntity(this, pEntity);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::DeSpawnEntity(Entity *pEntity)
{
	m_pFOW->RemoveFromFOW(pEntity);

	// observers
	PlayerObserverList::iterator oi = m_observers.begin();
	PlayerObserverList::iterator oe = m_observers.end();

	for (; oi != oe; ++oi)
	{
		(*oi)->OnDeSpawnEntity(this, pEntity);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
PlayerFOW *RDNPlayer::GetFogOfWar()
{
	return m_pFOW;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNPlayer::KillPlayer(int reason)
{
	// validate parm
	dbAssert(reason >= 0 && reason < KPR_COUNT);

	// debug spew
	if (reason != KPR_UnusedPlayer)
	{
		const RDNWorld *w =
				ModObj::i()->GetWorld();

		dbTracef("MOD -- Player %S has been killed (frame %d)", GetName(), w->GetGameTicks());
	}

	// inherited
	Player::KillPlayer(reason);

	// game event
	if (reason != KPR_UnusedPlayer)
	{
		GameEventSys::Instance()->PublishEvent(GameEvent_PlayerKilled(this, reason));
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : called when we have successfully tagged an enemy creature.
//				Used to update FoW
// Result    : e added to m_taggedCreatures
// Param.    : e - the tagged enemy
// Author    : dswinerd
//
void RDNPlayer::AddTaggedEntity(Entity *e)
{
	GetFogOfWar()->AddTaggedEntity(e);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: call this when the tag has been removed from a creature we tagged
//	Result	: e removed from m_taggedCreatures
//	Param.	: e - the untagged enemy
//	Author	: dswinerd
//
void RDNPlayer::RemoveTaggedEntity(Entity *e)
{
	GetFogOfWar()->RemTaggedEntity(e);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::CmdCheatCash(Player *sender, unsigned long n)
{
	UNREF_P(sender);

	// validate cheat
	if (RDNDllSetup::Instance()->GetCheats() != RDNDllSetup::CHT_Yes)
		return;

	// validate parm
	if (n <= 0 || n > 65535)
		return;

	// apply cheat
	IncResourceCash(float(n));

	// debug spew
	dbTracef("MOD -- (CHEAT) player %d added %d cash", GetID(), n);

	//
	GameEventSys::Instance()->PublishEvent(GameEvent_PlayerCheat(this));

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNPlayer::CmdCheatKillSelf(Player *sender)
{
	dbAssert(sender == this);

	// validate cheat
	if (RDNDllSetup::Instance()->GetCheats() != RDNDllSetup::CHT_Yes)
		return;

	//
	dbTracef("MOD -- (CHEAT) player %d killed itself", GetID());

	//
	GameEventSys::Instance()->PublishEvent(GameEvent_PlayerCheat(this));

	// send a self-destroy command to everybody
	EntityGroup g;
	g.SetFlag(EF_IsSpawned);
	g = GetEntities();

	WorldDoCommandEntity(
			ModObj::i()->GetWorld(),
			CMD_Destroy,
			0,
			0,
			GetID(),
			g);

	return;
}

void RDNPlayer::CmdBuildBuilding(Player *sender, long ebpid, const EntityGroup &entities, const Vec3f *pos)
{
	dbTracef("RDNPlayer::CmdBuildBuilding");
	if (sender != this)
		dbFatalf("RDNPlayer::CmdBuildBuilding command came from an unexpected player");

	const ControllerBlueprint *pCbp = ModObj::i()->GetEntityFactory()->GetControllerBP(ebpid);
	if (!pCbp)
		dbFatalf("RDNPlayer::CmdBuildBuilding Controller blueprint not found for EBP network ID (%d)", ebpid);

	Entity *building = ModObj::i()->GetEntityFactory()->CreateEntity(pCbp, pos->x, pos->z);

	Vec3f bestPosition;
	bool bGotPosition = ModObj::i()->GetWorld()->GetPathfinder()->Query()->GiveClosestFreePosition(
			Vec2f(pos->x, pos->z),
			static_cast<const SimEntity *>(building),
			0,
			bestPosition);

	if (!bGotPosition)
		dbFatalf("Could not find a place to put the buildling :(");

	Matrix43f m = building->GetTransform();
	m.T = bestPosition;

	building->SetTransform(m);
	building->SetPrevTransform(m);
	building->SetPrevPrevTransform(m);
	building->SetEntityFlag(EF_IsVisible);
	building->SetEntityFlag(EF_CanCollide);

	AddEntity(building);
	ModObj::i()->GetWorld()->DoSpawnEntity(building);

	// TODO: move to buildingController onspawnentity
	building->GetAnimator()->SetMotionVariable("Build", 100);

	EntityGroup::const_iterator henchmanIter = entities.begin();
	for (; henchmanIter != entities.end(); henchmanIter++)
	{
		ModController *henchmanController = static_cast<ModController *>((*henchmanIter)->GetController());

		StateBuild *pStateBuild = static_cast<StateBuild *>(henchmanController->QIStateAll(StateBuild::StateID));
		if (!pStateBuild)
			dbFatalf("RDNPlayer::CmdBuildBuilding one of the selected entities does not have state build");

		pStateBuild->Enter(building);
		henchmanController->SetActiveState(StateBuild::StateID);
	}

	// dbTracef("Trying to attach the decal");
	// building->GetAnimator()->AttachDecal("build_con", 10, true, true);

	// ModObj::i()->GetDecalInterface()->AddDecal("build_con.tga", bestPosition.x, bestPosition.z, 1, 0);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
void RDNPlayer::AddObserver(Observer *p) const
{
	// validate parm
	dbAssert(p != NULL);
	dbAssert(m_observers.find(p) == m_observers.end());

	// add
	m_observers.insert(p);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
void RDNPlayer::RemoveObserver(Observer *p) const
{
	// validate parm
	dbAssert(p != NULL);

	// erase
	PlayerObserverList::iterator found = m_observers.find(p);
	dbAssert(found != m_observers.end());

	m_observers.erase(found);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
void RDNPlayer::Update()
{
}
