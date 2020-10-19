/////////////////////////////////////////////////////////////////////
// File    : ResourceExtInfo.h
// Desc    :
// Created : Monday, March 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h"

/////////////////////////////////////////////////////////////////////
// ResourceExtInfo

class ResourceExtInfo : public ModStaticInfo::ExtInfo
{
	// types
public:
	enum
	{
		ExtensionID = ModStaticInfo::EXTINFOID_Resource
	};

	// fields
public:
	float resourceMax;
	int maxGatherers;

	// construction
public:
	ResourceExtInfo(const ControllerBlueprint *cbp)
	{
		resourceMax = GetVal(cbp, "max_resources", 1.0f, 10000.0f);
		maxGatherers = GetVal(cbp, "max_henchmen", 1);
		dbTracef("Max gatherers: %d", maxGatherers);
	}
};
