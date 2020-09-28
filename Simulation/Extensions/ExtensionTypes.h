/////////////////////////////////////////////////////////////////////
// File    : ExtensionTypes.h
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//
#pragma once

///////////////////////////////////////////////////////////////////////////////
//  ExtensionTypes

// NOTE: Do not edit the order of this list at threat of death.
// The save game will no longer work if the order changes, just add to the end
enum ExtensionList
{
	EXTID_Begin = 1,

	EXTID_Health = 1,
	EXTID_Resource,
	EXTID_Attack,
	EXTID_UnitSpawner,
	EXTID_Modifier,
	EXTID_Moving,
	EXTID_Sight,
	EXTID_Gather,
	EXTID_Gatherer,

	EXTID_Count,
};
