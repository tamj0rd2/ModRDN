/////////////////////////////////////////////////////////////////////
// File    : BuildingController.cpp
// Desc    :
// Created : Wednesday, February 21, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "BuildingController.h"

#include "../../ModObj.h"
#include "../RDNPlayer.h"
#include "../CommandTypes.h"
#include "../RDNWorld.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/EntityCommand.h>
#include <SimEngine/BuildingDynamics.h>

#include <EngineAPI/EntityFactory.h>
#include <EngineAPI/ControllerBlueprint.h>

#include <ModInterface/ECStaticInfo.h>

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
BuildingController::StaticInfo::StaticInfo(const ControllerBlueprint *cbp)
		: HealthExtInfo(cbp),
			SightExtInfo(cbp),
			SiteExtInfo(cbp),
			CostExtInfo(cbp),
			ModStaticInfo(cbp),
			UIExtInfo(cbp)
{
}

const ModStaticInfo::ExtInfo *
BuildingController::StaticInfo::QInfo(unsigned char id) const
{
	if (id == HealthExtInfo ::ExtensionID)
		return static_cast<const HealthExtInfo *>(this);
	if (id == CostExtInfo ::ExtensionID)
		return static_cast<const CostExtInfo *>(this);
	if (id == SiteExtInfo ::ExtensionID)
		return static_cast<const SiteExtInfo *>(this);
	if (id == SightExtInfo ::ExtensionID)
		return static_cast<const SightExtInfo *>(this);
	if (id == UIExtInfo ::ExtensionID)
		return static_cast<const UIExtInfo *>(this);

	return 0;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
BuildingController::BuildingController(Entity *pEntity, const ECStaticInfo *si) : ModController(pEntity, SiteExtInfo::CreateDynamics(pEntity, QIExtInfo<SiteExtInfo>(si)), si),
																																									HealthExt(pEntity, QIExtInfo<HealthExtInfo>(si)),
																																									UnitSpawnerExt(UnitSpawnerExt::BT_Spawn),
																																									SightExt(QIExtInfo<SightExtInfo>(si)),
																																									m_stateidle(GetEntityDynamics()),
																																									m_statedead(GetEntityDynamics()),
																																									m_pCurrentState(NULL)
{
	//
	SetHealth(GetHealthMax());

	// setup health attributes
	SetHealthAttribute(HA_Building);

	//
	m_commandproc.Init(this);
	SetCommandProcessor(&m_commandproc);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
BuildingController::~BuildingController()
{
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
bool BuildingController::CommandDoProcessNow(const EntityCommand *ec)
{
	dbTracef("BuildingController::CommandDoProcessNow");
	return ModController::CommandDoProcessNow(ec);
}

void BuildingController::Execute()
{
	// forward to base class
	ModController::Execute();

	if (m_commandproc.IsDead())
		return;

	UnitBuild();

	// Let my modifier extension run and update the active modifiers
	ModifierExt::Execute();

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
ModController *BuildingController::GetSelf()
{
	return this;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::OnUnitSpawn(Entity *entity)
{
	// make the henchmen conform to terrain
	SimController *pSimController = static_cast<SimController *>(entity->GetController());
	if (pSimController && pSimController->GetEntityDynamics())
	{
		Vec2f facing;
		facing = Vec2f(entity->GetPosition().x, entity->GetPosition().z) - Vec2f(GetEntity()->GetPosition().x, GetEntity()->GetPosition().z);

		// watch out for Normalizing 0 length vector
		if (facing.LengthSqr() > 0.1)
		{
			facing.NormalizeSelf();
		}
		else
		{
			facing.Set(0.0, 1.0f);
		}

		// set the entities facing and also make the up vector point appropriately
		pSimController->GetEntityDynamics()->SetEntityFacing(facing);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
Extension *BuildingController::QI(unsigned char id)
{
	// If we do any dynamic detachment of an extension put that logic here
	if (m_commandproc.IsDead())
		return NULL;

	return BuildingController::QIAll(id);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
Extension *BuildingController::QIAll(unsigned char id)
{
	if (id == HealthExt::ExtensionID)
		return static_cast<HealthExt *>(this);
	if (id == SightExt ::ExtensionID)
		return static_cast<SightExt *>(this);
	if (id == UnitSpawnerExt::ExtensionID)
		return static_cast<UnitSpawnerExt *>(this);

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *BuildingController::QIActiveState(unsigned char stateid)
{
	// check if there is a current state
	if (m_pCurrentState == NULL)
		return NULL;

	// If the current state ID matches the asked for state, then return it
	if (m_pCurrentState->GetStateID() == stateid)
	{
		dbAssert(QIStateAll(stateid) == m_pCurrentState);
		return m_pCurrentState;
	}
	// If the asked for state is the Current state then return it
	else if (stateid == State::SID_Current)
	{
		return m_pCurrentState;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *BuildingController::QIStateAll(unsigned char stateid)
{
	if (stateid == State::SID_Current)
		return m_pCurrentState;
	if (stateid == StateIdle::StateID)
		return &m_stateidle;
	if (stateid == StateDead::StateID)
		return &m_statedead;

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::SetActiveState(unsigned char stateid)
{
	if (stateid == State::SID_NULLState)
	{
		m_pCurrentState = NULL;
		return;
	}

	State *pState = QIStateAll(stateid);
	dbAssert(pState);

	m_pCurrentState = pState;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::Save(BiFF &biff) const
{
	// call base class
	ModController::Save(biff);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::Load(IFF &iff)
{
	// call base class
	ModController::Load(iff);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long BuildingController::HandleLECD(IFF &, ChunkNode *, void *, void *)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::CancelBuildUnit(unsigned long unitIndex)
{
	// validate parm
	if (unitIndex >= BuildQueueSize())
		// old command -- ignore
		return;

	//
	BuildQueueRmv(unitIndex);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void BuildingController::NotifyHealthGone()
{
	m_commandproc.MakeDead();
}