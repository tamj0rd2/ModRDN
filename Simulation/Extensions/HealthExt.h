/////////////////////////////////////////////////////////////////////
// File    : HealthExtension.h
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"
#include "../AttackTypes.h"
#include "../AttackMemory.h"

#include <SimEngine/EntityGroup.h>

struct AttackPackage;
class HealthExtInfo;
class ControllerBlueprint;

///////////////////////////////////////////////////////////////////////////////
// HealthExt

class HealthExt : public Extension
{
	// types
public:
	enum
	{
		ExtensionID = EXTID_Health,
	};

	// Health attribute bit-field
	// This bit-field is used to prune-out unnecessary checks and computations
	enum HealthAttribute
	{
		HA_Building = 1,
		HA_HasPoisonTouch = 1 << 1,

		HA_IsHenchman = 1 << 4,
		HA_IsFence = 1 << 5,
		HA_IsRex = 1 << 6,

		HA_ProtectedByStinkDome = 1 << 8,

		HA_ImmuneToStink = 1 << 9,
		HA_ImmuneToPlague = 1 << 10,
		HA_ImmuneToAll = 1 << 11,

		HA_AffectedByPoison = 1 << 16,
		HA_AffectedBySonic = 1 << 17,
		HA_AffectedByStink = 1 << 18,
		HA_AffectedBySabotage = 1 << 19,
		HA_AffectedByPlague = 1 << 20,
	};

	// interface
public:
	// returns maximum health (default implementation queries 'HealthExtInfo')
	virtual float GetHealthMax() const;

	virtual float GetHealth() const;
	virtual void SetHealth(float amount);

	void DecHealth(float amount);

	// return actual dmg applied
	float ApplyRepair(const float amount);
	float ApplyDamage(const float amount, const float damageBonus, const DamageType type, const AttackType atype, Entity *pAttacker);

	// apply full damage without consideration for attack or defense values
	void ApplyFullDamage(const float amount, const DamageType type, const Entity *pAttacker);

	// Allows derived classes to implement damage specific FX, etc..
	virtual void OnApplyDamage(const float amountdone, const DamageType type);

	void SetUnderConstruction();
	void ClearUnderConstruction();

	virtual void SelfDestroy();

	void RefreshAnimator();

	//
	virtual bool ImmuneTo(DamageType type) const;
	virtual bool ReceivesNoDamageFrom(const DamageType type, const AttackType attacktype, const Entity *pAttacker) const;

	// AttackMemory functions
	const AttackMemory &GetAttackMemory() const;

	void ResetHealthMax();
	virtual void SetHealthMax(float max);

	// Attribute bits
	void SetHealthAttribute(const int mask);
	void SetHealthAttributeBit(const HealthAttribute mask);
	void ClearHealthAttributeBit(const HealthAttribute mask);

private:
	virtual void OnHealthChange();
	virtual void OnHealthGone();

	// inherit from this if you want to overide the default behaviour of killing itself.
	virtual void NotifyHealthGone();

	// inherited interface: Extension
private:
	virtual void SaveExt(BiFF &) const;
	virtual void LoadExt(IFF &);

	// Chunk Handlers
private:
	static unsigned long HandleEHEL(IFF &, ChunkNode *, void *, void *);

protected:
	// return actual dmg applied
	float IncHealth(float amount);

private:
	float CalcBonus(float amount, float damageBonus, DamageType type, AttackType attacktype, Entity *attacker);
	bool UpdateDamageState(DamageType type, AttackType attacktype, Entity *attacker);
	float ComputeDefense(DamageType damagetype, AttackType attacktype, Entity *pAttacker);

	// construction
protected:
	HealthExt(Entity *e, const HealthExtInfo *info, bool bHasHealth = true);

	// fields
private:
	float m_maxHealth;
	float m_health;

	// decorator
	unsigned long m_decoratorHealth;

	// underconstruction, use this so that while we are being built we don't show damage.
	bool m_bUnderConstruction;

	// remember the last entity that attacked this (so we can keep track of who killed this for stats)
	EntityGroup m_lastAttacker;
	AttackMemory m_attackMemory; // FIX: m_lastAttacker is no longer needed, the same info should be stored in the AttackMemory

	int m_attributeMask;
};
