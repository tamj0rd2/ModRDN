/////////////////////////////////////////////////////////////////////
// File    : RDNEBPs.h
// Desc    :
// Created : Thursday, March 08, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// forward declaration
class ControllerBlueprint;

/////////////////////////////////////////////////////////////////////
//

class RDNEBP
{
public:
	struct EBPName
	{
		const char *folder;
		const char *file;
	};

	static const EBPName Lab;
	static const EBPName Henchmen;
	static const EBPName CreatureChamber;
	static const EBPName WaterChamber;
	static const EBPName AirChamber;
	static const EBPName GeoGenerator;
	static const EBPName BrambleFence;
	static const EBPName Workshop;
	static const EBPName SoundBeamTower;
	static const EBPName LightningRod;

	static void Preload();

	static const ControllerBlueprint *Get(const EBPName &);
};
