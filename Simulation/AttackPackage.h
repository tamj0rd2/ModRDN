/////////////////////////////////////////////////////////////////////
// File    : CreatureExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "AttackTypes.h"

#include "CommandTypes.h"

///////////////////////////////////////////////////////////////////// 
// Basic AttackPackage.  This info is used by StateAttack
// in order to know how to attack

struct AttackPackage
{
	AttackType					m_type;
			
	float						m_damagePerSec;
	float						m_damagePerHit;
	long						m_attackticks;
	long						m_coolticks;
	DamageType					m_dmgType;
	
	// extra stuff needed for triggers		
	float						m_minRange;
	float						m_maxRange;
	float						m_rangesqr;

	// extra stuff needed for triggers
	bool						m_bHasAnimation;
	CommandArgumentType			m_argTypes;

	// anim info
	char						m_Animation[32];
};

// typedefs
typedef std::vector<AttackPackage>	AttackPackageList;

///////////////////////////////////////////////////////////////////// 
// This is a container for all the different attacks that a
// controller can have.

struct AttackInfoPackage
{
	// attack info lists
	AttackPackageList			meleeList;
	bool						hasAttack;
	
	// this is the max range this unit can attack to (3m for melee or 9m sq )
	float						maxRange;
};