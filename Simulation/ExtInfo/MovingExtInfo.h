/////////////////////////////////////////////////////////////////////
// File    : MovingExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h" 

#include <SimEngine/Pathfinding/PathTypes.h>

class EntityDynamics;
class SimEntity;

///////////////////////////////////////////////////////////////////// 
// MovingExtInfo

class MovingExtInfo : public ModStaticInfo::ExtInfo
{
// types
public:
	enum { ExtensionID = ModStaticInfo::EXTINFOID_Moving };

	enum MovingType
	{
		MOV_FLYER,
		MOV_GROUND,
		MOV_SWIMMER,
		MOV_AMPHIBIOUS,
	};

// statics
public:
	static MovingType GetMovingType( const ControllerBlueprint* cbp );

// functions
public:
	inline bool		IsFlyer() const;
	inline bool		IsGround() const;
	inline bool		IsSwimmer() const;

// fields
public:
	MovingType	movingType;
	
	bool		bCannotBeSlowed;

	float		speed;
	float		airSpeed;
	float		waterSpeed;

	float		entityMoveAP;

// methods
public:
	TCMask GetMovementMask() const;

// construction
public:
	MovingExtInfo( const ControllerBlueprint* cbp );

};

//
inline bool MovingExtInfo::IsFlyer() const
{
	return movingType == MOV_FLYER;
}

//
inline bool MovingExtInfo::IsGround() const
{
	return movingType == MOV_GROUND || movingType == MOV_AMPHIBIOUS;
}

//
inline bool MovingExtInfo::IsSwimmer() const
{
	return movingType == MOV_SWIMMER || movingType == MOV_AMPHIBIOUS;
}

///////////////////////////////////////////////////////////////////// 
// 

EntityDynamics* CreateDynamics( SimEntity* pEntity, const MovingExtInfo* moving );
