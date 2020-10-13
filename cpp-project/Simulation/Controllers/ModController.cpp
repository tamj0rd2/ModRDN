/////////////////////////////////////////////////////////////////////
//	File	:
//	Desc.	:
//		12.Dec.00 (c) Relic Entertainment Inc.
//

#include "pch.h"

#include "ModController.h"

#include "../../ModObj.h"
#include "../RDNWorld.h"
#include "../RDNPlayer.h"
#include "../CommandProcessor.h"
#include "../RDNQuery.h"
#include "../CommandTypes.h"

#include "../Extensions\ExtensionTypes.h"
#include "../Extensions\HealthExt.h"
#include "../Extensions\ModifierExt.h"

#include <SimEngine/EntityDynamics.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/EntityCommand.h>

#include <Util/Iff.h>
#include <Util/Biff.h>

#include <ModInterface/ECStaticInfo.h>

/////////////////////////////////////////////////////////////////////
// Local functions

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
ModController::ModController(Entity *e, EntityDynamics *pDynamics, const ECStaticInfo *pStatinf)
		: SimController(e, pDynamics),
			m_pStatinf(pStatinf),
			m_pCommandProc(NULL)
{
	dbAssert(e);
	dbAssert(pStatinf);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
ModController::~ModController()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ControllerType ModController::GetControllerType() const
{
	return ControllerType(GetECStaticInfo()->GetControllerBlueprint()->GetControllerType());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
Extension *ModController::QI(unsigned char)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *ModController::QIActiveState()
{
	return QIActiveState(State::SID_Current);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *ModController::QIActiveState(unsigned char)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::SimulatePre()
{
	HealthExt *pHealthExt = static_cast<HealthExt *>(QIAll(HealthExt::ExtensionID));
	if (pHealthExt)
	{
		pHealthExt->RefreshAnimator();
	}

	if (GetEntityDynamics())
		GetEntityDynamics()->OnConstructed(); // let the dynamics know this Entity is constructed.  Note: must come after the Pathfinder::SimulatePre()
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
Extension *ModController::QIAll(unsigned char)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *ModController::QIStateAll(unsigned char)
{
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::SetActiveState(unsigned char stateid)
{
	UNREF_P(stateid);

	dbAssert(stateid != State::SID_Current);

	dbBreak();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::SetCommandProcessor(CommandProcessor *pCommandProc)
{
	dbAssert(m_pCommandProc == NULL);
	dbAssert(pCommandProc);
	m_pCommandProc = pCommandProc;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::Save(BiFF &biff) const
{
	SimController::Save(biff);

	// Query for each extesion and then save it
	for (unsigned char id = EXTID_Begin; id < EXTID_Count; ++id)
	{
		const Extension *pExtension = QIAll(id);

		if (pExtension)
		{
			pExtension->SaveExt(biff);
		}
	}

	// If we have a current state, then we need to save it, if we don't have
	// one, then we don't have any states to save.
	const State *pState = QIActiveState();
	if (pState)
	{
		IFF &iff = *biff.GetIFF();

		biff.StartChunk(Type_NormalVers, 'MCEC', "Controller: ModController", 1);

		unsigned char stateid = (unsigned char)pState->GetStateID();
		IFFWrite(iff, stateid);

		// Save out the state info
		pState->SaveState(biff);

		biff.StopChunk();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::Load(IFF &iff)
{
	SimController::Load(iff);

	// Query for each extesion and then Load it
	for (unsigned char id = EXTID_Begin; id < EXTID_Count; ++id)
	{
		Extension *pExtension = QIAll(id);

		if (pExtension)
		{
			pExtension->LoadExt(iff);
		}
	}

	iff.AddParseHandler(HandleMCEC, Type_NormalVers, 'MCEC', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long ModController::HandleMCEC(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	ModController *pModController = static_cast<ModController *>(pContext1);
	dbAssert(pModController);

	unsigned char stateid;
	IFFRead(iff, stateid);

	// Restore the state properly
	State *pState = pModController->QIStateAll(stateid);
	dbAssert(pState);
	if (pState)
	{
		if (pModController->QIActiveState())
		{
			pModController->QIActiveState()->ForceExit();
		}

		pState->LoadState(iff);

		pModController->SetActiveState(stateid);

		// make sure that we loaded the active state properly
		dbAssert(pState == pModController->QIActiveState());
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::OnSpawnEntity()
{
	// inherited
	SimController::OnSpawnEntity();

	if (m_pCommandProc)
	{
		m_pCommandProc->OnSpawnEntity();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::OnDeSpawnEntity()
{
	// inherited
	SimController::OnDeSpawnEntity();

	if (m_pCommandProc)
	{
		m_pCommandProc->OnDeSpawnEntity();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool ModController::CommandDoProcessNow(const EntityCommand *pEC)
{
	dbTracef("ModController::CommandDoProcessNow");

	if (m_pCommandProc)
		return m_pCommandProc->CommandDoProcessNow(pEC);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool ModController::CommandIsClearQueue(const EntityCommand *pEC) const
{
	if (m_pCommandProc)
		return m_pCommandProc->CommandIsClearQueue(pEC);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool ModController::Update(const EntityCommand *pEC)
{
	if (pEC)
	{
		dbTracef("ModController::Update");
	}

	if (m_pCommandProc)
		return m_pCommandProc->Update(pEC);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ModController::Execute()
{
}
