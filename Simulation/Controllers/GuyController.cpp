/////////////////////////////////////////////////////////////////////
// File    : GuyController.cpp
// Desc    :
// Created : Wednesday, February 21, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "GuyController.h"

#include "../../ModObj.h"
#include "../GameEventDefs.h"
#include "../CommandTypes.h"
#include "../RDNPlayer.h"
#include "../RDNTuning.h"
#include "../RDNQuery.h"

#include "../Extensions/ResourceExt.h"

#include <EngineAPI/EntityFactory.h>
#include <EngineAPI/ControllerBlueprint.h>

#include <SimEngine/Entity.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/EntityGroup.h>
#include <SimEngine/EntityCommand.h>

#include <SimEngine/GroundDynamics.h>

#include <Util/Biff.h>
#include <Util/Iff.h>

#include <SurfVol/OBB3.h>

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
GuyController::StaticInfo::StaticInfo(const ControllerBlueprint *cbp)
		: MovingExtInfo(cbp),
			AttackExtInfo(cbp),
			HealthExtInfo(cbp),
			SightExtInfo(cbp),
			CostExtInfo(cbp),
			ModStaticInfo(cbp),
			UIExtInfo(cbp)
{
}

const ModStaticInfo::ExtInfo *
GuyController::StaticInfo::QInfo(unsigned char id) const
{
	if (id == MovingExtInfo ::ExtensionID)
		return static_cast<const MovingExtInfo *>(this);
	if (id == AttackExtInfo ::ExtensionID)
		return static_cast<const AttackExtInfo *>(this);
	if (id == HealthExtInfo ::ExtensionID)
		return static_cast<const HealthExtInfo *>(this);
	if (id == CostExtInfo ::ExtensionID)
		return static_cast<const CostExtInfo *>(this);
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
GuyController::GuyController(Entity *pEntity, const ECStaticInfo *pStaticInfo)
		: ModController(pEntity, new GroundDynamics(static_cast<SimEntity *>(pEntity)), pStaticInfo),
			HealthExt(static_cast<Entity *>(pEntity), QIExtInfo<HealthExtInfo>(pStaticInfo)),
			MovingExt(pEntity),
			SightExt(QIExtInfo<SightExtInfo>(pStaticInfo)),
			m_CurrentState(NULL),
			m_statemove(GetEntityDynamics()),
			m_stateidle(GetEntityDynamics()),
			m_statedead(GetEntityDynamics()),
			m_stateattack(GetEntityDynamics()),
			m_stateattackmove(GetEntityDynamics()),
			m_stategroupmove(GetEntityDynamics()),
			m_statepause(GetEntityDynamics()),
			m_bFirstUpdate(true)
{

	// Setup swim ability
	SetDynSwimmer(true);

	// Initialize all of the states that need to share instances of each other
	m_stateattack.Init(&m_statemove, QIExt<AttackExt>(this), &QIExtInfo<AttackExtInfo>(this)->attackInfo);
	m_stateattackmove.Init(&m_statemove, &m_stateattack);
	m_stategroupmove.Init(&m_statemove);
	m_statedead.UseWaterDeathAnim(true);

	// init the Command Processor
	m_commandproc.Init(this);
	SetCommandProcessor(&m_commandproc);

	// current health of the creature to its max hitpoints.
	//	SetHealth( GetHealthMax() ); // (moved this to Update)

	// setup health attributes
	SetHealthAttribute(HA_IsHenchman | HA_ProtectedByStinkDome | HA_AffectedByPoison | HA_AffectedBySonic | HA_AffectedByStink);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
GuyController::~GuyController()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool GuyController::Update(const EntityCommand *pEntCmd)
{
	// we can override the default behaviour of ModController if we like here

	// set the unit health on the first update
	if (m_bFirstUpdate)
	{
		SetHealth(GetHealthMax());
		m_bFirstUpdate = false;
	}

	// if henchman is stunned then return
	//if ( !m_commandproc.IsDead() && IsStinkStunned() )
	//	return false;

	bool ret = m_commandproc.Update(pEntCmd);

	// we can process more henchman specific commands here if we like.  It would
	// be better to add the code to ModController though.

	// If we processed a command and there was a command, send some notifications
	// I believe this is here purely for the AI, seems kinda haxor
	if (ret && pEntCmd)
	{
		switch (pEntCmd->GetCommandType())
		{
		case EntityCommand::CT_EntityPoint:
		{
			// publish that we have received this command
			GameEventSys::Instance()->PublishEvent(
					GameEvent_HenchmanEntityPointCmd(
							static_cast<const RDNPlayer *>(pEntCmd->GetPlayer()),
							GetEntity()));
		}
		break;
		case EntityCommand::CT_EntityEntity:
		{
			const EntityCommand_EntityEntity *pEntCmd_EE = static_cast<const EntityCommand_EntityEntity *>(pEntCmd);
			// publish that we have received this command
			GameEventSys::Instance()->PublishEvent(
					GameEvent_HenchmanEntityEntityCmd(
							static_cast<const RDNPlayer *>(pEntCmd->GetPlayer()),
							GetEntity(),
							pEntCmd_EE->GetTargets().front()));
		}
		break;
		case EntityCommand::CT_Entity:
		{
			// publish that we have received this command
			GameEventSys::Instance()->PublishEvent(
					GameEvent_HenchmanEntityCmd(
							static_cast<const RDNPlayer *>(pEntCmd->GetPlayer()),
							GetEntity()));
		}
		break;
		}
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::Execute()
{
	// forward to base class
	ModController::Execute();

	// Execute() is called once per sim step.

	if (m_commandproc.IsDead())
		return;

	GetEntityDynamics()->DoStep(GetMovingSpeed(MovingExt::GetSpeed()));

	// Let my modifier extension run and update the active modifiers
	ModifierExt *pModExt = QIExt<ModifierExt>(this);
	if (pModExt)
	{
		pModExt->Execute();
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::NotifyHealthGone()
{
	// forward to ModController
	m_commandproc.MakeDead();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void GuyController::OnApplyDamage(const float amountdone, const DamageType type)
{
	UNREF_P(type);

	if (amountdone > 0.0f)
	{
		// start attack effect
		if (GetEntity()->GetAnimator())
		{
			const RDNTuning::EffectInfo &inf = RDNTuning::Instance()->GetEffectInfo();
			GetEntity()->GetAnimator()->AttachEffect(
					inf.impact.fx,
					inf.impact.location,
					inf.impact.count);

			// play flinch animation.
			if (GetEntityDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater)
				GetEntity()->GetAnimator()->SetMotionTreeNode("SwimStumbleAction");
			else
				GetEntity()->GetAnimator()->ResetStyle('HURT');
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
ModController *GuyController::GetSelf()
{
	return this;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
Extension *GuyController::QI(unsigned char id)
{
	if (m_commandproc.IsDead())
	{
		return NULL;
	}

	return GuyController::QIAll(id);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
Extension *GuyController::QIAll(unsigned char id)
{
	if (id == HealthExt ::ExtensionID)
		return static_cast<HealthExt *>(this);
	if (id == ModifierExt ::ExtensionID)
		return static_cast<ModifierExt *>(this);
	if (id == MovingExt ::ExtensionID)
		return static_cast<MovingExt *>(this);
	if (id == SightExt ::ExtensionID)
		return static_cast<SightExt *>(this);
	if (id == AttackExt ::ExtensionID)
		return static_cast<AttackExt *>(this);

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *GuyController::QIActiveState(unsigned char StateID)
{
	// check if there is a current state
	if (m_CurrentState == NULL)
		return NULL;

	// If the current state ID matches the asked for state, then return it
	if (m_CurrentState->GetStateID() == StateID)
	{
		dbAssert(QIStateAll(StateID) == m_CurrentState);
		return m_CurrentState;
	}
	// If the asked for state is the Current state then return it
	else if (StateID == State::SID_Current)
	{
		return m_CurrentState;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State *GuyController::QIStateAll(unsigned char StateID)
{
	if (StateID == State ::SID_Current)
		return m_CurrentState;

	if (StateID == StateIdle ::StateID)
		return &m_stateidle;
	if (StateID == StateMove ::StateID)
		return &m_statemove;
	if (StateID == StateDead ::StateID)
		return &m_statedead;
	if (StateID == StateAttack ::StateID)
		return &m_stateattack;
	if (StateID == StateAttackMove ::StateID)
		return &m_stateattackmove;
	if (StateID == StateGroupMove ::StateID)
		return &m_stategroupmove;
	if (StateID == StatePause ::StateID)
		return &m_statepause;
	return NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::SetActiveState(unsigned char stateid)
{
	dbAssert(stateid != State::SID_Current);

	if (stateid == State::SID_NULLState)
	{
		m_CurrentState = NULL;
		return;
	}

	State *pState = QIStateAll(stateid);
	dbAssert(pState);

	m_CurrentState = pState;

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::Save(BiFF &biff) const
{
	ModController::Save(biff);

	m_commandproc.Save(*biff.GetIFF());

	IFF &iff = *biff.GetIFF();

	iff.PushChunk(Type_NormalVers, 'GYEC', 0);

	iff.PopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::Load(IFF &iff)
{
	ModController::Load(iff);

	m_commandproc.Load(iff);

	iff.AddParseHandler(HandleGYEC, Type_NormalVers, 'GYEC', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long GuyController::HandleGYEC(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	UNREF_P(iff);

	GuyController *pGuyController = static_cast<GuyController *>(pContext1);
	dbAssert(pGuyController);

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::OnDoDamageTo(float damagePerHit, float damageBonus, const AttackPackage &attack, Entity *pTarget)
{
	// validate parm
	dbAssert(pTarget);
	//
	HealthExt *health = QIExt<HealthExt>(pTarget->GetController());
	if (health == 0)
	{
		dbBreak();
		return;
	}

	// its easier to do all the damage calcs and stuff in here, cuz we have access
	// to both attacker and targets controllers (which we should have)
	// apply the damage of the specified type to the target
	health->ApplyDamage(
			damagePerHit,
			damageBonus,
			attack.m_dmgType,
			attack.m_type,
			GetEntity() // attacking entity
	);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void GuyController::OnDoTriggeredAttack(const AttackPackage &, Entity *)
{
	// do nothing
}
