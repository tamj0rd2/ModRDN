/////////////////////////////////////////////////////////////////////
//	File	: ModTriggerTypes.h
//	Desc.	: Types that are used by Mod triggers
//		14.Feb.02 (c) Relic Entertainment Inc.
//
#pragma once

namespace ModTriggerTypes
{
	//----------------------------------------
	// Hud types
	// order is VERY IMPORTANT
	enum ModTrigerHudType
	{
		HT_MaxNumHudTypes
	};

	//----------------------------------------
	// entity command types
	// order is VERY IMPORTANT
	enum ModTriggerEntityCmdType
	{
		CMD_Move,
		CMD_Attack,
		CMD_AttackMove,
		CMD_RallyPoint,
		CMD_DefaultAction,

		CMD_MaxNumCmdTypes
	};

	//----------------------------------------
	// objective state
	// order is VERY IMPORTANT
	enum ObjectiveState
	{
		OS_Off,
		OS_Incomplete,
		OS_Complete,
		OS_Failed,

		OS_NumStates
	};
} // namespace ModTriggerTypes
