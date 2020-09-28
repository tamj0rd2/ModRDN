/////////////////////////////////////////////////////////////////////
// File    : AttackEnums.h
// Desc    :
// Created : Saturday, May 05, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// different categories of attacks
enum AttackType
{
	ATTACKTYPE_NonRetaliate,
	ATTACKTYPE_Melee,

	ATTACKTYPE_COUNT
};

enum DamageType
{
	DT_None = 0,
	DT_Rock = (1 << 0),
	DT_Paper = (1 << 1),
	DT_Scissors = (1 << 2),
};
