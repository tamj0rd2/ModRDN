/////////////////////////////////////////////////////////////////////
// File    : RDNHUD.cpp
// Desc    :
// Created : Sunday, February 18, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNHUD.h"

#include "RDNSimProxy.h"
#include "RDNUIProxy.h"
#include "RDNTaskbar.h"
#include "RDNUIOptions.h"
#include "RDNEntityFilter.h"
#include "ObjectiveFactory.h"
#include "BlipFactory.h"
#include "RDNInputBinder.h"
#include "RDNGhost.h"
#include "RDNUIState.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "../Simulation/RDNPlayer.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/GameEventDefs.h"
#include "../Simulation/PlayerFow.h"
#include "../Simulation/WorldFow.h"

#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/Extensions/HealthExt.h"
#include "../Simulation/Extensions/SightExt.h"

#include <SimEngine/EntityAnimator.h>

#include <EngineAPI/UIInterface.h>
#include <EngineAPI/CameraInterface.h>
#include <EngineAPI/SelectionInterface.h>

#include <Lua/LuaConfig.h>

#include <Filesystem/FilePath.h>

#include <Util/IFF.h>

#include <Assist/StlExMap.h>

/////////////////////////////////////////////////////////////////////
// RDNHUD

class RDNHUD::Data
{
public:
	RDNInputBinder *m_pInputBinder;
	RDNSimProxy *m_proxy;
	RDNUIProxy *m_ui;
	RDNTaskbar *m_taskbar;
	RDNUIOptions *m_uioptions;
	RDNGhost *m_ghost;

	LuaConfig *m_lua;

	CameraInterface *m_pCameraInterface;
	UIInterface *m_pUIInterface;
};

static RDNHUD *s_instance = 0;

RDNHUD::RDNHUD()
		: m_pimpl(new Data)
{
}

RDNHUD::~RDNHUD()
{
	// these MUST be destroyed BEFORE lua
	DELETEZERO(m_pimpl->m_proxy);
	DELETEZERO(m_pimpl->m_ui);
	DELETEZERO(m_pimpl->m_taskbar);
	DELETEZERO(m_pimpl->m_ghost);

	DELETEZERO(m_pimpl->m_pInputBinder);
	DELETEZERO(m_pimpl->m_uioptions);

	DELETEZERO(m_pimpl->m_lua);

	DELETEZERO(m_pimpl);

	return;
}

RDNHUD *RDNHUD::instance()
{
	dbAssert(s_instance); // Shelby - I need to test for NULL cuz the ME has no HUD
	return s_instance;
}

bool RDNHUD::IsInitialized()
{
	return s_instance != NULL;
}

void RDNHUD::Initialize(
		const RDNPlayer *localplayer,
		RTSHud *hud,
		CommandInterface *command,
		SelectionInterface *selection,
		CameraInterface *camera,
		UIInterface *ui,
		SoundInterface *sound,
		FXInterface *fx,
		MessageInterface *message)
{
	// check for duplicate initialization
	dbAssert(s_instance == 0);

	// set local player
	RDNEntityFilter::Instance()->SetLocalPlayer(localplayer);

	//
	s_instance = new RDNHUD;

	//
	s_instance->m_pimpl->m_lua = new LuaConfig("RDNhud");
	s_instance->m_pimpl->m_lua->RegisterDefaultLibs(LuaConfig::DL_BASELIB);

	s_instance->m_pimpl->m_pInputBinder = new RDNInputBinder;
	s_instance->m_pimpl->m_pInputBinder->Load();

	//
	s_instance->m_pimpl->m_uioptions = new RDNUIOptions(
			ui,
			camera);

	//
	s_instance->m_pimpl->m_ghost = new RDNGhost(
			localplayer);

	//
	s_instance->m_pimpl->m_proxy = new RDNSimProxy(
			s_instance->m_pimpl->m_lua,
			selection,
			command,
			camera,
			ui,
			fx,
			hud,
			ModObj::i()->GetWorld(),
			localplayer);

	//
	s_instance->m_pimpl->m_ui = new RDNUIProxy(
			s_instance->m_pimpl->m_lua,
			hud,
			selection,
			camera,
			ui,
			fx,
			sound,
			message,
			s_instance->m_pimpl->m_proxy,
			s_instance->m_pimpl->m_pInputBinder,
			s_instance->m_pimpl->m_uioptions);

	//
	s_instance->m_pimpl->m_taskbar = new RDNTaskbar(
			s_instance->m_pimpl->m_lua,
			hud,
			camera,
			selection,
			ui,
			fx,
			s_instance->m_pimpl->m_proxy,
			s_instance->m_pimpl->m_pInputBinder,
			s_instance->m_pimpl->m_ui,
			s_instance->m_pimpl->m_ghost);

	// load lua script
	// NOTE: this must be AFTER everything has been exported to lua
	if (!s_instance->m_pimpl->m_lua->LoadFile("data:RDN/taskbar.lua"))
	{
		// Error loading taskbar.lua (script compile error)
		dbBreak();
	}

	// camera
	s_instance->m_pimpl->m_pCameraInterface = camera;

	// ui
	s_instance->m_pimpl->m_pUIInterface = ui;

	// call first update immediately
	// NOTE: this must be the LAST function before returning
	s_instance->Update(0);

	// Must come After the Update Call
	// Initialize UI state
	if (RDNUIState::i()->IsLoaded())
	{
		RDNUIState *pUIState = RDNUIState::i();

		camera->FocusOnTerrain(pUIState->GetCameraTarget());
		camera->SetDeclination(pUIState->GetCameraDeclination());
		camera->SetRotation(pUIState->GetCameraRotation());
		camera->SetZoom(pUIState->GetCameraZoom());
		camera->ForceCamera();

		size_t i = 0;
		for (; i != RDNUIState::nHOTKEYGROUPS; ++i)
		{
			selection->SetSelection(pUIState->GetHotkeyGroup(i));
			selection->AssignHotkeyGroupFromSelection(int(i), RDNEntityFilter::Instance());
		}

		selection->SetSelection(pUIState->GetSelection());
	}

	return;
}

