/////////////////////////////////////////////////////////////////////
// File    : AttackExtInfo.h
// Desc    :
// Created :
// Author  :
//
// (c) 2003 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h"

#include "../AttackTypes.h"
#include "../AttackPackage.h"

/////////////////////////////////////////////////////////////////////
// AttackExtInfo

class AttackExtInfo : public ModStaticInfo::ExtInfo
{
	// types
public:
	enum
	{
		ExtensionID = ModStaticInfo::EXTINFOID_Attack
	};

	// fields
public:
	AttackInfoPackage attackInfo;

	// construction
public:
	AttackExtInfo(const ControllerBlueprint *cbp);

	// implementation
private:
};
