---------------------------------------------------------------------
-- File    : Tuning.lua
-- Desc    : 
-- Created : Wednesday, June 20, 2001
-- Author  : 
-- 
-- (c) 2001 Relic Entertainment Inc.
-- 

-- * Global Tuning Values


---------------------------------------------------------------------
-- Player Info

Player =
{
	-- starting resources for a player
	startingcash = 500.0,
}

---------------------------------------------------------------------
-- Race

Race =
{
	stronger_healthMultiplier = 2.0,
	stronger_costMultiplier   = 1.0,
	stronger_speedMultiplier  = 1.0,

	cheaper_healthMultiplier  = 1.0,
	cheaper_costMultiplier    = 0.5,
	cheaper_speedMultiplier   = 1.0,

	faster_healthMultiplier   = 1.0,
	faster_costMultiplier     = 1.0,
	faster_speedMultiplier    = 2.0,
}

---------------------------------------------------------------------
-- Effect

Effect =
{
	-- Location types :
	--  0 = root
	--  1 = random marker attachment
	--  2 = sparse marker attachement
	--  3 = unattached
	--  4 = over the object

	impact_fx       = "COMBAT_IMPACT_COMBO",
	impact_location = "random",
	impact_count    = 1,
}

---------------------------------------------------------------------
-- FogOfWar

FogOfWar =
{
	--	 the number of seconds an attacker is reveal in the victims FoW
	attackerRevealTime = 4.0,
}