void RDNHUD::Shutdown()
{
	// check for duplicate shutdown
	dbAssert(s_instance != 0);

	//
	DELETEZERO(s_instance);

	return;
}

void RDNHUD::OnEntityCreate(const Entity *pEntity)
{
	//	Ghosts need to be created
	m_pimpl->m_ghost->OnEntityCreate(pEntity);
}

bool RDNHUD::EntityVisible(const Entity *e) const
{
	// fow stuff
	return m_pimpl->m_taskbar->IsEntityVisible(e);
}

void RDNHUD::EntityVisUpdate(const Entity *pEntity, const Vec3f &interpPos, bool bSelected)
{
	UNREF_P(bSelected);

	// fow stuff
	m_pimpl->m_taskbar->UpdateEntityFow(pEntity);

	// rally point
	m_pimpl->m_taskbar->RallyPointUpdate(pEntity, interpPos);

	return;
}

void RDNHUD::Update(float elapsedSeconds)
{
	// update proxy
	m_pimpl->m_proxy->Update();

	// update controls
	m_pimpl->m_taskbar->Update(elapsedSeconds);

	// update UI
	m_pimpl->m_ui->Update();

	// update ghost
	m_pimpl->m_ghost->Update();

	return;
}

bool RDNHUD::Input(const Plat::InputEvent &ie)
{
	// sim proxy (must be BEFORE the taskbar)
	if (m_pimpl->m_proxy->Input(ie))
		return true;

	// taskbar
	if (m_pimpl->m_taskbar->Input(ie))
		return true;

	return false;
}

const char *RDNHUD::GetCursor(const Entity *mouseOverEntity)
{
	static char cursor[128];
	int ttStrId = 0;
	cursor[0] = 0;

	m_pimpl->m_taskbar->GetCursorInfoOverride(cursor, LENGTHOF(cursor), ttStrId, mouseOverEntity);

	// if the taskbar didn't supply a tooltip
	// check the proxy
	if (strlen(cursor) == 0)
	{
		m_pimpl->m_proxy->GetCursorInfo(cursor, LENGTHOF(cursor), ttStrId, mouseOverEntity);
	}

	// pass tooltip too the taskbar to display
	m_pimpl->m_taskbar->OnIngameTooltip(ttStrId);

	return cursor;
}

void RDNHUD::DoCommand(const EntityGroup &eg)
{
	dbTracef("RDNHUD::DoCommand group command");
	m_pimpl->m_proxy->DoCommand(eg, m_pimpl->m_taskbar->IsCommandQueueKeyPressed());
}

void RDNHUD::DoCommand(const Vec3f *v, unsigned long num)
{
	dbTracef("RDNHUD::DoCommand single command");
	m_pimpl->m_proxy->DoCommand(v, num, m_pimpl->m_taskbar->IsCommandQueueKeyPressed());
}

void RDNHUD::UIPause(bool bPause)
{
	UNREF_P(bPause);
}

void RDNHUD::ShowModOptions(void)
{
	m_pimpl->m_ui->ModOptionsShow();
}

void RDNHUD::OnPlayerDrops(unsigned long idplayer)
{
	const RDNPlayer *player =
			static_cast<const RDNPlayer *>(ModObj::i()->GetWorld()->GetPlayerFromID(idplayer));

	dbAssert(player);

	GameEventSys::Instance()->PublishEvent(GameEvent_PlayerDropped(player));

	return;
}

void RDNHUD::OnHostMigrated(unsigned long idplayer)
{
	const RDNPlayer *player =
			static_cast<const RDNPlayer *>(ModObj::i()->GetWorld()->GetPlayerFromID(idplayer));

	dbAssert(player);

	GameEventSys::Instance()->PublishEvent(GameEvent_HostMigrated(player));

	return;
}

void RDNHUD::OnCinematicMode(bool bCinematic)
{
	m_pimpl->m_taskbar->OnCinematicMode(bCinematic);
	m_pimpl->m_ui->OnCinematicMode(bCinematic);
}

void RDNHUD::OnShowTeamColour(bool bShow)
{
	UNREF_P(bShow);
}

void RDNHUD::OnResetSM()
{
	// reset taskbar
	m_pimpl->m_proxy->SetDirtyFlag();

	// reset modal UI state
	m_pimpl->m_taskbar->ModalUIReset();
}

void RDNHUD::OnCharacterTalk(unsigned long entityID, bool bTalk)
{
	m_pimpl->m_ui->OnCharacterTalk(entityID, bTalk);
}

RDNTaskbar *RDNHUD::GetTaskbar()
{
	return m_pimpl->m_taskbar;
}

void RDNHUD::Draw()
{
}

const Array2D<unsigned long> *RDNHUD::GetFOWInfo(unsigned long &visiblemask, unsigned long &exploredmask)
{
	const RDNPlayer *pPlayer = m_pimpl->m_proxy->GetPlayer();

	// there is no local player when playing back recorded games
	if (!pPlayer)
	{
		return NULL;
	}

	const PlayerFOWID fowID = pPlayer->GetFogOfWar()->GetPlayerSharedFOWID();

	const WorldFOW *pWorldFOW = ModObj::i()->GetWorld()->GetWorldFOW();

	visiblemask = pWorldFOW->CreateCellMask(fowID, FOWC_Visible);
	exploredmask = pWorldFOW->CreateCellMask(fowID, FOWC_Explored);

	return &pWorldFOW->GetCellData();
}
