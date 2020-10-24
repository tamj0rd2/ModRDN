/////////////////////////////////////////////////////////////////////
//	File	:
//	Desc.	:
//		11.Dec.00 (c) Relic Entertainment Inc.
//
#pragma once

enum ControllerType
{
	NULL_EC = 0,

	Henchmen_EC = 3,
	Coal_EC = 4,
	Lab_EC = 5,
	// lightning rod
	ResourceRenew_EC = 6,
	// creature chamber
	RemoteChamber_EC = 7,
	WaterChamber_EC = 8,
	// air chamber
	Aviary_EC = 9,
	BrambleFence_EC = 13,
	// workshop
	Foundry_EC = 17,
	SoundBeamTower_EC = 19,
	// this is actually the geogenerator
	ElectricGenerator_EC = 30,

	MAX_EC
};
