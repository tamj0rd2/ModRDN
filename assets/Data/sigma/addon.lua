---------------------------------------------------------------------
-- File    : addon.lua
-- Desc    : 
-- Created : Wednesday, June 06, 2001
-- Author  : 
-- 
-- (c) 2001 Relic Entertainment Inc.
-- 

addon = 
{
	geyser_1 = 
	{
		type		= ADDON_EGen1,
		name		= 40000,
		icon		= "ui/ingame/addonbatteries1.tga",
		cost		= 125,
		costrenew	= 50,
		health		= 500,
		ticks		= 120,
		structure   = ElectricGenerator_EC,
		dependency  = ADDON_COUNT,
	},
	
	geyser_2 = 
	{
		type		= ADDON_EGen2,
		name		= 40001,
		icon		= "ui/ingame/addonbatteries2.tga",
		cost		= 125,
		costrenew	= 75,
		health		= 500,
		ticks		= 120,
		structure   = ElectricGenerator_EC,
		dependency  = ADDON_EGen1,
	},

	geyser_3 = 
	{
		type		= ADDON_EGen3,
		name		= 40002,
		icon		= "ui/ingame/addonbatteries3.tga",
		cost		= 125,
		costrenew	= 100,
		health		= 500,
		ticks		= 120,
		structure   = ElectricGenerator_EC,
		dependency  = ADDON_EGen2,
	},

}
