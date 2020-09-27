/////////////////////////////////////////////////////////////////////
// File    : WarriorExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h" 
#include "../AttackTypes.h"

///////////////////////////////////////////////////////////////////// 
// RaceExtInfo

class WarriorExtInfo : public ModStaticInfo::ExtInfo
{
// types
public:
	enum { ExtensionID = ModStaticInfo::EXTINFOID_Warrior };

// fields
public:
	enum WarriorID
	{
		WARRIOR_Rock,
		WARRIOR_Paper,
		WARRIOR_Scissors,
	};
	
	WarriorID	warriorID;

	//	AttackTypes.h/DamageType defines flags for these fields
	int		damagedBy;

// construction
public:
	WarriorExtInfo( const ControllerBlueprint* cbp )
	{
		damagedBy	= GetVal( cbp, "damagedBy", 0, DT_None|DT_Rock|DT_Paper|DT_Scissors );
	}
};
