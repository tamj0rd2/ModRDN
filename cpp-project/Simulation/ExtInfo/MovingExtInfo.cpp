/////////////////////////////////////////////////////////////////////
// File    : MovingExtInfo.cpp
// Desc    :
// Created : Monday, March 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "MovingExtInfo.h"

#include "ModObj.h"

#include "Simulation/RDNWorld.h"

#include <SimEngine/GroundDynamics.h>
#include <SimEngine/FlyingDynamics.h>

/////////////////////////////////////////////////////////////////////
// MovingExtInfo

MovingExtInfo::MovingExtInfo(const ControllerBlueprint *cbp)
{
	const float factor =
			(1.0f / 3.6f) / ModObj::i()->GetWorld()->GetNumSimsPerSecond();

	const float land = GetVal(cbp, "speed_max", 0.0f, 300.0f);
	const float air = GetVal(cbp, "airspeed_max", 0.0f, 300.0f);
	const float water = GetVal(cbp, "waterspeed_max", 0.0f, 300.0f);

	bCannotBeSlowed = GetVal(cbp, "end_bonus", false);

	speed = factor * land;
	airSpeed = factor * air;
	waterSpeed = factor * water;

	movingType = GetMovingType(cbp);

	entityMoveAP = 0.0f;
	if (cbp->GameAttributeCheck("entity_move_ap"))
	{
		entityMoveAP = cbp->GameAttributeRetrieve("entity_move_ap");
	}

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
static bool GetBlueprintVal(const ControllerBlueprint *cbp, const char *attr, bool def)
{
	// check if attribute exists
	if (!cbp->GameAttributeCheck(attr))
	{
		dbWarningf('SMOD', "ebp %S missing attribute %s",
							 cbp->GetScreenName(),
							 attr);

		return def;
	}

	// retrieve value
	return cbp->GameAttributeRetrieve(attr) != 0.0f;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
MovingExtInfo::MovingType MovingExtInfo::GetMovingType(const ControllerBlueprint *cbp)
{
	bool isFlyer = GetBlueprintVal(cbp, "is_flyer", false);
	bool isGround = GetBlueprintVal(cbp, "is_land", true);
	bool isSwimmer = GetBlueprintVal(cbp, "is_swimmer", false);

	MovingType movingType;

	//
	if (isFlyer)
	{
		movingType = MOV_FLYER;
	}
	else if (isSwimmer && !isGround)
	{
		movingType = MOV_SWIMMER;
	}
	else if (!isSwimmer && isGround)
	{
		movingType = MOV_GROUND;
	}
	else
	{
		movingType = MOV_AMPHIBIOUS;
	}

	return movingType;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: returns the pathfinding movement mask type, indicating what type of terrain
//				the Entity can move on
//	Result	: return TCMask
//
TCMask MovingExtInfo::GetMovementMask() const
{
	TCMask mask = 0;
	switch (movingType)
	{
	case MOV_FLYER:
		mask = 0;
		break;
	case MOV_GROUND:
		mask = eLandImpassible;
		break;
	case MOV_SWIMMER:
		mask = eWaterImpassible;
		break;
	case MOV_AMPHIBIOUS:
		mask = eAmphibianImpassible;
		break;
	default:
		dbBreak();
	}
	return mask;
}

/////////////////////////////////////////////////////////////////////
//

EntityDynamics *CreateDynamics(SimEntity *entity, const MovingExtInfo *moving)
{
	// validate parm
	dbAssert(entity);
	dbAssert(moving != 0);

	//
	EntityDynamics *ed = 0;

	if (moving->IsFlyer())
	{
		// create flying dynamics.
		ed = new FlyingDynamics(entity);
	}
	else
	{
		// create ground dynamics.
		GroundDynamics *gd = new GroundDynamics(entity);

		gd->SetMovementMask(moving->GetMovementMask());

		ed = gd;
	}

	return ed;
}
