/////////////////////////////////////////////////////////////////////
// File    : HealthExtInfo.cpp
// Desc    :
// Created : Monday, March 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "HealthExtInfo.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"

/////////////////////////////////////////////////////////////////////
// HealthExtInfo

HealthExtInfo::HealthExtInfo(const ControllerBlueprint *cbp)
{
	health = GetVal(cbp, "hitpoints", 1.0f, 100000.0f);
	fadeAndDeleteWhenDead = GetVal(cbp, "fadeAndDeleteWhenDead", true);

	// stayInPathfindingAfterDead only applies when fadeAndDeleteWhenDead is false
	stayInPathfindingAfterDead = GetVal(cbp, "stayInPathfindingAfterDead", false);
}
