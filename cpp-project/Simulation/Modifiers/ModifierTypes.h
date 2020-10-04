/////////////////////////////////////////////////////////////////////
// File    : Modifier.h
// Desc    :
// Created : Wednesday, June 27, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

/////////////////////////////////////////////////////////////////////
// The order of this list is VERY important for the savegame.
// Make sure to not to modify this

enum ModifierType
{
	MID_Begin = 1,

	MID_Frenzy = 1,
	MID_Leap,
	MID_Charge,
	MID_Plague,
	MID_Poison,
	MID_Sabotage,
	MID_SonicDmg,
	MID_Stealth,
	MID_Stink,
	MID_StinkProtect,
	MID_VenomSpray,
	MID_Loner,
	MID_AutoDefense,
	MID_Webbed,
	MID_Flash,
	MID_Infested,
	MID_Swamp,
	MID_SoiledLand,
	MID_SoiledDmg,

	MID_Count,
};
