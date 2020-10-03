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
#include "Simulation/Controllers/LabController.h"
#include "Simulation/Controllers/GuyController.h"

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

	RC("Lab", Lab_EC, LabController);
	RC("Guy", Guy_EC, GuyController);

#undef RC

	pEntityFactory->SetEntitySize(sizeof(SimEntity));

	return;
}

static void EntityCreate(const Entity *pEntity)
{
	//	Give the RDNHud a chance to see the entity spawning
	if (RDNHUD::IsInitialized())
		RDNHUD::i()->OnEntityCreate(pEntity);

	//
	RDNWorld *pRDNWorld = ModObj::i()->GetWorld();

	EntityAnimator *pAnimator = pEntity->GetAnimator();
	const EntityController *pEC = pEntity->GetController();

	// Make sure entity has animator and controller blueprint
	if (!pEC)
		dbTracef(">>>DEBUG missing controller blueprint for %s", pEntity->GetControllerBP()->GetFileName());

	if (!pAnimator)
		dbTracef(">>>DEBUG missing animator for %s", pEntity->GetControllerBP()->GetFileName());

	if (!pEC || !pAnimator)
	{
		return;
	}

	// Set up the Selection Intersection method for this entity

	// Figure out if the entity is a building, unit, or unselectable(ignore if so)

	// make sure the entity is selectable
	if (!pEntity->GetEntityFlag(EF_Selectable))
		return; // not selectable

	const MovingExtInfo *moving = QIExtInfo<MovingExtInfo>(pEntity);
	const SiteExtInfo *site = QIExtInfo<SiteExtInfo>(pEntity);

	if (moving == 0 && site == 0)
	{
		dbFatalf("MOD -- Missing attributes for placing %S", pEntity->GetControllerBP()->GetScreenName());
	}

	// If it's a moving entity
	if (moving)
	{
		// ENTITY IS A UNIT
		pAnimator->SetSelectionIntersection(EntityAnimator::SIT_ChildBVs | EntityAnimator::SIT_OverSizeBV);
		return;
	}
	else if (site)
	{
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
		dbBreak();
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
			dbBreak();
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
															private DLLGuiInterface,
															public DLLGameInterface
	{
	public:
		RDNDllGameInterface(SimEngineInterface *sim)
		{
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
			return static_cast<DLLGuiInterface *>(this);
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
			UNREF_P(lc);
			//	lc->RegisterLibrary( LUALIB_RDNAI );
		}

		virtual void ShutLuaAI(LuaConfig *lc)
		{
			UNREF_P(lc);
			//	lc->DeRegisterLibrary( LUALIB_RDNAI );
		}

		// inherited -- DLLSimInterface
	private:
		virtual bool InitLuaSim(LuaConfig *lc)
		{
			UNREF_P(lc);
			return true;
		}

		virtual void ShutLuaSim(LuaConfig *lc)
		{
			UNREF_P(lc);
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
				dbBreak();
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

		// inherited -- DLLGuiInterface
	private:
		virtual void InitLuaGui(LuaConfig *lc)
		{
			UNREF_P(lc);
		}

		virtual void ShutLuaGui(LuaConfig *lc)
		{
			UNREF_P(lc);
		}

		virtual void OnEntityCreate(const Entity *e)
		{
			EntityCreate(e);
		}

		virtual void ChangePlayerArmy(unsigned long, const std::vector<long> &)
		{
			dbFatalf("Sample mod doesn't use armies.");
		}

		virtual EntityFilter *GetEntityFilter()
		{
			return RDNEntityFilter::Instance();
		}

		virtual ModSimVis *GetModSimVis()
		{
			if (RDNHUD::IsInitialized())
				return RDNHUD::i();

			return NULL;
		}

		virtual ModUIEvent *GetModUIEvent()
		{
			return RDNHUD::i();
		}

		virtual NISletInterface *GetNISletInterface()
		{
			return RDNNISletInterface::Instance();
		}

		virtual void DoCommand(const EntityGroup &g)
		{
			RDNHUD::i()->DoCommand(g);
		}

		virtual void DoCommand(const Vec3f *v, unsigned long n)
		{
			RDNHUD::i()->DoCommand(v, n);
		}

		virtual bool ProcessInput(const Plat::InputEvent &ie)
		{
			return RDNHUD::i()->Input(ie);
		}

		virtual const char *GetCursor(const Entity *mouseOverEntity)
		{
			return RDNHUD::i()->GetCursor(mouseOverEntity);
		}

		virtual void CreateHUD(
				const Player *localplayer,
				RTSHud *hud,
				CommandInterface *command,
				UIInterface *ui,
				MessageInterface *message,
				SelectionInterface *sel,
				CameraInterface *cam,
				SoundInterface *sound,
				FXInterface *fx)
		{
			// these shouldn't be passed here
			ModObj::i()->SetSoundInterface(sound);
			ModObj::i()->SetFxInterface(fx);

			// these should be sent directly to the trigger system, NOT the ModObj
			ModObj::i()->SetCameraInterface(cam);
			ModObj::i()->SetSelectionInterface(sel);
			ModObj::i()->SetUIInterface(ui);

			RDNHUD::Initialize(
					static_cast<const RDNPlayer *>(localplayer),
					hud,
					command,
					ModObj::i()->GetSelectionInterface(),
					ModObj::i()->GetCameraInterface(),
					ui,
					ModObj::i()->GetSoundInterface(),
					ModObj::i()->GetFxInterface(),
					message);
		}

		virtual void ShutdownHUD()
		{
			RDNHUD::Shutdown();
		}

		virtual void UpdateHUD(float elapsedSeconds)
		{
			RDNHUD::i()->Update(elapsedSeconds);
		}

		virtual void UIPause(bool bPause)
		{
			RDNHUD::i()->UIPause(bPause);
		}

		virtual void Save(IFF &iff)
		{
			RDNUIState::i()->Save(iff);
		}

		virtual void Load(IFF &iff)
		{
			RDNUIState::i()->Load(iff);
		}

		virtual void ShowModOptions(void)
		{
			RDNHUD::i()->ShowModOptions();
		}
	};

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
			m_version[0] = '\0';

			// retrieve mod name
			if (!Localizer::GetString(m_name, LENGTHOF(m_name), MODNAME))
			{
				wcscpy(m_name, L"Impossible Creatures");
			}

			dbTracef("Hello world!");
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
			dbAssert(m_init);
			return m_version;
		}

		// inherited
	public:
		virtual const wchar_t *GetName()
		{
			return m_name;
		}

		virtual bool IsScenarioCompatible(const char *modname) const
		{
			dbTracef(">>>DEBUG checking is scenario is compatible");
			bool isCompatible = (strcmp("RDNMod", modname) == 0);

			return isCompatible;
		}

		virtual bool Initialize(const char *version)
		{
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
			dbTracef(">>>DEBUG creating a game");
			dbAssert(m_init);
			dbAssert(m_game == 0);
			dbAssert(m_score == 0); // shouldn't create a game until score is released
			dbAssert(m_setup == 0); // shouldn't create a game until setup is released

			m_game = new RDNDllGameInterface(sim);

			return m_game;
		}

		virtual void GameDestroy(DLLGameInterface *p)
		{
			//
			dbAssert(p != 0);
			dbAssert(p == m_game);

			//
			DELETEZERO(m_game);

			return;
		}

		virtual DLLScoreInterface *ScoreCreate()
		{
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
			dbAssert(p != 0);
			dbAssert(p == m_score);

			//
			RDNDllScoreDestroy(m_score);
			m_score = 0;

			return;
		}

		virtual DLLInterface::ZsProgress ZsPublish()
		{
			return ZSP_Done;
		}

		virtual DLLInterface::ZsProgress ZsUpdate()
		{
			return ZSP_Done;
		}

		virtual DLLInterface::ZsProgress ZsAbort()
		{
			return ZSP_Done;
		}
	};
} // namespace

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
