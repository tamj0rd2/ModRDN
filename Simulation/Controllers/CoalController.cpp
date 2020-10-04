#include "pch.h"
#include "CoalController.h"

#include "../../ModObj.h"
#include "../GameEventDefs.h"
#include "../CommandTypes.h"
#include "../RDNPlayer.h"
#include "../RDNTuning.h"
#include "../RDNQuery.h"

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
			ModStaticInfo(cbp)
{
}

const ModStaticInfo::ExtInfo *
CoalController::StaticInfo::QInfo(unsigned char id) const
{
	if (id == SiteExtInfo ::ExtensionID)
		return static_cast<const SiteExtInfo *>(this);
	if (id == UIExtInfo::ExtensionID)
		return static_cast<const UIExtInfo *>(this);

	return 0;
}

CoalController::CoalController(Entity *pEntity, const ECStaticInfo *pStaticInfo)
		: ModController(pEntity, new GroundDynamics(static_cast<SimEntity *>(pEntity)), pStaticInfo)

{
	// init the Command Processor
	// m_commandproc.Init(this);
	// SetCommandProcessor(&m_commandproc);
	return;
}

CoalController::~CoalController()
{
}

ModController *CoalController::GetSelf()
{
	return this;
}
