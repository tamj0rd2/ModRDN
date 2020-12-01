/////////////////////////////////////////////////////////////////////
// File    : AttackExt.cpp
// Desc    :
// Created : Tuesday, March 06, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "../RDNQuery.h"
#include "../RDNTuning.h"
#include "../AttackPackage.h"

#include "../Controllers/ModController.h"

#include "AttackExt.h"
#include "SightExt.h"
#include "ModifierExt.h"

#include "../ExtInfo/AttackExtInfo.h"

#include <SimEngine/Entity.h>

#include <Util/Biff.h>
#include <Util/Iff.h>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
AttackExt::AttackExt()
{
	m_Multiplier = 1.0f;
	m_MultCount = 0;

	m_meleeMultiplier = 1.0f;
	m_meleeCount = 0;
	m_meleeBonus = 0.0f;

	m_artilleryMultiplier = 1.0f;
	m_artilleryCount = 0;
	m_artilleryBonus = 0.0f;

	m_rangedMultiplier = 1.0f;
	m_rangedCount = 0;
	m_rangedBonus = 0.0f;

	m_splashDamageDefenseMult = 0.0f;
	m_areaAttackRadiusMult = 1.0f;

	m_pLastAttackPkg = NULL;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : does damage based on the AttackPackage's damagePerHit
// Result    :
// Param.    :
// Author    :
//
void AttackExt::DoDamageTo(const AttackPackage &attack, Entity *pTarget)
{
	// remember the attack package
	m_pLastAttackPkg = &attack;

	// compute the damage per hit with all the multipliers
	float damagePerHit;
	float damageBonus;
	AttackDamagePerHit(attack, pTarget, damagePerHit, damageBonus);

	OnDoDamageTo(damagePerHit, damageBonus, attack, pTarget);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: does damage based on numSeconds and the AttackPackage's damagePerSec
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void AttackExt::DoDamageToTimeBased(const AttackPackage &attack, const float numSeconds, Entity *pTarget)
{
	// remember the attack package
	m_pLastAttackPkg = &attack;

	// calculate damage multiplier and damage bonus
	float damageMultiplier, damageBonus;
	AttackDamageModifiers(attack, pTarget, damageMultiplier, damageBonus);

	// calculate the amount of damage this attack did
	float damagePerHit = attack.m_damagePerSec * numSeconds;

	// accummulate the damage bonus due to damage
	damageBonus += (damagePerHit * (damageMultiplier - 1.0f));

	OnDoDamageTo(damagePerHit, damageBonus, attack, pTarget);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: calculates the attacks damage multiplier and damage bonus.
//				These values should then be used to calculate the final damage as follows...
//					finalDamage = initialDamage * multiplier + bonus;
//	Result	:
//	Param.	:
//
// TODO: implement this
void AttackExt::AttackDamageModifiers(const AttackPackage &attack, const Entity *pTarget,
																			float &multiplier, float &bonus) const
{
	multiplier = m_Multiplier;
	bonus = 0;

	// apply damage specific multipliers
	if (attack.m_type == ATTACKTYPE_Melee)
	{
		multiplier += (m_meleeMultiplier - 1.0f);
	}

	// apply damage specific bonuses
	if (attack.m_type == ATTACKTYPE_Melee)
	{
		bonus += m_meleeBonus;
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void AttackExt::AttackDamagePerHit(const AttackPackage &attack, Entity *pTarget, float &damagePerHit, float &damageBonus) const
{
	// calculate damage multiplier and damage bonus
	float multiplier, bonus;
	AttackDamageModifiers(attack, pTarget, multiplier, bonus);

	//
	damagePerHit = attack.m_damagePerHit;

	// compute damage bonus
	damageBonus = (damagePerHit * (multiplier - 1.0f)) + bonus;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::DoTriggeredAttack(const AttackPackage &attack, Entity *pTarget)
{
	// remember the attack package
	m_pLastAttackPkg = &attack;

	OnDoTriggeredAttack(attack, pTarget);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::AddDamageMultiplier(float multiplier)
{
	// Note: applying N damage-multipliers, m_i, has the following interpretation.
	//
	//		net_multiplier = 1.0 + sum( from i=0, to i=N-1, m_i - 1 );
	//
	// e.g. if m_0 = 1.5 and m_1 = 2.0, then applying both multipliers would give
	//		net_multiplier = 1.0 + (0.5 + 1.0) = 2.5
	//

	m_Multiplier += (multiplier - 1.0f);
	m_MultCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::RemDamageMultiplier(float multiplier)
{
	m_Multiplier -= (multiplier - 1.0f);
	m_MultCount--;
	if (m_MultCount == 0)
	{
		m_Multiplier = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::AddMeleeDamageMultiplier(const float multiplier)
{
	m_meleeMultiplier += (multiplier - 1.0f);
	m_meleeCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::RemMeleeDamageMultiplier(const float multiplier)
{
	m_meleeMultiplier -= (multiplier - 1.0f);
	m_meleeCount--;
	if (m_meleeCount == 0)
	{
		m_meleeMultiplier = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SetMeleeDamageBonus(const float bonus)
{
	m_meleeBonus = bonus;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::ClearMeleeDamageBonus(void)
{
	m_meleeBonus = 0.0f;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::AddArtilleryDamageMultiplier(const float multiplier)
{
	m_artilleryMultiplier += (multiplier - 1.0f);
	m_artilleryCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::RemArtilleryDamageMultiplier(const float multiplier)
{
	m_artilleryMultiplier -= (multiplier - 1.0f);
	m_artilleryCount--;
	if (m_artilleryCount == 0)
	{
		m_artilleryMultiplier = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SetArtilleryDamageBonus(const float bonus)
{
	m_artilleryBonus = bonus;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::ClearArtilleryDamageBonus(void)
{
	m_artilleryBonus = 0.0f;
}

// ***********

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::AddRangedDamageMultiplier(const float multiplier)
{
	m_rangedMultiplier += (multiplier - 1.0f);
	m_rangedCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::RemRangedDamageMultiplier(const float multiplier)
{
	m_rangedMultiplier -= (multiplier - 1.0f);
	m_rangedCount--;
	if (m_rangedCount == 0)
	{
		m_rangedMultiplier = 1.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SetRangedDamageBonus(const float bonus)
{
	m_rangedBonus = bonus;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::ClearRangedDamageBonus(void)
{
	m_rangedBonus = 0.0f;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SaveExt(BiFF &biff) const
{
	biff.StartChunk(Type_NormalVers, 'EATT', "Attack Extension", 3);

	IFF &iff = *biff.GetIFF();

	IFFWrite(iff, m_meleeMultiplier);
	IFFWrite(iff, m_meleeCount);
	IFFWrite(iff, m_meleeBonus);

	IFFWrite(iff, m_artilleryMultiplier);
	IFFWrite(iff, m_artilleryCount);
	IFFWrite(iff, m_artilleryBonus);

	IFFWrite(iff, m_rangedMultiplier);
	IFFWrite(iff, m_rangedCount);
	IFFWrite(iff, m_rangedBonus);

	IFFWrite(iff, m_splashDamageDefenseMult);

	IFFWrite(iff, m_areaAttackRadiusMult);

	IFFWrite(iff, m_Multiplier);
	IFFWrite(iff, m_MultCount);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleEATT, Type_NormalVers, 'EATT', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long AttackExt::HandleEATT(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	long version = iff.GetNormalVersion();

	AttackExt *pAttack = static_cast<AttackExt *>(pContext1);

	// save the states that are affected by research and upgrade
	IFFRead(iff, pAttack->m_meleeMultiplier);
	if (version >= 3)
	{
		IFFRead(iff, pAttack->m_meleeCount);
	}

	IFFRead(iff, pAttack->m_meleeBonus);

	if (version >= 3)
	{
		IFFRead(iff, pAttack->m_artilleryMultiplier);
		IFFRead(iff, pAttack->m_meleeCount);
		IFFRead(iff, pAttack->m_artilleryBonus);
	}

	IFFRead(iff, pAttack->m_rangedMultiplier);
	if (version >= 3)
	{
		IFFRead(iff, pAttack->m_rangedCount);
	}

	IFFRead(iff, pAttack->m_rangedBonus);

	IFFRead(iff, pAttack->m_splashDamageDefenseMult);

	IFFRead(iff, pAttack->m_areaAttackRadiusMult);

	//
	if (version >= 2)
	{
		IFFRead(iff, pAttack->m_Multiplier);
		IFFRead(iff, pAttack->m_MultCount);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float AttackExt::GetAttackSearchRadius() const
{
	float attackRange = 0;

	const AttackExtInfo *pAttackExtInfo = QIExtInfo<AttackExtInfo>(GetSelf());
	if (pAttackExtInfo)
	{
		attackRange = pAttackExtInfo->attackInfo.maxRange;
	}

	const SightExt *pSight = QIExt<SightExt>(GetSelf());
	if (pSight)
	{
		// the plus 4.0 is there to compensate for the discretization of the sight radius in the
		// FOW system
		attackRange = std::max(pSight->GetSightRadius() + 4.0f, attackRange);
	}

	return attackRange;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SetSplashDmgDefenseMult(float multiplier)
{
	m_splashDamageDefenseMult = multiplier;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::ClearSplashDmgDefenseMult()
{
	m_splashDamageDefenseMult = 0.0f;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float AttackExt::GetSplashDmgDefenseMult() const
{
	return m_splashDamageDefenseMult;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::SetAreaAttackRadiusMult(float multiplier)
{
	m_areaAttackRadiusMult = multiplier;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void AttackExt::ClearAreaAttackRadiusMult()
{
	m_areaAttackRadiusMult = 1.0f;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float AttackExt::GetAreaAttackRadiusMult() const
{
	return m_areaAttackRadiusMult;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
EntityMemory &AttackExt::GetUnreachableMemory()
{
	return m_unreachables;
}
