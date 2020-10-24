/////////////////////////////////////////////////////////////////////
//	File	:
//	Desc.	:
//		11.Dec.00 (c) Relic Entertainment Inc.
//
#pragma once

/////////////////////////////////////////////////////////////////////
// list of simulation commands
// these form the content of the game/sync cmds

#include <EngineAPI/CommandInterface.h>

enum CommandArgumentType
{
	CAT_Self = 0x01,
	CAT_Point = 0x02,
	CAT_Entity = 0x04,
};

enum CommandType
{
	// NOTE: Do not edit the order of this list at threat of death.
	// The save game will no longer work if the order changes, just add to the end

	// The ID for the default action
	// this CommandID can be passed to any of the EntityController command functions.
	CMD_DefaultAction = CommandInterface::CT_DefaultCommand,

	// These are the Simple command ID's  they are received in the
	// MOD_DoEntity function, and passed to the EntityController DoEntity function
	CMD_Stop = 1,		 // cancel last action, and stop the unit
	CMD_Destroy = 2, // scuttle this unit

	CMD_BuildUnit = 3,
	CMD_CancelBuildUnit = 4,

	CMD_Move = 5,				// move command
	CMD_Attack = 6,			// attack command
	CMD_AttackMove = 7, // attack move command, same as Starcraft
	CMD_RallyPoint = 8, // set a rally point

	CMD_Pause = 9,
	CMD_UnPause = 10,

	CMD_Gather = 11, // gather from a resource
	CMD_Unload = 12, // assuming this means unloading a gyrocopter

	// TODO: create a new PlayerEntityCommandType
	CMD_BuildBuilding = 13, // build a building

	CMD_COUNT
};

enum PlayerCommandType
{
	// NOTE: Do not edit the order of this list at threat of death.
	// The save game will no longer work if the order changes, just add to the end

	PCMD_ChangeSharedVision,
	PCMD_ChangeAlliance,
	PCMD_DonationCash,
	PCMD_CheatCash,
	PCMD_CheatKillSelf,
};

enum CommandFlags
{
	// NOTE: Do not edit the order of this list at threat of death.
	// The save game will no longer work if the order changes, just add to the end

	CMDF_Queue = 1 << 0,
};
