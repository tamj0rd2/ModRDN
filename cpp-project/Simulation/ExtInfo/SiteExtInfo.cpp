/////////////////////////////////////////////////////////////////////
// File    : SiteExtInfo.cpp
// Desc    :
// Created : Wednesday, March 21, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "SiteExtInfo.h"

#include <SurfVol/obb3.h>

#include <SimEngine/SimEntity.h>
#include <SimEngine/BuildingDynamics.h>

SiteExtInfo::SiteExtInfo(const ControllerBlueprint *cbp)
{
	canPlaceType = LT_NoWhere;

	bool bIsLand = GetVal(cbp, "is_land", false);
	bool bIsWater = GetVal(cbp, "is_water", false);

	if (!bIsLand && !bIsWater)
	{
		// one of these must be set, default to land
		bIsLand = true;
	}

	// set the canPlaceType
	if (bIsLand && bIsWater)
	{
		canPlaceType = LT_LandWater;
		dbAssert(GetVal(cbp, "is_shoreline", false) == false);
	}
	else if (bIsLand)
	{
		canPlaceType = LT_Land;
		dbAssert(GetVal(cbp, "is_shoreline", false) == false);
	}
	else if (bIsWater)
	{
		canPlaceType = LT_Water;
		dbAssert(GetVal(cbp, "is_shoreline", false) == false);
	}

	// heightSnapType
	if (GetVal(cbp, "snapSurface", true))
	{
		heightSnapType = HST_Surface;
		dbAssert(GetVal(cbp, "snapHeightMap", false) == false);
	}
	else if (GetVal(cbp, "snapHeightMap", true))
	{
		heightSnapType = HST_HeightMap;
		dbAssert(GetVal(cbp, "snapSurface", false) == false);
	}

	attachTo = (int)GetVal(cbp, "attachTo", (int)NULL_EC);

	if (attachTo != NULL_EC)
	{
		canPlaceType = LT_Object;
	}

	long rotate = GetVal(cbp, "orientation", 0);
	rotateRads = rotate * (PI / 2.0f);

	// show building decal
	showSiteDecal = GetVal(cbp, "showSiteDecal", true);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: creates an EntityDynamics with the proper snap
//	Result	:
//	Param.	:
//
EntityDynamics *SiteExtInfo::CreateDynamics(Entity *entity, const SiteExtInfo *site)
{
	// validate parm
	dbAssert(entity);
	dbAssert(site != 0);

	//
	EntityDynamics *ed = 0;

	if (site->heightSnapType == SiteExtInfo::HST_Surface)
	{
		ed = new BuildingDynamics(static_cast<SimEntity *>(entity), BuildingDynamics::HST_Surface);
	}
	else if (site->heightSnapType == SiteExtInfo::HST_HeightMap)
	{
		ed = new BuildingDynamics(static_cast<SimEntity *>(entity), BuildingDynamics::HST_HeightMap);
	}
	else
	{ // should never hit this
		dbBreak();
	}

	return ed;
}
