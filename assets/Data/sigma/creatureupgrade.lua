---------------------------------------------------------------------
-- File    : creatureupgrade.lua
-- Desc    : 
-- Created : Tuesday, March 19, 2002
-- Author  : 
-- 
-- (c) 2002 Relic Entertainment Inc.
-- 

-- * data for the creature upgrade types

-- Cedric: update localized text!

creatureupgrade = 
{
	defense =
	{
		upgrade		= CREATUREUPGRADE_Defense,
		name		= 40267,
		icon		= "ui/ingame/upgradeDefense.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,
		
		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 50,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 75,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 150,
			},
			-- level 5
			{
				cost		= 300,
				costrenew	= 200,
			},
		},
	},

	speed =
	{
		upgrade		= CREATUREUPGRADE_Speed,
		name		= 40268,
		icon		= "ui/ingame/upgradeSpeed.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 150,
				costrenew	= 25,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 50,
			},
			-- level 4
			{
				cost		= 200,
				costrenew	= 125,
			},
			-- level 5
			{
				cost		= 250,
				costrenew	= 150,
			},
		},
	},

	meleedamage =
	{
		upgrade		= CREATUREUPGRADE_MeleeDamage,
		name		= 40269,
		icon		= "ui/ingame/upgradeMeleeDamage.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 50,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 75,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 150,
			},
			-- level 5
			{
				cost		= 300,
				costrenew	= 200,
			},
		},
	},

	hitpoints =
	{
		upgrade		= CREATUREUPGRADE_HitPoints,
		name		= 40270,
		icon		= "ui/ingame/upgradeHitPoints.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 50,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 75,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 150,
			},
			-- level 5
			{
				cost		= 300,
				costrenew	= 200,
			},
		},
	},

	sightradius =
	{
		upgrade		= CREATUREUPGRADE_SightRadius,
		name		= 40271,
		icon		= "ui/ingame/upgradeSightRadius.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 75,
				costrenew	= 0,
			},
			-- level 3
			{
				cost		= 100,
				costrenew	= 50,
			},
			-- level 4
			{
				cost		= 100,
				costrenew	= 50,
			},
			-- level 5
			{
				cost		= 150,
				costrenew	= 50,
			},
		},
	},

	rangeddamage =
	{
		upgrade		= CREATUREUPGRADE_RangedDamage,
		name		= 40272,
		icon		= "ui/ingame/upgradeRangedDamage.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 50,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 50,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 75,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 150,
			},
			-- level 5
			{
				cost		= 300,
				costrenew	= 200,
			},
		},
	},

	splashdamage =
	{
		upgrade		= CREATUREUPGRADE_SplashDamage,
		name		= 40273,
		icon		= "ui/ingame/upgradeSplashDamage.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 0,
				costrenew	= 0,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 50,
			},
			-- level 3
			{
				cost		= 150,
				costrenew	= 75,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 150,
			},
			-- level 5
			{
				cost		= 300,
				costrenew	= 200,
			},
		},
	},

	areaattackradius =
	{
		upgrade		= CREATUREUPGRADE_AreaAttackRadius,
		name		= 40274,
		icon		= "ui/ingame/upgradeAreaAttackRadius.tga",
		upgradeticks	= 160,
		structure	= GeneticAmplifier_EC,

		level = 
		{
			-- level 1
			{
				cost		= 100,
				costrenew	= 25,
			},
			-- level 2
			{
				cost		= 125,
				costrenew	= 25,
			},
			-- level 3
			{
				cost		= 250,
				costrenew	= 200,
			},
			-- level 4
			{
				cost		= 250,
				costrenew	= 200,
			},
			-- level 5
			{
				cost		= 250,
				costrenew	= 200,
			},
		},
	},
}
