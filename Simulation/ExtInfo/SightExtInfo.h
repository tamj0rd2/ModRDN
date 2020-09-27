/////////////////////////////////////////////////////////////////////
// File    : SightExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h" 
//#include "RDNTuning.h"


///////////////////////////////////////////////////////////////////// 
// SightExtInfo

class SightExtInfo : public ModStaticInfo::ExtInfo
{
// types
public:
	enum { ExtensionID = ModStaticInfo::EXTINFOID_Sight };

// fields
public:
	float	sightRadius;

// construction
public:
	SightExtInfo( const ControllerBlueprint* cbp )
	{
		sightRadius = GetVal( cbp, "sight_radius",  10.0f );
	}
};
