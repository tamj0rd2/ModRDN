#include "pch.h"
#include "CoalController.h"

#include "../../ModObj.h"
#include "../GameEventDefs.h"
#include "../CommandTypes.h"
#include "../RDNPlayer.h"
#include "../RDNTuning.h"
#include "../RDNQuery.h"
#include "../RDNWorld.h"

#include <EngineAPI/ControllerBlueprint.h>

#include <SimEngine/Entity.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/EntityGroup.h>
#include <SimEngine/EntityCommand.h>

#include <SimEngine/GroundDynamics.h>

#include <SurfVol/OBB3.h>

// TODO: make use of ResourceExt somehow
CoalController::StaticInfo::StaticInfo(const ControllerBlueprint *cbp)
		: UIExtInfo(cbp),
			SiteExtInfo(cbp),
			ModStaticInfo(cbp),
			ResourceExtInfo(cbp)
{
}

const ModStaticInfo::ExtInfo *
CoalController::StaticInfo::QInfo(unsigned char id) const
{
	if (id == SiteExtInfo::ExtensionID)
		return static_cast<const SiteExtInfo *>(this);

	if (id == UIExtInfo::ExtensionID)
		return static_cast<const UIExtInfo *>(this);

	if (id == ResourceExtInfo::ExtensionID)
		return static_cast<const ResourceExtInfo *>(this);

	return 0;
}

CoalController::CoalController(Entity *pEntity, const ECStaticInfo *pStaticInfo)
		: ModController(pEntity, SiteExtInfo::CreateDynamics(pEntity, QIExtInfo<SiteExtInfo>(pStaticInfo)), pStaticInfo),
			ResourceExt(),
			m_stateidle(GetEntityDynamics()),
			m_pCurrentState(NULL)

{
	SetResources(100);

	// init the Command Processor
	m_commandproc.Init(this);
	SetCommandProcessor(&m_commandproc);
	return;
}

CoalController::~CoalController()
{
}

ModController *CoalController::GetSelf()
{
	return this;
}

void CoalController::SetActiveState(unsigned char stateid)
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

Extension *CoalController::QI(unsigned char id)
{
	// If we do any dynamic detachment of an extension put that logic here
	if (m_commandproc.IsDead())
		return NULL;

	return CoalController::QIAll(id);
}

Extension *CoalController::QIAll(unsigned char id)
{
	if (id == ResourceExt::ExtensionID)
		return static_cast<ResourceExt *>(this);

	return NULL;
}

State *CoalController::QIActiveState(unsigned char stateid)
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

State *CoalController::QIStateAll(unsigned char stateid)
{
	if (stateid == State::SID_Current)
		return m_pCurrentState;
	if (stateid == StateIdle::StateID)
		return &m_stateidle;

	return NULL;
}

void CoalController::OnZeroResources()
{
	GetEntity()->ClearEntityFlag(EF_IsVisible);
	ModObj::i()->GetWorld()->DeSpawnEntity(GetEntity());
}

bool CoalController::Update(const EntityCommand *currentCommand)
{
	if (currentCommand)
	{
		dbTracef("Coal controller processing update");
	}

	return ModController::Update(currentCommand);
}
