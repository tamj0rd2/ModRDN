/////////////////////////////////////////////////////////////////////
// File    : AttackExt.h
// Desc    : 
// Created : Tuesday, March 06, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

#include <SimEngine/EntityMemory.h>

////////////////////////////////////////////////////////////////////
//	Forward Declarations
//
class Entity;
class SimEntity;
struct AttackPackage;

///////////////////////////////////////////////////////////////////////////////
// AttackExt

class AttackExt : public Extension
{
// types
public:
	enum
	{
		ExtensionID = EXTID_Attack,
	};

// interface
public:
	void					DoDamageTo( const AttackPackage& attack, Entity* pTarget );
	void					DoTriggeredAttack( const AttackPackage& attack, Entity* pTarget );
	void					AttackDamagePerHit( const AttackPackage& attack, Entity* pTarget, float& damagePerHit, float& damageBonus  ) const;

	void					DoDamageToTimeBased( const AttackPackage& attack, const float numSeconds, Entity* pTarget );
	void					DoDamageToTimeBased( const AttackPackage& attack, const float numSeconds, const Vec3f& pos );
	void					AttackDamageModifiers( const AttackPackage& attack, const Entity* pTarget, float& multiplier, float& bonus ) const;

	// general damage multpliers
	void					AddDamageMultiplier (float multiplier);
	void					RemDamageMultiplier (float multiplier);

	// melee damage multipliers and bonuses
	void					AddMeleeDamageMultiplier (const float multiplier);
	void					RemMeleeDamageMultiplier (const float multiplier);
	void					SetMeleeDamageBonus (const float bonus);
	void					ClearMeleeDamageBonus();

	// artillery damage multipliers and bonuses
	void					AddArtilleryDamageMultiplier (const float multiplier);
	void					RemArtilleryDamageMultiplier (const float multiplier);
	void					SetArtilleryDamageBonus (const float bonus);
	void					ClearArtilleryDamageBonus();

    // ranged damage multipliers and bonuses
	void					AddRangedDamageMultiplier (const float multiplier);
	void					RemRangedDamageMultiplier (const float multiplier);
	void					SetRangedDamageBonus (const float bonus);
	void					ClearRangedDamageBonus();

	// piercing splash damage
	void					SetSplashDmgDefenseMult (float multiplier);
	void					ClearSplashDmgDefenseMult();
	float					GetSplashDmgDefenseMult() const;

	// area-attack bonus
	void					SetAreaAttackRadiusMult (float multiplier);
	void					ClearAreaAttackRadiusMult();
	float					GetAreaAttackRadiusMult() const;

	const AttackPackage*	GetLastAttackPkg( ) const { return m_pLastAttackPkg; }

	float					GetAttackSearchRadius( ) const;

	EntityMemory&			GetUnreachableMemory( );

	// Interface for Charge
	void					DoChargeAttack( );
	void					StopChargeAttack( );
	bool					IsChargeAttackOn( );

	// Interface for Leap Attack
	void					DoLeapAttack( );
	void					StopLeapAttack( );
	bool					IsLeapAttackOn( );

// interface
private:

	// do an attack against the given target
	virtual void OnDoDamageTo( float damagePerHit, float damageBonus, const AttackPackage& attack, Entity* pTarget ) = 0;
	// do a triggered attack
	virtual void OnDoTriggeredAttack( const AttackPackage& attack, Entity* pTarget ) = 0;

// inherited interface: Extension
private:

	virtual void SaveExt( BiFF& ) const;
	virtual void LoadExt( IFF& );

// Chunk Handlers
private:
	
	static unsigned long HandleEATT( IFF&, ChunkNode*, void*, void* );

// construction
protected:
	AttackExt();

// Data
private:
	// general damage multiplier
	float					m_Multiplier;
	unsigned char			m_MultCount;

	// melee damage multiplier and bonus
	float					m_meleeMultiplier;
	unsigned char			m_meleeCount;
	float					m_meleeBonus;

	// artillery damage multiplier and bonus
	float					m_artilleryMultiplier;
	unsigned char			m_artilleryCount;
	float					m_artilleryBonus;

	// ranged damage multiplier and bonus
	float					m_rangedMultiplier;
	unsigned char			m_rangedCount;
	float					m_rangedBonus;

	// piercing splash damage
	float					m_splashDamageDefenseMult;

	// area-attack bonus
	float					m_areaAttackRadiusMult;
							
	const AttackPackage*	m_pLastAttackPkg;

	// memory of unreachable targets
	EntityMemory			m_unreachables;
};

