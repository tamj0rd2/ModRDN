/////////////////////////////////////////////////////////////////////
// File    : RDNWorld.cpp
// Desc    :
// Created : Friday, February 23, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "RDNWorld.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "RDNQuery.h"
#include "RDNPlayer.h"
#include "RDNEBPs.h"
#include "RDNTuning.h"
#include "CommandTypes.h"
#include "WorldFOW.h"
#include "PlayerFOW.h"
#include "GameEventSys.h"
#include "GameEventDefs.h"
#include "UnitConversion.h"

#include "Controllers/ControllerTypes.h"
#include "Controllers/ModController.h"

#include "Extensions/HealthExt.h"
#include "Extensions/UnitSpawnerExt.h"

#include "../UI/RDNText.h"

#include "../Stats/RDNStats.h"

#include <EngineAPI/SimEngineInterface.h>
#include <EngineAPI/ControllerBlueprint.h>

#include <SimEngine/GroupController.h>
#include <SimEngine/EntityCommand.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/GroundDynamics.h> // to get access to impassmap
#include <SimEngine/FlyingDynamics.h>
#include <SimEngine/BuildingDynamics.h> // to get access to BuildingDynamics static helper functions
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>
#include <SimEngine/Pathfinding/Impassmap.h>
#include <SimEngine/Pathfinding/PreciseTerrainMap.h>

#include <SimEngine/SimWorld.h>

#include <SurfVol/Hull2D.h>

#include <Filesystem/CRC.h>

#include <Util/DebugRender.h>
#include <Util/PerfBlock.h>
#include <Util/Biff.h>
#include <Util/Iff.H>
#include <Util/LogFile.h>
#include <Util/BitPlane.h>

/////////////////////////////////////////////////////////////////////
//

namespace
{
	// Size of the spatial buckets in meters
	const long k_SpatialBucketSize = 16;
} // namespace

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
class RDNWorld::Data
{
public:
	static RDNWorld *s_instance;

public:
	bool m_simulated;
	bool m_bEnableFowUpdate;

	WorldFOW *m_pWorldFOW;

	bool m_missionEdFlag;

	std::smallvector<Vec3f, 6> m_playerSlots;

	// statgraph & performance
	PerfBlock m_timingState;

	long m_playerIDWon; // the player id of the player who won. -1 for no one.
											// assumes only one player can win (no support for teams)

public:
	Data()
			: m_timingState("simstates")
	{
	}
};

