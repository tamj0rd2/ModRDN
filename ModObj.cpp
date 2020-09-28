/////////////////////////////////////////////////////////////////////
// File    : ModObj.cpp
// Desc    :
// Created : Friday, February 16, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "ModObj.h"

#include "Simulation/GameEventSys.h"
#include "Simulation/RDNWorld.h"

#include "UI/ObjectiveFactory.h"
#include "UI/BlipFactory.h"
#include "UI/RDNEntityFilter.h"
#include "UI/RDNNISletInterface.h"

#include <EngineApi/SimEngineInterface.h>
#include <EngineApi/CharacterMap.h>

#include <EngineApi/UiInterface.h>
#include <EngineApi/DecalInterface.h>

/////////////////////////////////////////////////////////////////////
// ModObj

class ModObj::Data
{
public:
	// all weak pointers
	SimEngineInterface *m_pSimEngine;
	EntityFactory *m_pEntityFactory;
	TriggerFactory *m_pTriggerFactory;
	ObjectiveFactory *m_pObjectiveFactory;
	BlipFactory *m_pBlipFactory;
	SelectionInterface *m_pSelInt;
	CameraInterface *m_pCamInt;
	SoundInterface *m_pSndInt;
	FXInterface *m_pFxInt;
	GhostInterface *m_pGhostInt;
	DecalInterface *m_pDecalInt;
	TerrainOverlayInterface *m_pTerrOverlayInt;
	UIInterface *m_pUiInt;
	CharacterMap *m_pCharMap;
};

static ModObj *s_modobj = NULL;

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ModObj::ModObj()
		: m_pimpl(new Data)
{
	m_pimpl->m_pSimEngine = 0;
	m_pimpl->m_pEntityFactory = 0;
	m_pimpl->m_pTriggerFactory = 0;
	m_pimpl->m_pObjectiveFactory = 0;
	m_pimpl->m_pBlipFactory = 0;
	m_pimpl->m_pSelInt = 0;
	m_pimpl->m_pCamInt = 0;
	m_pimpl->m_pSndInt = 0;
	m_pimpl->m_pFxInt = 0;
	m_pimpl->m_pGhostInt = 0;
	m_pimpl->m_pDecalInt = 0;
	m_pimpl->m_pTerrOverlayInt = 0;
	m_pimpl->m_pUiInt = 0;
	m_pimpl->m_pCharMap = 0;

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ModObj::~ModObj()
{
	DELETEZERO(m_pimpl->m_pObjectiveFactory);
	DELETEZERO(m_pimpl->m_pBlipFactory);
	DELETEZERO(m_pimpl);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::Initialize(SimEngineInterface *sim)
{
	// check for duplicate initialization
	dbAssert(s_modobj == NULL);

	// create instance
	s_modobj = new ModObj;

	s_modobj->m_pimpl->m_pSimEngine = sim;
	s_modobj->m_pimpl->m_pEntityFactory = sim->GetEntityFactory();
	s_modobj->m_pimpl->m_pTriggerFactory = sim->GetTriggerFactory();
	s_modobj->m_pimpl->m_pCharMap = sim->GetCharacterMap();

	s_modobj->m_pimpl->m_pObjectiveFactory = new ObjectiveFactory;
	s_modobj->m_pimpl->m_pBlipFactory = new BlipFactory;

	// initialze dependent singletons
	GameEventSys ::Initialize();
	RDNEntityFilter ::Initialize();
	RDNNISletInterface ::Initialize();

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::Shutdown()
{
	// check for duplicate shutdown
	dbAssert(s_modobj != NULL);

	// shutdown dependent singletons
	GameEventSys ::Shutdown();
	RDNEntityFilter ::Shutdown();
	RDNNISletInterface ::Shutdown();

	//
	DELETEZERO(s_modobj);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ModObj *ModObj::i()
{
	dbAssert(s_modobj);
	return s_modobj;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
EntityFactory *ModObj::GetEntityFactory()
{
	dbAssert(m_pimpl->m_pEntityFactory);
	return m_pimpl->m_pEntityFactory;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
TriggerFactory *ModObj::GetTriggerFactory()
{
	dbAssert(m_pimpl->m_pTriggerFactory);
	return m_pimpl->m_pTriggerFactory;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ObjectiveFactory *ModObj::GetObjectiveFactory()
{
	dbAssert(m_pimpl->m_pObjectiveFactory);
	return m_pimpl->m_pObjectiveFactory;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
BlipFactory *ModObj::GetBlipFactory()
{
	dbAssert(m_pimpl->m_pBlipFactory);
	return m_pimpl->m_pBlipFactory;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
SelectionInterface *ModObj::GetSelectionInterface()
{
	return m_pimpl->m_pSelInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
CameraInterface *ModObj::GetCameraInterface()
{
	return m_pimpl->m_pCamInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
SoundInterface *ModObj::GetSoundInterface()
{
	return m_pimpl->m_pSndInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
FXInterface *ModObj::GetFxInterface()
{
	return m_pimpl->m_pFxInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
GhostInterface *ModObj::GetGhostInterface()
{
	return m_pimpl->m_pGhostInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
DecalInterface *ModObj::GetDecalInterface()
{
	return m_pimpl->m_pDecalInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
UIInterface *ModObj::GetUIInterface()
{
	return m_pimpl->m_pUiInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
CharacterMap *ModObj::GetCharacterMap()
{
	return m_pimpl->m_pCharMap;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetSelectionInterface(SelectionInterface *pSelInt)
{
	m_pimpl->m_pSelInt = pSelInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetCameraInterface(CameraInterface *pCamInt)
{
	m_pimpl->m_pCamInt = pCamInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetSoundInterface(SoundInterface *pSndInt)
{
	m_pimpl->m_pSndInt = pSndInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetFxInterface(FXInterface *pFX)
{
	m_pimpl->m_pFxInt = pFX;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetGhostInterface(GhostInterface *pGhost)
{
	m_pimpl->m_pGhostInt = pGhost;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetDecalInterface(DecalInterface *pDecal)
{
	m_pimpl->m_pDecalInt = pDecal;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetTerrainOverlayInterface(TerrainOverlayInterface *overlay)
{
	m_pimpl->m_pTerrOverlayInt = overlay;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
TerrainOverlayInterface *ModObj::GetTerrainOverlayInterface()
{
	return m_pimpl->m_pTerrOverlayInt;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::SetUIInterface(UIInterface *pUi)
{
	m_pimpl->m_pUiInt = pUi;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNWorld *ModObj::GetWorld()
{
	return RDNWorld::GetWorld();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModObj::CreateWorld(bool bMissionEd)
{
	RDNWorld::CreateWorld(m_pimpl->m_pSimEngine, bMissionEd);
}
