/////////////////////////////////////////////////////////////////////
// File    : CostExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h" 

///////////////////////////////////////////////////////////////////// 
// CostExtInfo

class CostExtInfo : public ModStaticInfo::ExtInfo
{
// types
public:
	enum { ExtensionID = ModStaticInfo::EXTINFOID_Cost };

// fields
public:
	// cost in gatherable resources
	float	costCash;

	// time it takes to construct this unit
	long	constructionTicks;

// construction
public:
	CostExtInfo( const ControllerBlueprint* cbp )
	{
		costCash			= GetVal( cbp, "cost",      0.0f, 100000.0f );

		constructionTicks	= GetVal( cbp, "constructionticks", 1, 1000 );
	}
};
