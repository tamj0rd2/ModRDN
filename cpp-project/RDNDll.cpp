/////////////////////////////////////////////////////////////////////
// File    : RDNDll.cpp
// Desc    :
// Created : Thursday, August 02, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNDll.h"
#include "RDNDllScore.h"

#include "Dll/ModGuiInterface.h"
#include "ModObj.h"
#include "RDNDllSetup.h"
#include "UI/RDNText.h"

#include "Stats/RDNStats.h"

#include "Simulation/RDNPlayer.h"
#include "Simulation/RDNWorld.h"
#include "Simulation/RDNTuning.h"
#include "Simulation/RDNEBPs.h"

#include "Simulation/CommandTypes.h"

#include "Simulation/Controllers/ControllerTypes.h"
#include "Simulation/Controllers/CoalController.h"
#include "Simulation/Controllers/BuildingController.h"
#include "Simulation/Controllers/LabController.h"
#include "Simulation/Controllers/HenchmenController.h"

#include "UI/RDNHud.h"
#include "UI/RDNUIState.h"
#include "UI/RDNEntityFilter.h"
#include "UI/RDNNISletInterface.h"

#include "CPUPlayer/RDNAI.h"

#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/BuildingDynamics.h>
#include <SimEngine/PathFinding/ImpassMap.h>
#include <SimEngine/PathFinding/PathFinding.h>
#include <SimEngine/PathFinding/PathFindingLua.h>

#include <EngineAPI/SimEngineInterface.h>
#include <EngineAPI/EntityFactory.h>

#include <ModInterface/DllInterface.h>

#include <Localizer/Localizer.h>

#include <Filesystem/FilePath.h>
#include <Util/Iff.h>

/////////////////////////////////////////////////////////////////////
//

static void RegisterControllers(SimEngineInterface *p)
{
	EntityFactory *pEntityFactory = p->GetEntityFactory();

#define RC(name, type, classtype) \
	pEntityFactory->RegisterController(name, type, new EntityFactory_ControllerCreator_Templ<classtype::StaticInfo, classtype>);

	RC("Henchmen", Henchmen_EC, HenchmenController);
	RC("Coal", Coal_EC, CoalController);
	RC("Lab", Lab_EC, LabController);
	RC("LightningRod", ResourceRenew_EC, BuildingController);
	RC("CreatureChamber", RemoteChamber_EC, BuildingController);
	RC("WaterChamber", WaterChamber_EC, BuildingController);
	RC("AirChamber", Aviary_EC, BuildingController);
	RC("BrambleFence", BrambleFence_EC, BuildingController);
	RC("Workshop", Foundry_EC, BuildingController);
	RC("SoundbeamTower", SoundBeamTower_EC, BuildingController);
	RC("GeoGenerator", ElectricGenerator_EC, BuildingController);

#undef RC

	pEntityFactory->SetEntitySize(sizeof(SimEntity));

	return;
}

