/////////////////////////////////////////////////////////////////////
// File    : HealthExtension.cpp
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "HealthExt.h"

#include "../../ModObj.h"
#include "../../RaceTypes.h"

#include "../RDNWorld.h"
#include "../RDNPlayer.h"
#include "../RDNTuning.h"
#include "../GameEventDefs.h"

#include "../Controllers/ModController.h"

#include "AttackExt.h"
#include "ModifierExt.h"

#include "../ExtInfo/HealthExtInfo.h"
#include "../ExtInfo/SiteExtInfo.h"

#include <EngineAPI/ControllerBlueprint.h>
#include <EngineAPI/SoundInterface.h>

#include <SimEngine/Entity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/TerrainHMBase.h>

#include <Util/Biff.h>
#include <Util/Iff.h>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
HealthExt::HealthExt(Entity *e, const HealthExtInfo *info, bool bHasHealth) : m_maxHealth(0),
																																							m_health(0),
																																							m_bUnderConstruction(false),
																																							m_decoratorHealth(0),
																																							m_attributeMask(0)
{
	// animator
	if (e->GetAnimator() && bHasHealth)
	{
		// create the health bar for this entity
		m_decoratorHealth = e->GetAnimator()->CreateDecorator(EntityAnimator::DT_PowerBar, "\0\0", 0, EntityAnimator::DF_DrawSelected);
		e->GetAnimator()->SetMotionVariable("Damage", 0.0f);
	}

	m_maxHealth = info->health;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
const AttackMemory &HealthExt::GetAttackMemory() const
{
	return m_attackMemory;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float HealthExt::GetHealth() const
{
	return m_health;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SetHealth(float amount)
{
	if (amount < 0.0f)
	{
		amount = 0.0f;
	}

	if (m_health != amount)
	{
		m_health = amount;

		if (m_health > GetHealthMax())
			m_health = GetHealthMax();

		// event
		OnHealthChange();

		if (m_health <= 0.0f)
		{
			OnHealthGone();
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
void HealthExt::DecHealth(float amount)
{
	// validate parm
	dbAssert(amount >= 0.0f);

	//
	if (amount > m_health)
		amount = m_health;

	//
	if (amount > 0.0f)
	{
		SetHealth(m_health - amount);
	}

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::RefreshAnimator()
{
	HealthExt::OnHealthChange();
}

/////////////////////////////////////////////////////////////////////
// Desc.     : Check to see if an entity is immune to a type of damage
//             Note: immunity means don't apply the related modifier; it does not cancel damages.
// Result    :
// Param.    :
// Author    :
//
bool HealthExt::ImmuneTo(DamageType) const
{
	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : Check to see if an entity receives no damage from the given attack
// Result    :
// Param.    :
// Author    :
//
bool HealthExt::ReceivesNoDamageFrom(const DamageType damagetype, const AttackType attacktype, const Entity *pAttacker) const
{
	UNREF_P(damagetype);
	UNREF_P(pAttacker);
	UNREF_P(attacktype);

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SaveExt(BiFF &biff) const
{
	biff.StartChunk(Type_NormalVers, 'EHEL', "Health Extension", 2);

	IFF &iff = *biff.GetIFF();

	IFFWrite(iff, m_bUnderConstruction);
	IFFWrite(iff, m_health);

	// save states that are modified by research and upgrades
	IFFWrite(iff, m_maxHealth);

	m_lastAttacker.SaveEmbedded(*biff.GetIFF());

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleEHEL, Type_NormalVers, 'EHEL', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long HealthExt::HandleEHEL(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	unsigned long version = iff.GetNormalVersion();

	HealthExt *pHealth = static_cast<HealthExt *>(pContext1);

	IFFRead(iff, pHealth->m_bUnderConstruction);
	IFFRead(iff, pHealth->m_health);

	if (version >= 2)
	{
		IFFRead(iff, pHealth->m_maxHealth);

		// health may be greater than max health if this entity received additional health through
		// addons
	}

	pHealth->m_lastAttacker.LoadEmbedded(iff, ModObj::i()->GetEntityFactory());

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float HealthExt::IncHealth(float amount)
{
	// validate parm
	dbAssert(amount >= 0.0f);

	//
	if (amount > GetHealthMax() - m_health)
		amount = GetHealthMax() - m_health;

	//
	if (amount > 0.0f)
	{
		SetHealth(m_health + amount);
	}

	return amount;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float HealthExt::ApplyRepair(const float amount)
{
	return IncHealth(amount);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float HealthExt::ApplyDamage(const float amount, const float damageBonus, const DamageType type, const AttackType attacktype, Entity *pAttacker)
{
	UNREF_P(attacktype);

	// Always remember the last attacker
	m_lastAttacker.clear();
	if (pAttacker)
	{
		m_lastAttacker.push_back(pAttacker);
		m_attackMemory.SetAttackedBy(pAttacker, ModObj::i()->GetWorld()->GetGameTicks());
	}

	float modifiedDmgAmount = amount + damageBonus;

	// Apply damage done after (possible) defense bonus
	ApplyFullDamage(modifiedDmgAmount, type, pAttacker);

	// Call controller handling of damage
	OnApplyDamage(modifiedDmgAmount, type);

	return (modifiedDmgAmount);
}

/////////////////////////////////////////////////////////////////////
// Desc.     : Ignores any attack or defense modifiers and applies
//             the full damage amount.
// Result    :
// Param.    :
// Author    :
//
void HealthExt::ApplyFullDamage(const float amount, const DamageType type, const Entity *pAttacker)
{
	// send an event to say that someone is being attacked
	const Entity *thisEntity = GetSelf()->GetEntity();
	const RDNPlayer *owner = static_cast<const RDNPlayer *>(thisEntity->GetOwner());

	GameEventSys::Instance()->PublishEvent(
			GameEvent_PlayerBeingAttacked(owner, pAttacker, thisEntity, amount, type));

	DecHealth(amount);
}

/////////////////////////////////////////////////////////////////////
// Desc.     : Default imp.
// Result    :
// Param.    :
// Author    :
//
void HealthExt::OnApplyDamage(const float amountdone, const DamageType type)
{
	UNREF_P(amountdone);
	UNREF_P(type);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SetUnderConstruction()
{
	m_bUnderConstruction = true;
	if (GetSelf()->GetEntity()->GetAnimator())
	{
		// Set to not damaged
		GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Damage", 0.0f);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::ClearUnderConstruction()
{
	m_bUnderConstruction = false;
	if (GetSelf()->GetEntity()->GetAnimator())
	{
		float health = GetHealth() / GetHealthMax();
		// Set to current damage level
		GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Damage", 1.0f - health);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::OnHealthChange()
{
	if (GetSelf()->GetEntity()->GetAnimator())
	{
		float health = GetHealth() / GetHealthMax();
		GetSelf()->GetEntity()->GetAnimator()->UpdateDecorator(m_decoratorHealth, health);
		if (!m_bUnderConstruction)
		{
			GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Damage", 1.0f - health);
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
void HealthExt::OnHealthGone()
{
	//
	Entity *thisEntity = GetSelf()->GetEntity();

	//
	const RDNPlayer *owner = static_cast<const RDNPlayer *>(thisEntity->GetOwner());

	//
	GameEventSys::Instance()->PublishEvent(
			GameEvent_EntityKilled(owner, thisEntity, m_lastAttacker.front()));

	//
	NotifyHealthGone();

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::NotifyHealthGone()
{
	// kill this entity
	ModObj::i()->GetWorld()->DeleteEntity(GetSelf()->GetEntity());

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SelfDestroy()
{
	//
	m_lastAttacker.clear();

	//
	DecHealth(GetHealth());

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float HealthExt::GetHealthMax() const
{
	const RDNPlayer *pPlayer =
			static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());
	if (pPlayer != NULL)
	{
		return m_maxHealth * pPlayer->GetRaceBonusHealthMax(GetSelf()->GetEntity()->GetControllerBP());
	}

	return m_maxHealth;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : reset the maximum health to that stored in HealthExtInfo
// Result    :
// Param.    :
// Author    :
//
void HealthExt::ResetHealthMax()
{
	m_maxHealth = 0.0f;

	const HealthExtInfo *pInfo = QIExtInfo<HealthExtInfo>(GetSelf());
	if (pInfo != NULL)
	{
		m_maxHealth = pInfo->health;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     : set the maximum health
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SetHealthMax(float max)
{
	dbAssert(max > 0.0f);

	m_maxHealth = max;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : set the damage mask so that only certain types of immunities are checked
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SetHealthAttribute(const int mask)
{
	m_attributeMask = mask;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : set one of the immunity bits
// Result    :
// Param.    :
// Author    :
//
void HealthExt::SetHealthAttributeBit(const HealthAttribute mask)
{
	m_attributeMask |= mask;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : set the damage mask so that only certain types of immunities are checked
// Result    :
// Param.    :
// Author    :
//
void HealthExt::ClearHealthAttributeBit(const HealthAttribute mask)
{
	if (m_attributeMask & mask)
	{
		m_attributeMask = m_attributeMask ^ mask;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     : update states that may are affected by the damage
// Result    : return true if no damage should be applied immediately
// Param.    :
// Author    :
//
bool HealthExt::UpdateDamageState(DamageType type, AttackType attacktype, Entity *attacker)
{
	UNREF_P(attacker);
	UNREF_P(attacktype);

	if (ImmuneTo(type))
	{
		return false;
	}

	//	Attach any modifiers that are the result of an attack

	return false;
}
