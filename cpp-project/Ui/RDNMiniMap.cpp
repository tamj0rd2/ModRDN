/////////////////////////////////////////////////////////////////////
// File    : RDNMiniMap.cpp
// Desc    :
// Created : Monday, April 09, 2001
// Author  : Shelby Hubick
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNMiniMap.h"

#include "../ModObj.h"

#include "BlipFactory.h"
#include "RDNGhost.h"

#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNPlayer.h"
#include "../Simulation/PlayerFOW.h"
#include "../Simulation/WorldFOW.h"

#include "../Simulation/Controllers/ModController.h"

#include "../Simulation/Extensions/UnitSpawnerExt.h"
#include "../Simulation/Extensions/ResourceExt.h"
#include "../Simulation/Extensions/HealthExt.h"
#include "../Simulation/Extensions/SightExt.h"

#include "../Simulation/ExtInfo/UIExtInfo.h"
#include "../Simulation/ExtInfo/HealthExtInfo.h"
#include "../Simulation/ExtInfo/SiteExtInfo.h"

#include <EngineAPI/GhostInterface.h>
#include <EngineAPI/SelectionInterface.h>

#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/EntityController.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/Player.h>

#include <Util/Colour.h>

#include <Assist/Array2d.h>

#include <Platform/InputTypes.h>
#include <Platform/Platform.h>

//------------------------------------------------------------------------------------------------
// NOTE TO SELF: remember that 0,0 of an image is the bottom, left corner!
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------------------------

namespace
{
	const float k_SwitchExpireTime = 1.0f;
}

//------------------------------------------------------------------------------------------------
// RDNMiniMap::Data
//------------------------------------------------------------------------------------------------

class RDNMiniMap::Data
{
public:
	MiniMap *m_miniMap;
	CameraInterface *m_ICamera;
	SelectionInterface *m_ISelection;
	int m_texWidth;
	int m_texHeight;
	float m_terrainWidth;
	float m_terrainHeight;
	const RDNPlayer *m_pPlayer;
	long m_LastGameTickUpdate;
	const RDNGhost *m_pGhost;

	bool m_fowRevealAll;