static void EntityCreate(const Entity *pEntity)
{
	dbTracef("RDNDLL::EntityCreate");

	//	Give the RDNHud a chance to see the entity spawning
	if (RDNHUD::IsInitialized())
		RDNHUD::instance()->OnEntityCreate(pEntity);

	//
	RDNWorld *pRDNWorld = ModObj::i()->GetWorld();

	EntityAnimator *pAnimator = pEntity->GetAnimator();
	const EntityController *pEC = pEntity->GetController();

	/**
	 * These aren't necessarily required for every entity in the game. Just the ones that we specifically
	 * want to be able to control (like henchmen for instance)
	*/
	if (!pEC || !pAnimator)
	{
		dbTracef("RDNDll::EntityCreate no controller blueprint or animator for %s", pEntity->GetControllerBP()->GetFileName());
		return;
	}

	// Set up the Selection Intersection method for this entity

	// Figure out if the entity is a building, unit, or unselectable(ignore if so)

	// make sure the entity is selectable
	if (!pEntity->GetEntityFlag(EF_Selectable))
		return; // not selectable

	const MovingExtInfo *moving = QIExtInfo<MovingExtInfo>(pEntity);
	const SiteExtInfo *site = QIExtInfo<SiteExtInfo>(pEntity);
	const ResourceExtInfo *resourceInfo = QIExtInfo<ResourceExtInfo>(pEntity);

	if (moving == 0 && site == 0 && resourceInfo == 0)
	{
		dbFatalf("MOD -- Missing attributes for placing %S", pEntity->GetControllerBP()->GetScreenName());
	}

	const ControllerBlueprint *pControllerBP = pEntity->GetControllerBP();
	if (pControllerBP)
	{
		dbTracef("Placing %s in the world", pControllerBP->GetFileName());
	}

	// If it's a moving entity
	if (moving)
	{
		// ENTITY IS A UNIT
		dbTracef("Entity is a unit");
		pAnimator->SetSelectionIntersection(EntityAnimator::SIT_ChildBVs | EntityAnimator::SIT_OverSizeBV);
		return;
	}
	else if (site)
	{
		dbTracef("Entity is a site/building");
		// ENTITY IS A BUILDING
		pAnimator->SetSelectionIntersection(EntityAnimator::SIT_GroundSelect | EntityAnimator::SIT_ChildBVs);
		// set impass info
		long cellSize = (long)pRDNWorld->GetPathfinder()->GetTerrainCellMap()->GetCellSize();
		long width, height;
		BuildingDynamics::GetEntityWidthHeight(pEntity->GetControllerBP(), &pEntity->GetTransform(), width, height);
		pAnimator->SetGroundSelectInfo(width * cellSize, height * cellSize);
	}
	else
	{
		dbTracef("Entity is something else");
		// If we can't figure out what it is then treat it as a building.
		pAnimator->SetSelectionIntersection(EntityAnimator::SIT_ChildBVs | EntityAnimator::SIT_OverSizeBV);
	}

	return;
}

