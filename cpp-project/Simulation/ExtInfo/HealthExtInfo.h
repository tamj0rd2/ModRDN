/////////////////////////////////////////////////////////////////////
// File    : HealthExtInfo.h
// Desc    :
// Created : Monday, March 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h"

/////////////////////////////////////////////////////////////////////
// HealthExtInfo

class HealthExtInfo : public ModStaticInfo::ExtInfo
{
	// types
public:
	enum
	{
		ExtensionID = ModStaticInfo::EXTINFOID_Health
	};

	// fields
public:
	float health;
	bool fadeAndDeleteWhenDead;
	bool stayInPathfindingAfterDead;

	// construction
public:
	HealthExtInfo(const ControllerBlueprint *cbp);
};