	bool m_bDrawLocalFirst;
	float m_lastSwitchTime;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RDNMiniMap::RDNMiniMap(MiniMap *pMiniMap, const RDNPlayer *pPlayer, CameraInterface *camera, SelectionInterface *selection, const RDNGhost *pGhost)
		: m_pimpl(new Data)
{
	// player can be null??
	dbAssert(pMiniMap);

	// cache pointer
	m_pimpl->m_miniMap = pMiniMap;
	m_pimpl->m_ICamera = camera;
	m_pimpl->m_ISelection = selection;
	m_pimpl->m_pPlayer = pPlayer;
	m_pimpl->m_pGhost = pGhost;

	//
	m_pimpl->m_fowRevealAll = false;

	//
	m_pimpl->m_bDrawLocalFirst = true;
	m_pimpl->m_lastSwitchTime = 0.0f;

	// get width and height - for now its a constant
	m_pimpl->m_texWidth = 128;
	m_pimpl->m_texHeight = 128;

	const TerrainHMBase *pTerrain = ModObj::i()->GetWorld()->GetTerrain();

	m_pimpl->m_terrainWidth = pTerrain->GetIslandWidth();
	m_pimpl->m_terrainHeight = pTerrain->GetIslandLength();

	// we need a FOW to get the width and height
	// NOTE: local player may be null

	const PlayerFOW *pFoW = 0;

	if (m_pimpl->m_pPlayer != 0)
	{
		pFoW = m_pimpl->m_pPlayer->GetFogOfWar();
	}
	else
	{
		const RDNPlayer *player =
				static_cast<const RDNPlayer *>(ModObj::i()->GetWorld()->GetPlayerAt(0));

		pFoW = player->GetFogOfWar();
	}

	unsigned long w = pFoW->GetWidth(),
								h = pFoW->GetHeight();
	pMiniMap->SetMapSize(w, h);
	m_pimpl->m_LastGameTickUpdate = 0;

	// add all entity spots - with color of team

	// add all resource spots - set to a certain color

	return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RDNMiniMap::~RDNMiniMap()
{
	DELETEZERO(m_pimpl);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void RDNMiniMap::RegisterCallbacksToButtons()
{
	//m_pimpl->m_rightClickCB = BuildingCB::Bind( this, &RDNMiniMap::OnRightClick );
	//m_pimpl->m_leftClickCB = BuildingCB::Bind( this, &RDNMiniMap::OnRightClick );

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void RDNMiniMap::UpdatePoints(float elapsedSeconds)
{
	// let the hud know we are going to be adding points
	m_pimpl->m_miniMap->BeginMarkingPoints();

	//
	if (m_pimpl->m_bDrawLocalFirst)
	{
		AddWorldEntities();
		AddLocalEntities();
	}
	else
	{
		AddLocalEntities();
		AddWorldEntities();
	}

	const World *w = ModObj::i()->GetWorld();
	//
	if (w->GetGameTime() - m_pimpl->m_lastSwitchTime > k_SwitchExpireTime)
	{
		m_pimpl->m_lastSwitchTime = w->GetGameTime();

		m_pimpl->m_bDrawLocalFirst = !m_pimpl->m_bDrawLocalFirst;
	}

	// let the hud know we are done adding points
	m_pimpl->m_miniMap->EndMarkingPoints();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNMiniMap::AddWorldEntities()
{
	const World *w = ModObj::i()->GetWorld();

	//	Update all the ghosts
	EntityFactory *pEntityFactory = ModObj::i()->GetEntityFactory();
	const GhostCont &cGhost = m_pimpl->m_pGhost->GetGhostContainer();
	GhostCont::const_iterator iGhost = cGhost.begin();
	GhostCont::const_iterator eGhost = cGhost.end();
	for (; iGhost != eGhost; ++iGhost)
	{
		const Ghost *pGhost = (*iGhost);
		if (!pGhost->IsVisited())
			continue;

		const ECStaticInfo *si = pEntityFactory->GetECStaticInfo(pGhost->GetControllerBP());
		dbAssert(si);
		const UIExtInfo *pUIExtInfo = QIExtInfo<UIExtInfo>(si);
		if (pUIExtInfo == NULL)
			continue;

		if (!pUIExtInfo->bMiniMapable)
			continue;

		const Vec3f &vec = pGhost->GetTransform().T;
		if (pUIExtInfo->bMiniMapTeamColour)
			m_pimpl->m_miniMap->AddPoint(vec.x, vec.z, pGhost->GetOwner());
		else
			m_pimpl->m_miniMap->AddPoint(vec.x, vec.z, pUIExtInfo->miniMapColour);
	}

	// display all entities
	World::EntityControllerList::const_iterator i = w->GetEntityControllerList().begin();
	World::EntityControllerList::const_iterator e = w->GetEntityControllerList().end();
	for (; i != e; ++i)
	{
		const EntityController *pEC = *i;

		if (pEC->GetEntity() && !pEC->GetEntity()->GetEntityFlag(EF_IsSpawned))
		{
			// don't draw it if not spawned
			continue;
		}

		if ((pEC->GetEntity()->GetOwner() == NULL) || (pEC->GetEntity()->GetOwner() != m_pimpl->m_pPlayer))
		{
			//
			AddEntityPoint(*i, false);
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNMiniMap::AddLocalEntities()
{
	const EntityGroup &selection = m_pimpl->m_ISelection->GetSelection();

	// display all entities
	if (m_pimpl->m_pPlayer)
	{
		EntityGroup::const_iterator iter = m_pimpl->m_pPlayer->GetEntities().begin();
		EntityGroup::const_iterator eiter = m_pimpl->m_pPlayer->GetEntities().end();

		for (; iter != eiter; ++iter)
		{
			if (!(*iter)->GetEntityFlag(EF_IsSpawned))
			{
				continue;
			}

			if ((*iter)->GetController())
			{
				bool bSelected = (std::find(selection.begin(), selection.end(), *iter) != selection.end());
				AddEntityPoint((*iter)->GetController(), bSelected);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNMiniMap::AddEntityPoint(const EntityController *pEC, bool bSelected)
{
	//
	const EntityController *controller = pEC;
	const Entity *entity = controller->GetEntity();

	const UIExtInfo *pUIExtInfo = QIExtInfo<UIExtInfo>(controller);
	if (pUIExtInfo == NULL)
		return;

	if (!pUIExtInfo->bMiniMapable)
		return;

	//	Ghosts are handled outside this loop
	if (pUIExtInfo->bGhostable)
	{
		if (m_pimpl->m_pPlayer &&
				!m_pimpl->m_pPlayer->CanControlEntity(entity))
			return;
	}

	//	Check if the object is alive
	// NOTE: if something doesn't have an HealthExit, then it might be dead
	if (QIExt<HealthExt>(controller) == NULL)
	{
		// if something not selectable, then quite likely it's dead
		if (!entity->GetEntityFlag(EF_Selectable))
			return;
	}

	// check if object is visible
	if (!IsEntityVisible(entity))
		return;

	//
	const Vec3f &vec = entity->GetPosition();

	if (bSelected)
	{
		// display the entity
		m_pimpl->m_miniMap->AddPoint(vec.x, vec.z, Colour::White);

		// check if this is a building with a rally point
		const UnitSpawnerExt *pExt = QIExt<UnitSpawnerExt>(pEC);
		if (pExt && pExt->GetRallyType() != UnitSpawnerExt::RALLY_NoPoint)
		{
			// display the rally point
			Vec3f rally;
			pExt->GetRallyPosition(rally);
			m_pimpl->m_miniMap->AddPoint(rally.x, rally.z, Colour::Red);
		}
	}
	else
	{
		if (pUIExtInfo->bMiniMapTeamColour)
			m_pimpl->m_miniMap->AddPoint(vec.x, vec.z, entity->GetOwner());
		else
			m_pimpl->m_miniMap->AddPoint(vec.x, vec.z, pUIExtInfo->miniMapColour);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void RDNMiniMap::UpdateBlips(float elapsedSeconds)
{
	m_pimpl->m_miniMap->BeginMarkingCircles();

	// display all blips
	BlipFactory *pBlipFac = ModObj::i()->GetBlipFactory();

	size_t bi = 0;
	size_t be = pBlipFac->GetBlipCount();

	for (; bi != be; ++bi)
	{
		Blip *p = pBlipFac->GetBlipAt(bi);

		if (p->Update(elapsedSeconds))
		{
			m_pimpl->m_miniMap->AddCircle(
					p->GetPosition().x,
					p->GetPosition().z,
					p->GetRadius(),
					p->GetColour());
		}
	}

	m_pimpl->m_miniMap->EndMarkingCircles();

	// remove all dead blips
	pBlipFac->DeleteBlipDead();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void RDNMiniMap::UpdateFog(float elapsedSeconds)
{
	if (m_pimpl->m_pPlayer != 0 &&
			m_pimpl->m_pPlayer->IsPlayerDead() == 0 &&
			m_pimpl->m_fowRevealAll == 0)
	{
		const PlayerFOWID fowID =
				m_pimpl->m_pPlayer->GetFogOfWar()->GetPlayerSharedFOWID();

		const WorldFOW *pWorldFOW = ModObj::i()->GetWorld()->GetWorldFOW();

		m_pimpl->m_miniMap->UpdateFOW(
				pWorldFOW->GetCellData(),
				pWorldFOW->CreateCellMask(fowID, FOWC_Visible),
				pWorldFOW->CreateCellMask(fowID, FOWC_Explored));

		TerrainHMBase *pTerrain = ModObj::i()->GetWorld()->GetTerrain();
		pTerrain->SetFogOfWarFullbright(false);
	}
	else
	{
		TerrainHMBase *pTerrain = ModObj::i()->GetWorld()->GetTerrain();
		pTerrain->SetFogOfWarFullbright(true);

		m_pimpl->m_miniMap->RevealAll();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void RDNMiniMap::Update(float elapsedSeconds)
{
	const World *w = ModObj::i()->GetWorld();

	if (m_pimpl->m_LastGameTickUpdate != w->GetGameTicks())
	{
		// per simtick updates go in here

		// remember the last gametick
		m_pimpl->m_LastGameTickUpdate = w->GetGameTicks();

		// update the entity points
		UpdatePoints(elapsedSeconds);

		// update the terrain fog
		UpdateFog(elapsedSeconds);
	}

	// blips are updated every renderframe because they animate
	UpdateBlips(elapsedSeconds);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNMiniMap::SetModalClickCapture(bool bCapture)
{
	m_pimpl->m_miniMap->SetModalClickCapture(bCapture);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNMiniMap::UpdateEntityFow(const Entity *e)
{
	// validate parms
	dbAssert(e);

	// only check FOW if there is a local player, not dead
	bool visible = true;

	if (m_pimpl->m_fowRevealAll)
	{
		visible = true;
	}
	else if (m_pimpl->m_pPlayer)
	{
		if (m_pimpl->m_pPlayer->IsPlayerDead())
			visible = true;
		else
			visible = m_pimpl->m_pPlayer->GetFogOfWar()->IsVisible(e);
	}

	// entities without controller are always visible
	// this includes tress, rocks, waterfalls, ...
	if (e->GetController() == 0)
	{
		//	Make object unlit if it is out of the FoW
		e->GetAnimator()->SetInFOW(!visible);
	}
	else
	{
		//	We only update the opacity if the guy is alive, else StateDead controls it
		if (QIExt<HealthExt>(e))
		{
			//	Setup opacity
			float opacity = 1.0f;

			//
			e->GetAnimator()->SetOpacity(opacity);
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
void RDNMiniMap::SetRevealAll(bool b)
{
	m_pimpl->m_fowRevealAll = b;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNMiniMap::GetRevealAll() const
{
	return m_pimpl->m_fowRevealAll;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNMiniMap::IsEntityVisible(const Entity *e)
{
	if (m_pimpl->m_fowRevealAll)
		return ModObj::i()->GetWorld()->IsEntityVisible(0, e);
	else
		return ModObj::i()->GetWorld()->IsEntityVisible(m_pimpl->m_pPlayer, e);
}