RDNWorld *RDNWorld::Data::s_instance = 0;

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
RDNWorld::RDNWorld(SimEngineInterface *pSimInt, bool missionEdFlag)
		: SimWorld(pSimInt, k_SpatialBucketSize),
			m_pimpl(new Data)
{
	SetSimsPerSecond(k_SimStepsPerSecond);

	m_pimpl->m_simulated = false;
	m_pimpl->m_bEnableFowUpdate = true;

	m_pimpl->m_pWorldFOW = NULL;

	m_pimpl->m_missionEdFlag = missionEdFlag;

	m_pimpl->m_playerIDWon = -1;

	// singleton
	RDNWorld::Data::s_instance = this;

	RDNStats::Instance()->Reset();

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
RDNWorld::~RDNWorld()
{
	// ensure all objects are removed from pathfinding.  Needs to come BEFORE the Entities are deleted.
	RemAllPathfinding();

	// singleton
	RDNWorld::Data::s_instance = 0;

	// Update the WorldFOW state based on the current ref counts of all the
	for (size_t pi = 0; pi != GetPlayerCount(); ++pi)
	{
		RDNPlayer *pPlayer = static_cast<RDNPlayer *>(GetPlayerAt(pi));

		pPlayer->GetFogOfWar()->ShutdownFOW();
	}

	//
	DELETEZERO(m_pimpl->m_pWorldFOW);
	DELETEZERO(m_pimpl);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNWorld *RDNWorld::GetWorld()
{
	dbAssert(RDNWorld::Data::s_instance != 0);
	return RDNWorld::Data::s_instance;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::CreateWorld(SimEngineInterface *p, bool missionEd)
{
	// check for duplicate instance
	dbAssert(RDNWorld::Data::s_instance == 0);

	// create it
	RDNWorld *dummy = new RDNWorld(p, missionEd);

	// ignore return value
	UNREF_P(dummy);

	// make sure it got created correctly
	dbAssert(RDNWorld::Data::s_instance != 0);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::SimulatePre()
{
	dbAssert(m_pimpl->m_simulated == 0);

	// flag
	m_pimpl->m_simulated = true;

	// special things to do when the game first starts
	if (!m_pimpl->m_missionEdFlag && (GetGameTicks() == 0))
	{
		// nuke all unneeded players
		PreRemoveEmptyPlayers();
		PreSetPlayerInfo();
	}

	// inherited
	SimWorld::SimulatePre();

	// preload all EBPs to make sure the network ids are sync'ed
	PreloadEBPs();

	// fow
	UpdateFoW();

	if (!m_pimpl->m_missionEdFlag)
	{
		// stats layer
		RDNStats::Instance()->RecordStart(this);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
//
void RDNWorld::SimulatePost()
{
	// inherited
	SimWorld::SimulatePost();

	if (!m_pimpl->m_missionEdFlag)
	{
		// gather all final stats
		RDNStats::Instance()->RecordStop(this);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::Simulate()
{
	// game rules
	if (!m_pimpl->m_missionEdFlag)
	{
		// evaluate the game rules until the game is over
		if (!IsGameOver())
		{
			// iterate through all the players and check:
			// 1) is anyone alive who has no hq? kill them.
			// 2) are all players but one dead? the game is over.
			size_t numPlayers = GetPlayerCount();
			size_t numPlayersAlive = 0;
			for (size_t i = 0; i < numPlayers; i++)
			{
				RDNPlayer *pPlayer = static_cast<RDNPlayer *>(GetPlayerAt(i));
				if (!pPlayer->IsPlayerDead())
				{
					const EntityGroup &eg = pPlayer->GetEntityGroup(Lab_EC);
					if (eg.empty())
					{
						// no hq, so kill this player
						pPlayer->KillPlayer(RDNPlayer::KPR_Trigger);
					}
					else
					{
						// not dead and not being killed, so alive
						numPlayersAlive++;
						m_pimpl->m_playerIDWon = pPlayer->GetID();
					}
				}
			}
			if (numPlayersAlive > 1)
			{
				// no one won yet.
				m_pimpl->m_playerIDWon = -1;

				// publish start game event if this is the first sim step
				if (GetGameTicks() == 0L)
				{
					GameEventSys::Instance()->PublishEvent(GameEvent_GameStart());
				}
			}
			else
			{
				// the game is over.
				SetGameOver();
			}
		}
	}

	m_pimpl->m_timingState.Start();
	m_pimpl->m_timingState.Pause();

	// this "simulate" steps forward a time step for the simulation.
	SimWorld::Simulate();

	m_pimpl->m_timingState.Resume();
	m_pimpl->m_timingState.Stop();

	// update each player so they can do whatever processing they want
	size_t pi = 0;
	size_t pe = GetPlayerCount();

	for (; pi != pe; ++pi)
	{
		static_cast<RDNPlayer *>(GetPlayerAt(pi))->Update();
	}

	// fow
	UpdateFoW();

	// records stats for each player
	RDNStats::Instance()->RecordFrame(this);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::SetTerrain(TerrainHMBase *pTerrain)
{
	SimWorld::SetTerrain(pTerrain);

	m_pimpl->m_pWorldFOW = new WorldFOW(pTerrain->GetIslandWidth(), pTerrain->GetIslandLength());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNWorld::IsEntityVisible(const Player *localPlayer, const Entity *entity) const
{
	// validate parm
	if (entity == 0)
	{
		dbBreak();
		return false;
	}

	// when no local player is set, everything is visible
	if (localPlayer == 0 || localPlayer->IsPlayerDead())
		return true;

	//
	return static_cast<const RDNPlayer *>(localPlayer)->FoWIsVisible(entity);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::GetSyncToken(std::vector<std::pair<unsigned long, const char *> > &crcArray) const
{
	// inherited
	SimWorld::GetSyncToken(crcArray);

#if !defined(RELIC_RTM)

	// crc player info
	CRC playerResCash;
	CRC playerFow;

	size_t pi = 0;
	size_t pe = GetPlayerCount();

	for (; pi != pe; ++pi)
	{
		const RDNPlayer *player = static_cast<const RDNPlayer *>(GetPlayerAt(pi));
		playerResCash.AddVar(player->GetResourceCash());
		playerFow.AddVar(player->GetFogOfWar()->GetSyncToken());
	}

	// states & extensions
	CRC crcState;
	CRC crcBuilding;
	CRC crcResearch;
	CRC crcHealth;
	CRC crcUnitSpawn;

	World::EntityControllerList::const_iterator ci = GetEntityControllerList().begin();
	World::EntityControllerList::const_iterator ce = GetEntityControllerList().end();

	for (; ci != ce; ++ci)
	{
		// state
		const State *state = static_cast<const ModController *>(*ci)->QIActiveState();
		crcState.AddVar(state ? state->GetStateID() : 0);

		// extensions
		const HealthExt *health = QIExt<HealthExt>(*ci);
		if (health)
		{
			crcHealth.AddVar(health->GetHealth());
		}

		const UnitSpawnerExt *unitspawn = QIExt<UnitSpawnerExt>(*ci);
		if (unitspawn)
		{
			crcUnitSpawn.AddVar(unitspawn->BuildQueueSize());

			const std::pair<const ControllerBlueprint *, float> r = unitspawn->UnitInProgress();

			crcUnitSpawn.AddVar(r.second);
			crcUnitSpawn.AddVar(r.first == 0 ? 0 : r.first->GetEBPNetworkID());
		}
	}

	// dynamics
	CRC crcDynamics;

	World::EntityControllerList::const_iterator di = GetEntityControllerList().begin();
	World::EntityControllerList::const_iterator de = GetEntityControllerList().end();

	for (; di != de; ++di)
	{
		const EntityDynamics *dynamics = static_cast<const ModController *>(*di)->GetEntityDynamics();

		if (dynamics != 0)
		{
			crcDynamics.AddVar(dynamics->GetDynamicsID());
			crcDynamics.AddVar(dynamics->QueryStatus());

			const Target *target = dynamics->GetTarget();
			crcDynamics.AddVar(target ? target->GetType() : 0);
		}
	}

	//
	crcArray.push_back(std::make_pair(playerResCash.GetCRC(), (const char *)"player res cash"));
	crcArray.push_back(std::make_pair(playerFow.GetCRC(), (const char *)"player fow"));

	crcArray.push_back(std::make_pair(crcBuilding.GetCRC(), (const char *)"building ticks"));
	crcArray.push_back(std::make_pair(crcResearch.GetCRC(), (const char *)"research"));
	crcArray.push_back(std::make_pair(crcHealth.GetCRC(), (const char *)"health"));
	crcArray.push_back(std::make_pair(crcUnitSpawn.GetCRC(), (const char *)"unit spawner"));

	crcArray.push_back(std::make_pair(crcDynamics.GetCRC(), (const char *)"dynamics"));

	crcArray.push_back(std::make_pair(crcState.GetCRC(), (const char *)"state"));

#else

	CRC RDNwCRC;

	size_t pi = 0;
	size_t pe = GetPlayerCount();

	for (; pi != pe; ++pi)
	{
		const RDNPlayer *player = static_cast<const RDNPlayer *>(GetPlayerAt(pi));

		RDNwCRC.AddVar(player->GetResourceCash());
	}

	crcArray.push_back(std::make_pair(RDNwCRC.GetCRC(), (const char *)"RDN world"));

#endif

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Param.    :
//
void RDNWorld::SyncLogEntity(LogFile &logfile, const Entity *pEntity) const
{
	// inherited first
	SimWorld::SyncLogEntity(logfile, pEntity);

	// controller information
	const ModController *pMC = static_cast<const ModController *>(pEntity->GetController());

	if (pMC != NULL)
	{
		// state
		const State *pState = pMC->QIActiveState();

		if (pState != NULL)
		{
			//
			char logmessage[1024];
			logmessage[0] = '\0';

			sprintf(logmessage + strlen(logmessage), "\tstate = %02d, stateExit = %d",
							pState->GetStateID(),
							pState->IsExiting() ? 1 : 0);

			//
			logfile.puts(logmessage);
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : does the work of the SpawnEntity call
// Param.    : pEntity - the entity to spawn
//
void RDNWorld::SpawnEntityInternal(Entity *pEntity)
{
	// validate parm
	if (pEntity == 0)
	{
		dbFatalf("MOD -- Spawning a null entity");
	}

	// validate that the Entity is being spawned inside the world
	if (pEntity->GetController())
	{
		const SimController *pSimController = static_cast<const SimController *>(pEntity->GetController());
		if (pSimController->GetEntityDynamics())
		{
			if (!pSimController->GetEntityDynamics()->IsInsideWorld(&pEntity->GetTransform()))
			{
				dbWarningf('SMOD', "(%g, %g): MOD -- Spawning an entity with extents outside the world. '%d'", pEntity->GetPosition().x, pEntity->GetPosition().z, pEntity->GetID());
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::DoSpawnEntity(Entity *pEntity)
{
	// delegate
	SpawnEntityInternal(pEntity);

	// call base class
	SimWorld::DoSpawnEntity(pEntity);

	// tell the owner
	if (pEntity->GetOwner())
	{
		RDNPlayer *pOwner = static_cast<RDNPlayer *>(pEntity->GetOwner());

		pOwner->SpawnEntity(pEntity);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::DoRestoreEntity(Entity *pEntity)
{
	SpawnEntityInternal(pEntity);

	// call the base class
	SimWorld::DoRestoreEntity(pEntity);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::DeSpawnEntity(Entity *pEntity)
{
	SimWorld::DeSpawnEntity(pEntity);

	if (pEntity->GetOwner())
	{
		RDNPlayer *pOwner = static_cast<RDNPlayer *>(pEntity->GetOwner());

		pOwner->DeSpawnEntity(pEntity);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNWorld::CBPHasReference(long CBPNetID) const
{
	if (SimWorld::CBPHasReference(CBPNetID))
		return true;

	long count = GetPlayerCount();

	for (long i = 0; i < count; ++i)
	{
		const RDNPlayer *pRDNPlayer = static_cast<const RDNPlayer *>(GetPlayerAt(i));

		for (unsigned long j = 0; j < pRDNPlayer->GetArmy().size(); ++j)
		{
			if (CBPNetID == pRDNPlayer->GetArmy()[j])
			{
				return true;
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
bool RDNWorld::QueryRelationship(const Player *p1, const Player *p2, PlayerRelationship query) const
{
	switch (query)
	{
	//
	case World::PR_Enemy:
		return p1 != p2;
	//
	case World::PR_Ally:
		return p1 == p2;
	//
	case World::PR_Neutral:
		return p1 == p2;
	//
	default:
		// should never hit this
		dbBreak();
	}

	return p1 == p2;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: calculates the odd size ground entities will use for pathfinding.  Should be overridden by the mod.
//	Result	: the integer pathfinding size of the entity.  MUST BE ODD.
//	Param.	: xscale - the length of the entity in the x dimension
//			  zscale - the length of the entity in the z dimension
//	Author	: dswinerd
//
int RDNWorld::CalculatePathfindingSize(float xscale, float zscale) const
{

	// temporary : change when henchman have obbs that aren't so wide.
	// x can't be bigger than z
	xscale = std::min(xscale, zscale);

	float scaleMax = std::max(xscale, zscale);
	float scaleMin = std::min(xscale, zscale);

	int intSize = 1;

	if (scaleMax < 1.25f)
	{
		intSize = 1;
	}
	else
	{
		// size 3 or 5
		if (scaleMin > 4.0)
		{
			intSize = 5;
		}
		else
		{
			intSize = 3;
		}
	}

	// intSize MUST be odd
	dbAssert((intSize & 1) == 1);

	return intSize;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::Save(BiFF &biff) const
{
	// inherited
	SimWorld::Save(biff);

	IFF &iff = *biff.GetIFF();

	//
	m_pimpl->m_pWorldFOW->Save(iff);

	RDNStats::Instance()->Save(*biff.GetIFF());

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::Load(IFF &iff)
{
	// inherited
	SimWorld::Load(iff);

	//
	m_pimpl->m_pWorldFOW->Load(iff);

	RDNStats::Instance()->Load(iff);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::GenerateWarnings() const
{
	// call base
	World::GenerateWarnings();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::PreLoadEBPS()
{
	// mod ebp
	RDNEBP::Preload();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::PreloadEBPs()
{
	// players ebp
	size_t pi = 0;
	size_t pe = GetPlayerCount();

	for (; pi != pe; ++pi)
	{
		const std::vector<long> &army = static_cast<RDNPlayer *>(GetPlayerAt(pi))->GetArmy();

		std::vector<long>::const_iterator ai = army.begin();
		std::vector<long>::const_iterator ae = army.end();

		for (; ai != ae; ++ai)
		{
			// this forces loading the static info
			const ECStaticInfo *si = GetEntityFactory()->GetECStaticInfo(*ai);
			si = 0;
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::SetGameOver()
{
	if (!IsGameOver())
	{
		// debug spew
		dbTracef("MOD -- Game Over at frame %d", GetGameTicks());

		// delegate
		SimWorld::SetGameOver();

		// publish event
		GameEventSys::Instance()->PublishEvent(GameEvent_GameOver());
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void RDNWorld::UpdateFoW()
{
	// quick out
	if (!m_pimpl->m_bEnableFowUpdate)
		return;

	// let each player get a piece of the action
	size_t pi = 0;
	size_t pe = GetPlayerCount();

	for (; pi != pe; ++pi)
	{
		static_cast<RDNPlayer *>(GetPlayerAt(pi))->FoWUpdate(this);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::ClampPointToWorld(Vec3f &point) const
{
	// clamp to the bounds of the island
	float islandWidth = GetTerrain()->GetIslandWidth() * 0.5f;
	float islandHeight = GetTerrain()->GetIslandLength() * 0.5f;

	float cellSize = GetPathfinder()->GetTerrainCellMap()->GetCellSize() * 0.5f;

	point.x = std::max(-islandWidth + cellSize, point.x);
	point.z = std::max(-islandHeight + cellSize, point.z);

	point.x = std::min(islandWidth - cellSize, point.x);
	point.z = std::min(islandHeight - cellSize, point.z);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::EnableFowUpdate(bool enable)
{
	m_pimpl->m_bEnableFowUpdate = enable;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNPlayer *RDNWorld::CreateNewPlayer()
{
	dbAssert(m_pimpl->m_pWorldFOW);

	return new RDNPlayer(m_pimpl->m_pWorldFOW);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
const WorldFOW *RDNWorld::GetWorldFOW() const
{
	dbAssert(m_pimpl->m_pWorldFOW);

	return m_pimpl->m_pWorldFOW;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::SaveStaticData(IFF &iff, const ImpassEditArray *impassEdit)
{
	iff.PushChunk(Type_Form, 'MSTC', 1);

	// Save out static data, i.e. terrain pathfindingmaps

	GetPathfinder()->Save(iff, this, impassEdit);

	iff.PopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::LoadStaticData(IFF &iff)
{
	iff.AddParseHandler(HandleMSTC, Type_Form, 'MSTC', this, NULL);
}

unsigned long RDNWorld::HandleMSTC(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	RDNWorld *pWorld = static_cast<RDNWorld *>(pContext1);
	dbAssert(pWorld);

	// Load in the saved out static data
	pWorld->GetPathfinder()->Load(iff, static_cast<SimWorld *>(pWorld));

	return iff.Parse();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::PreRemoveEmptyPlayers()
{
	// list all players in use
	std::smallvector<unsigned long, World::PLAYERMAX> inuse;

	size_t ui = 0;
	size_t ue = RDNDllSetup::Instance()->PlayerGetCount();

	for (; ui != ue; ++ui)
	{
		unsigned long idPlayer = RDNDllSetup::Instance()->MapPlayerToSimulation(this, ui);
		inuse.push_back(idPlayer);
	}

	// remove all unused players
	size_t pi = 0;

	while (pi != GetPlayerCount())
	{
		Player *player = GetPlayerAt(pi);

		if (std::find(inuse.begin(), inuse.end(), player->GetID()) == inuse.end())
		{
			RDNPlayer *splayer = static_cast<RDNPlayer *>(player);
			// remember where are the labs would have been - for the AI
			m_pimpl->m_playerSlots.push_back(splayer->GetStartingPosition());
			// this player is not used: kill it & delete it
			player->KillPlayer(RDNPlayer::KPR_UnusedPlayer);
			RmvPlayer(player->GetID());
		}
		else
		{
			pi++;
		}
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNWorld::PreSetPlayerInfo()
{
	//
	size_t i = 0;
	size_t e = GetPlayerCount();

	for (; i != e; ++i)
	{
		RDNPlayer *player = static_cast<RDNPlayer *>(GetPlayerAt(i));

		if (wcslen(player->GetName()) == 0)
		{
			player->SetName(RDNDllSetup::Instance()->PlayerName(player->GetID()));
		}
	}

	return;
}

size_t RDNWorld::GetPlayerSlotCount() const
{
	return m_pimpl->m_playerSlots.size();
}

const Vec3f &RDNWorld::GetPlayerSlotAt(size_t index) const
{
	dbAssert(index >= 0 && index < GetPlayerSlotCount());
	return m_pimpl->m_playerSlots[index];
}

void RDNWorld::CumulateStateTimeBegin()
{
	m_pimpl->m_timingState.Resume();

	return;
}

void RDNWorld::CumulateStateTimeEnd()
{
	m_pimpl->m_timingState.Pause();

	return;
}

long RDNWorld::GetPlayerIDWon() const
{
	return m_pimpl->m_playerIDWon;
}
