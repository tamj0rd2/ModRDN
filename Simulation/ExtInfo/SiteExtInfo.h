/////////////////////////////////////////////////////////////////////
// File    : SiteExtInfo.h
// Desc    :
// Created : Monday, March 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h"

#include "../Controllers/ControllerTypes.h"

/////////////////////////////////////////////////////////////////////
//	Forward Declarations
//
class Entity;
class EntityDynamics;

/////////////////////////////////////////////////////////////////////
// SiteExtInfo

class SiteExtInfo : public ModStaticInfo::ExtInfo
{
	// types
public:
	enum
	{
		ExtensionID = ModStaticInfo::EXTINFOID_Site
	};

	enum LocationType
	{
		LT_NoWhere,	 // can be placed no where
		LT_Land,		 // can be placed only on land
		LT_Water,		 // can be placed only on water
		LT_Object,	 // can be placed only on an object
		LT_LandWater // can be placed on land or water
	};

	enum HeightSnapType
	{
		HST_Surface,
		HST_HeightMap
	};

	// statics
public:
	static EntityDynamics *CreateDynamics(Entity *entity, const SiteExtInfo *site);

	// fields
public:
	LocationType canPlaceType;

	HeightSnapType heightSnapType;

	// this is only valid if the LocationType is LT_Object
	int attachTo;

	// how much should this object be rotated by when it's placed on the ground
	float rotateRads;

	// show site decal
	bool showSiteDecal;

	// construction
public:
	SiteExtInfo(const ControllerBlueprint *cbp);
};