static void KillSimPlayer(unsigned long idplayer, DLLSimInterface::NetworkKillType type)
{
	//
	RDNWorld *w = ModObj::i()->GetWorld();

	//
	Player *p = w->GetPlayerFromID(idplayer);

	if (p == 0)
	{
		dbFatalf("No player id given");
		return;
	}

	// kill that player (if not already dead)
	if (p->IsPlayerDead() == 0)
	{
		int RDNReason = -1;

		switch (type)
		{
		case DLLSimInterface::NKT_Aborted:
			RDNReason = RDNPlayer::KPR_NetworkAbort;
			break;
		case DLLSimInterface::NKT_Disconnected:
			RDNReason = RDNPlayer::KPR_NetworkDisconnected;
			break;
		case DLLSimInterface::NKT_KickedOut:
			RDNReason = RDNPlayer::KPR_NetworkKickedOut;
			break;
		case DLLSimInterface::NKT_OutOfSync:
			RDNReason = RDNPlayer::KPR_NetworkOutOfSync;
			break;

		default:
			dbFatalf("Unhandled player death reason");
		}

		p->KillPlayer(RDNReason);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//

namespace
{
	class RDNDllGameInterface : private DLLCpuInterface,
															private DLLSimInterface,
															public DLLGameInterface
	{
	public:
		ModGuiInterface *m_pDllGuiInterface;

		RDNDllGameInterface(SimEngineInterface *sim) : m_pDllGuiInterface(NULL)
		{
			m_pDllGuiInterface = new ModGuiInterface();

			RegisterControllers(sim);
			ModObj::Initialize(sim);
			RDNUIState::Startup();

			return;
		}

		~RDNDllGameInterface()
		{
			RDNUIState::Shutdown();

			ModObj::Shutdown();
		}

		// inherited -- DLLGameInterface
	public:
		virtual DLLCpuInterface *GetCpuInterface()
		{
			return static_cast<DLLCpuInterface *>(this);
		}

		virtual DLLGuiInterface *GetGuiInterface()
		{
			dbAssert(m_pDllGuiInterface != NULL);
			return m_pDllGuiInterface;
		}

		virtual DLLSimInterface *GetSimInterface()
		{
			return static_cast<DLLSimInterface *>(this);
		}

		// inherited -- DLLCpuInterface
	private:
		virtual GameAI *CreateGameAI(CommandInterface *command)
		{
			return new RDNAI(command);
		}

		virtual void InitLuaAI(LuaConfig *lc)
		{
			// TODO: implement this
			dbTracef("RDNDLL::InitLuaAI not implemented");
			// lc->RegisterLibrary( LUALIB_RDNAI );
		}

		virtual void ShutLuaAI(LuaConfig *lc)
		{
			// TODO: implement this
			dbTracef("RDNDLL::ShutLuaAI not implemented");
			// lc->DeRegisterLibrary( LUALIB_RDNAI );
		}

		// inherited -- DLLSimInterface
	private:
		virtual bool InitLuaSim(LuaConfig *lc)
		{
			// TODO: implement this
			dbTracef("RDNDLL::InitLuaSim not implemented");
			return true;
		}

		virtual void ShutLuaSim(LuaConfig *lc)
		{
			// TODO: implement this
			dbTracef("RDNDLL::ShutLuaSim not implemented");
		}

		virtual void InitTriggers()
		{
		}

		virtual void SetDecalInterface(DecalInterface *decal)
		{
			ModObj::i()->SetDecalInterface(decal);
		}

		virtual void SetTerrainOverlayInterface(TerrainOverlayInterface *overlay)
		{
			ModObj::i()->SetTerrainOverlayInterface(overlay);
		}

		virtual void SetGhostInterface(GhostInterface *ghost)
		{
			ModObj::i()->SetGhostInterface(ghost);
		}

		virtual World *CreateNewWorld(bool bMissionEd)
		{
			dbTracef(">>>DEBUG creating a new world");
			ModObj::i()->CreateWorld(bMissionEd);

			// set random seed
			World *pWorld = ModObj::i()->GetWorld();
			if (pWorld)
			{
				pWorld->SetRandomSeed(RDNDllSetup::Instance()->GetRandomSeed());
			}

			return pWorld;
		}

		virtual Player *CreateNewPlayer()
		{
			dbTracef(">>>DEBUG creating a new player");
			return ModObj::i()->GetWorld()->CreateNewPlayer();
		}

		virtual Entity *CreateNewEntity(void *buffer, unsigned long id, const ControllerBlueprint *cbp)
		{
			return new (buffer) SimEntity(id, cbp);
		}

		virtual unsigned long MapPlayerToSimulation(size_t playerIndex) const
		{
			dbTracef(">>>DEBUG mapping player to simulation");
			return RDNDllSetup::Instance()->MapPlayerToSimulation(ModObj::i()->GetWorld(), playerIndex);
		}

		virtual void GetDataToken(std::vector<std::pair<unsigned long, const char *> > &crcArray) const
		{
			// tuning
			const unsigned long tuning = RDNTuning::Instance()->GetSyncToken();

			//
			crcArray.push_back(std::make_pair(tuning, (const char *)"tuning"));

			return;
		}

		virtual bool IsPlayerAlly(unsigned long idPlayer1, unsigned long idPlayer2) const
		{
			return idPlayer1 == idPlayer2;
		}

		virtual bool IsPlayerEnemy(unsigned long idPlayer1, unsigned long idPlayer2) const
		{
			return idPlayer1 != idPlayer2;
		}

		virtual bool IsScenarioSuccess(unsigned long idPlayer) const
		{
			dbTracef(">>>DEBUG checking if scenario is a success");
			// check if game is over
			if (ModObj::i()->GetWorld()->IsGameOver() == 0)
				return false;

			// check if specified player is dead
			const Player *player = ModObj::i()->GetWorld()->GetPlayerFromID(idPlayer);

			if (player == 0)
			{
				dbFatalf("IsScenarioSuccess bad path");
				return false;
			}

			if (player->IsPlayerDead())
				return false;

			// yup, scenario is all good
			return true;
		}

		virtual void OnTerrainModify(const Rect2f &rect, const ImpassEditArray *impassEdit)
		{
			if (ModObj::i()->GetWorld()->GetPathfinder())
			{
				ModObj::i()->GetWorld()->GetPathfinder()->OnTerrainModify(ModObj::i()->GetWorld(), rect, impassEdit);
			}
		}

		virtual bool IsCellImpassible(int x, int z)
		{
			if (ModObj::i()->GetWorld()->GetPathfinder())
			{
				const TerrainCellMap *tcmap = ModObj::i()->GetWorld()->GetPathfinder()->GetTerrainCellMap();
				TCMask test = eLandImpassible | eWaterImpassible | eAmphibianImpassible;
				if ((tcmap && (tcmap->GetCell(x, z) & test) == test))
				{
					// totally impassible
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		virtual size_t GetPlayerCount(GameType gametype)
		{
			// go through each player
			const RDNWorld *pWorld = ModObj::i()->GetWorld();

			size_t pi = 0;
			size_t playerCount = 0;

			for (; pi != pWorld->GetPlayerCount(); ++pi)
			{
				const Player *player = pWorld->GetPlayerAt(pi);
				const EntityGroup &egroup = player->GetEntities();

				// it atleast has units
				if (egroup.size() > 0)
				{
					if (gametype == GT_SP)
					{
						// has units good, enough for SP
						playerCount++;
					}
					else
					{
						// look for the lab ebpnetID
						EntityGroup::const_iterator iter = egroup.begin();

						for (; iter != egroup.end(); iter++)
						{
							Entity *ent = (*iter);
							// found a lab
							if (ent->GetControllerBP() == RDNEBP::Get(RDNEBP::Lab))
							{
								playerCount++;
								break;
							}
						}
					}
				}
			}

			return playerCount;
		}

		virtual void SaveWorldStaticData(IFF &iff, const ImpassEditArray *impassEdit)
		{
			ModObj::i()->GetWorld()->SaveStaticData(iff, impassEdit);
		}

		virtual void LoadWorldStaticData(IFF &iff)
		{
			ModObj::i()->GetWorld()->LoadStaticData(iff);
		}

		virtual void LoadSPPersistentData(IFF &, SPPersistenceInterface *)
		{
			//	Add parse handlers here, allow caller to Parse()
		}

		virtual void SaveSPPersistentData(IFF &, SPPersistenceInterface *)
		{
		}

		virtual void NetworkKillPlayer(unsigned long idplayer, NetworkKillType type)
		{
			KillSimPlayer(idplayer, type);
		}

		virtual void StatsGameAbort()
		{
		}

		virtual void StatsZSSave()
		{
		}
	};

#define LOGIT(x) \
	dbTracef("RDNDllInterface: " #x)

	class RDNDllInterface : public DLLInterface
	{
		// fields
	private:
		wchar_t m_name[32];

		char m_version[16];

		RDNDllGameInterface *
				m_game;

		DLLSetupInterface *
				m_setup;

		DLLScoreInterface *
				m_score;

		bool m_init;

		// construction -- singleton
	public:
		RDNDllInterface()
				: m_init(false),
					m_game(0),
					m_score(0),
					m_setup(0)
		{
			//
			LOGIT(Constructor);
			m_version[0] = '\0';

			// retrieve mod name
			if (!Localizer::GetString(m_name, LENGTHOF(m_name), MODNAME))
			{
				wcscpy(m_name, L"Impossible Creatures");
			}
		}

		~RDNDllInterface()
		{
			dbAssert(m_game == 0);
			dbAssert(m_score == 0);
			dbAssert(m_setup == 0);
			dbAssert(m_init == false);
		}

		// interface
	public:
		const char *GetVersion() const
		{
			LOGIT(GetVersion);
			dbAssert(m_init);
			return m_version;
		}

		// inherited
	public:
		virtual const wchar_t *GetName()
		{
			LOGIT(GetName);
			return m_name;
		}

		virtual bool IsScenarioCompatible(const char *modname) const
		{
			LOGIT(IsScenarioCompatible);
			bool isCompatible = (strcmp("RDNMod", modname) == 0);

			return isCompatible;
		}

		virtual bool Initialize(const char *version)
		{
			LOGIT(Initialize);
			//
			dbAssert(!m_init);

			//
			m_init = true;

			//
			strcpy(m_version, version);

			// read-only data
			RDNTuning::Initialize();

			// initialize the stats layer - since the stats object lives longer than
			// the simulation, it is initialized before and shutdown later
			RDNStats::Initialize();

			// initialize mod setup stuff
			RDNDllSetup::Initialize();

			return true;
		}

		virtual void Shutdown()
		{
			LOGIT(Shutdown);
			//
			dbAssert(m_score == 0);
			dbAssert(m_setup == 0);
			dbAssert(m_game == 0);
			dbAssert(m_init);

			// read-only data
			RDNTuning::Shutdown();

			// mod setup stuff
			RDNDllSetup::Shutdown();

			// since the stats object lives longer than the simulation,
			// it is initialized before and shutdown later
			RDNStats::Shutdown();

			//
			m_init = false;

			return;
		}

		virtual DLLSetupInterface *SetupCreate()
		{
			LOGIT(SetupCreate);
			dbAssert(m_init);
			dbAssert(m_setup == 0);
			dbAssert(m_game == 0);	// shouldn't create a scores while a game is being played
			dbAssert(m_score == 0); // shouldn't create a scores until score is released

			RDNDllSetup::Instance()->Reset();
			m_setup = RDNDllSetup::Instance()->Get();

			return m_setup;
		}

		virtual void SetupDestroy(DLLSetupInterface *p)
		{
			LOGIT(SetupDestroy);
			//
			dbAssert(p != 0);
			dbAssert(p == m_setup);

			//
			RDNDllSetup::Instance()->Release(m_setup);
			m_setup = 0;

			return;
		}

		virtual DLLGameInterface *GameCreate(SimEngineInterface *sim)
		{
			LOGIT(GameCreate);
			dbAssert(m_init);
			dbAssert(m_game == 0);
			dbAssert(m_score == 0); // shouldn't create a game until score is released
			dbAssert(m_setup == 0); // shouldn't create a game until setup is released

			m_game = new RDNDllGameInterface(sim);

			return m_game;
		}

		virtual void GameDestroy(DLLGameInterface *p)
		{
			LOGIT(GameDestroy);
			//
			dbAssert(p != 0);
			dbAssert(p == m_game);

			//
			DELETEZERO(m_game);

			return;
		}

		virtual DLLScoreInterface *ScoreCreate()
		{
			LOGIT(ScoreCreate);
			dbAssert(m_init);
			dbAssert(m_score == 0);
			dbAssert(m_game == 0);	// shouldn't create a scores while a game is being played
			dbAssert(m_setup == 0); // shouldn't create a scores until setup is released

			m_score = RDNDllScoreCreate();

			return m_score;
		}

		virtual void ScoreDestroy(DLLScoreInterface *p)
		{
			//
			LOGIT(ScoreDestroy);
			dbAssert(p != 0);
			dbAssert(p == m_score);

			//
			RDNDllScoreDestroy(m_score);
			m_score = 0;

			return;
		}

		virtual DLLInterface::ZsProgress ZsPublish()
		{
			LOGIT(ZsPublish);
			return ZSP_Done;
		}

		virtual DLLInterface::ZsProgress ZsUpdate()
		{
			LOGIT(ZsUpdate);
			return ZSP_Done;
		}

		virtual DLLInterface::ZsProgress ZsAbort()
		{
			LOGIT(ZsAbort);
			return ZSP_Done;
		}
	};
} // namespace
#undef LOGIT

/////////////////////////////////////////////////////////////////////
//

static RDNDllInterface *s_instance = 0;

void RDNDllInterfaceInitialize()
{
	s_instance = new RDNDllInterface;
}

void RDNDllInterfaceShutdown()
{
	DELETEZERO(s_instance);
}

const char *RDNDLLVersion()
{
	return s_instance->GetVersion();
}

/////////////////////////////////////////////////////////////////////
//

extern "C"
{
	__declspec(dllexport) DLLInterface *__cdecl GetDllInterface()
	{
		return s_instance;
	}

	__declspec(dllexport) unsigned long __cdecl GetDllVersion()
	{
		return MODMAKE_VERSION(MajorVersion, MinorVersion);
	}
}
