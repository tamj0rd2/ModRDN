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
-- Poison Tuning values

Poison = 
{
	-- Total Time the Poison Lasts for
	poisontime				= 10.0,
	
	-- Total amount of damage the Poison should deal, per attacker rank
	poisondamage1			= 20.0,
	poisondamage2			= 20.0,
	poisondamage3			= 20.0,
	poisondamage4			= 23.0,
	poisondamage5			= 26.0,
	
	-- The amount to multiply the creature's speed by when poisoned
	speedmultiplier			= 0.75,
	
	-- The amount to multiply the creature's attack damage by when poisoned
	damagemultiplier		= 0.75,
	
	-- The poisoned victom has a reduced sight radius to this value
	poisonsightradius		= 0.0,
}

---------------------------------------------------------------------
-- Venom Spray Tuning values

VenomSpray =
{
	-- Total Time the Venom Spray Lasts for
	venomtime				= 5.0,
	-- Total amount of damage the Venom Spray should deal
	venomdamage				= 25.0,
	-- Amount to set the victim's sight radius to while it is blinded
	venomsightradius			= 1.0,
}

---------------------------------------------------------------------
-- Endurance Tuning values

Endurance = 
{
	-- the base amount of endurance for all animals/creatures
	endurancebase		= 100.0,
	
	-- the ammount of regeneration that occurs over 1 second
	regen_normal		= 1.0,

	regen_bonus			= 2.5,

	regen_penalty		= 3.0,
}

---------------------------------------------------------------------
-- Defense Tuning values

Defense = 
{
	-- the max your defence can be set to through modifications
	defensemax		= 0.75,
}


---------------------------------------------------------------------
-- Loner Bonus

Loner =
{
	-- The radius at which the loner bonus becomes effective (in metres)
	lonerRadius			= 35.0,

	-- Include henchmen in radius check?
	-- yes = 1 or no = 0 (default) 
	checkForHenchmen		= 0,

	-- Artillery ranged bonus per creature size
	-- This *is a multiplier*, e.g. a 50% bonus should be written as 1.5.
	artilleryBonusBaseSize1		= 4.0,
	artilleryBonusBaseSize2		= 4.0,
	artilleryBonusBaseSize3		= 4.0,
	artilleryBonusBaseSize4		= 4.0,
	artilleryBonusBaseSize5		= 4.0,
	artilleryBonusBaseSize6		= 4.0,
	artilleryBonusBaseSize7		= 4.0,
	artilleryBonusBaseSize8		= 4.0,
	artilleryBonusBaseSize9		= 4.0,
	artilleryBonusBaseSize10	= 4.0,

	-- Artillery bonus modifier per creature rank (+/-)
	-- This is a modifier to the above base percentage.
	artilleryBonusModRank1		= 0.0,
	artilleryBonusModRank2		= 0.0,
	artilleryBonusModRank3		= 0.0,
	artilleryBonusModRank4		= 0.0,
	artilleryBonusModRank5		= 0.0,

	-- Direct ranged bonus per creature size
	-- This *is a multiplier*, e.g. a 50% bonus should be written as 1.5.
	rangedBonusBaseSize1		= 5.0,
	rangedBonusBaseSize2		= 5.0,
	rangedBonusBaseSize3		= 5.0,
	rangedBonusBaseSize4		= 5.0,
	rangedBonusBaseSize5		= 5.0,
	rangedBonusBaseSize6		= 5.0,
	rangedBonusBaseSize7		= 5.0,
	rangedBonusBaseSize8		= 5.0,
	rangedBonusBaseSize9		= 5.0,
	rangedBonusBaseSize10		= 5.0,

	-- Ranged bonus modifier per creature rank (+/-)
	-- This is a modifier to the above base percentage.
	rangedBonusModRank1		= 0.0,
	rangedBonusModRank2		= 0.0,
	rangedBonusModRank3		= 0.0,
	rangedBonusModRank4		= 0.0,
	rangedBonusModRank5		= 0.0,

	-- Melee bonus per creature size
	-- This *is a multiplier*, e.g. a 50% bonus should be written as 1.5.
	meleeBonusBaseSize1		= 6.0,
	meleeBonusBaseSize2		= 6.0,
	meleeBonusBaseSize3		= 6.0,
	meleeBonusBaseSize4		= 6.0,
	meleeBonusBaseSize5		= 6.0,
	meleeBonusBaseSize6		= 6.0,
	meleeBonusBaseSize7		= 6.0,
	meleeBonusBaseSize8		= 6.0,
	meleeBonusBaseSize9		= 6.0,
	meleeBonusBaseSize10		= 6.0,

	-- Melee bonus modifier per creature rank (+/-)
	-- This is a modifier to the above base percentage.
	meleeBonusModRank1		= 0.0,
	meleeBonusModRank2		= 0.0,
	meleeBonusModRank3		= 0.0,
	meleeBonusModRank4		= 0.0,
	meleeBonusModRank5		= 0.0,

	-- Damage Reduction
	-- The is the percentage of damage reduction a loner creature has
	-- valid range: 0.0 - 1.0  = 0% reduction to 100% reduction
	damageReductionBaseSize1	= 0.75,
	damageReductionBaseSize2	= 0.75,
	damageReductionBaseSize3	= 0.75,
	damageReductionBaseSize4	= 0.75,
	damageReductionBaseSize5	= 0.75,
	damageReductionBaseSize6	= 0.75,
	damageReductionBaseSize7	= 0.75,
	damageReductionBaseSize8	= 0.75,
	damageReductionBaseSize9	= 0.75,
	damageReductionBaseSize10	= 0.75,
	
	-- Damage reduction modifier per creature rank (+/-)
	-- This is a modifier to the above base percentage.
	damageReductionModRank1		= 0.0,
	damageReductionModRank2		= 0.0,
	damageReductionModRank3		= 0.0,
	damageReductionModRank4		= 0.0,
	damageReductionModRank5		= 0.0,

	-- Build speed modifier per creature rank (multiplies to base tick)
	buildSpeedModRank1			= 1.0,
	buildSpeedModRank2			= 1.5,
	buildSpeedModRank3			= 2.0,
	buildSpeedModRank4			= 2.5,
	buildSpeedModRank5			= 3.0,
	
	-- Speed increase 
	--This *is a multiplier*, e.g. a 50% bonus should be written as 1.5.
	speedMultiplier		= 1.0,

	-- Sight radius increase 
	-- This *is a multiplier*, e.g. a 50% bonus should be written as 1.5.
	sightRadiusMultiplier		= 1.5,
}


---------------------------------------------------------------------
-- Deflection Tuning values

Deflection = 
{
	-- Next 3 sub-sections are normalised chances of deflection.
	-- Valid range is 0.0 (0% - never deflect) to 1.0 (100% - always deflect)

	-- Base deflection chance per creature size
	deflectionBaseSize1	= 0.35,
	deflectionBaseSize2	= 0.35,
	deflectionBaseSize3	= 0.35,
	deflectionBaseSize4	= 0.35,
	deflectionBaseSize5	= 0.35,
	deflectionBaseSize6	= 0.35,
	deflectionBaseSize7	= 0.35,
	deflectionBaseSize8	= 0.35,
	deflectionBaseSize9	= 0.35,
	deflectionBaseSize10	= 0.35,

	-- Deflection modifier per creature rank (+/-)
	deflectionModRank1	= 0.0,
	deflectionModRank2	= 0.0,
	deflectionModRank3	= 0.0,
	deflectionModRank4	= 0.05,
	deflectionModRank5	= 0.1,
	
	-- Deflection modifier per defenders attack type (+/-)
	-- Order of preference (artillery, direct ranged, melee)
	deflectionModArtillery	= 0.0,
	deflectionModRanged	= -0.1,
	deflectionModMelee	= 0.0,

	-- Deflect sonic attacks?
	-- yes = 1 (default) or no = 0 
	deflectSonicAttack	= 1,
}


---------------------------------------------------------------------
-- Flash Tuning values

Flash = 
{
	-- Endurance cost
	enduranceCost		= 100.0,
	
	-- Duration in game ticks
	duration		= 80.0, -- about 10 seconds

	-- Flash Radius in metres (based on flashing creatures' size)
	effectiveRadiusSize1	= 40.0,
	effectiveRadiusSize2	= 40.0,
	effectiveRadiusSize3	= 40.0,
	effectiveRadiusSize4	= 40.0,
	effectiveRadiusSize5	= 40.0,
	effectiveRadiusSize6	= 40.0,
	effectiveRadiusSize7	= 40.0,
	effectiveRadiusSize8	= 40.0,
	effectiveRadiusSize9	= 40.0,
	effectiveRadiusSize10	= 40.0,

	-- Reduced size radius in metres
	reducedSightRadius 	= 12.0,

	-- Height above the ground that initial effect will happen (in metres)
	fxTargetHeight		= 8.0,
}


---------------------------------------------------------------------
-- Health Regeneration Tuning values

HealthRegen =
{
	-- the amount of health that regenerates per second, by creature rank
	regenamount1		= 1.25,
	regenamount2		= 1.25,
	regenamount3		= 1.5,
	regenamount4		= 1.75,
	regenamount5		= 2.0,
}

---------------------------------------------------------------------
-- Stealth Modifier Tuning values

Stealth =
{
	-- the ammount of time in seconds that the creature becomes unCloaked for
	-- when it doesn an attack
	stealthtimeout		= 10.0,
}

---------------------------------------------------------------------
-- Pack Bonus

PackBonus =
{
	-- the minimum number of entities needed for the bonus to kick in
	minentitycount			= 4.0,

	-- the base damage modifier, new damage is this * normal damage
	basedamagemodifier		= 1.40,

	-- the base defense modifier, new defense is this * normal damage
	basedefensemodifier		= 1.50,

	-- the search radius that the pack bonus is effective for, meters
	searchradius			= 15.0,

	-- the amount of sim ticks that need to pass for packing to be reset
	delayticks				= 8,
}

---------------------------------------------------------------------
-- Building

Building =
{
	-- value for decreasing additional henchman to build a structure [0.0,1.0]
	buildDecrease			= 0.01,

	-- value for decreasing additional henchman to repair a structure [0.0,1.0]
	repairDecrease			= 0.08,

	-- amount of health a single henchman repairs [1.0, 1000.0]
	repairHealth			= 1.0,

	-- repair multiplier for labs
	repairLabMult			= 2.5,
	
	-- how far away does a structure have to be away from a resource before it can be built, in meters
	-- it will make buildings not placeable within a square around the resource of this size
	resourceNoBuildSize		= 22.0,

	-- what percentage of health should a building start with [0.0 to 1.0]
	startHealthPercent		= 0.2,

	-- what percentage of scrap do you get back when you self destroy a building
	scrapBackPercentage		= 0.75,

	-- what percentage of electricity you get back when you self destroy a building
	elecBackPercentage		= 0.75,
}

---------------------------------------------------------------------
-- ResearchBonus

ResearchBonus =
{
	-- hitpoint bonus to give to the lab with the advanced structure research
	advancedStructLabMultiplier		= 1.5,

	-- hitpoint bonus to give to buildings (no lab) with the increase building
	-- integrity research
	incBuildingIntegrityMultiplier		= 1.5,

	-- hitpoint bonus to give to bramble fences with the strengthen-fences 
	-- research
	strengthenFenceMultiplier		= 4.0,

	-- collection rate bonus to give to all electricity collectors with the 
	-- strengthen-electrical-grid research
	strengthenElecGridMultiplier		= 1.25,

	-- henchman improved healing
	henchmanImpHealingMultiplier		= 2.0,
}

---------------------------------------------------------------------
-- SiteDecal

SiteDecal =
{
	-- default decal names
	defaultLiveDecalName		= "build_con.tga",
	defaultDeadDecalName		= "build_des.tga",

	-- coal decal info
	coalDecalName			= "coal_01.tga",
	coalDecalScale			= 1.75,

	-- soiled land decal info
	soiledLandDecalName			= "Data:Sigma/Decals/soiled_land_decal.tga",
	soiledLandDecalScale		= 2.0,
	
	-- soiled land in water decal info
	soiledLandInWaterDecalName		= "Data:Sigma/Decals/soiled_water_decal.tga",
	soiledLandInWaterDecalScale		= 2.0,
	
	-- fade-in and fade-out times for live and dead building decals
	liveDecalFadeInTime			= 0.0,
	liveDecalFadeOutTime		= 1.0,
	deadDecalFadeInTime			= 1.0,
	deadDecalLifeTime			= 5.0,
	deadDecalFadeOutTime		= 1.0,
}

---------------------------------------------------------------------
-- Stink Cloud

StinkCloud =
{
	-- the minimum number of entities needed for the bonus to kick in
	enduranceCost			= 75.0,

	-- how long the cloud lasts, in ticks, per creature rank
	duration1				= 80.0, 
	duration2				= 96.0, 
	duration3				= 112.0, -- about 10 seconds
	duration4				= 128.0, 
	duration5				= 144.0, 

	-- damage radius = radiusOffset + creature size (in meters) * radiusScale
	radiusOffset			= 11.0,
	radiusScale				= 0.0,

	-- 
	postDuration			= 2.0,  -- 0.2 seconds

	--
	protect				= 1.0,

	--
	descentSpeed			= 8.0,	-- descend 8m per second	

	--
	reducedVictimSpeedTo	= 0.5,  -- 50% of full speed
}

---------------------------------------------------------------------
-- ElectricBurst

ElectricBurst =
{
	-- the minimum number of entities needed for the bonus to kick in
	enduranceCost			= 75.0,

	-- this is the duration of the attack in ticks, and modifies the base damage
	-- new damage is this * dmgPerTick (see below)
	duration				= 6.0,

	-- damage radius : C = (creature size (in meters)) O = offset, S = scale
	-- Final radius = O + C*S  (where O is the minimum radius)
	radiusOffset			= 9.0,
	radiusScale				= 0.0,

	-- damage per tick
	-- new damage is this * duration (see above)
	dmgPerTick1				= 20.0,
	dmgPerTick2				= 20.0,
	dmgPerTick3				= 20.0,
	dmgPerTick4				= 28.0,
	dmgPerTick5				= 39.0,

	-- not used!
	dmgToBuilding			= .10
}

---------------------------------------------------------------------
-- QuillBurst

QuillBurst =
{
	-- the minimum number of entities needed for the bonus to kick in
	enduranceCost			= 60.0,

	-- this is the duration of the attack in ticks, and modifies the base damage
	-- new damage is this * dmgPerTick (see below)
	duration				= 4.0,

	-- damage radius : C = (creature size (in meters)) O = offset, S = scale
	-- Final radius = O + C*S  (where O is the minimum radius)
	radiusOffset			= 9.0,
	radiusScale				= 0.0,

	-- damage per tick
	-- new damage is this * duration (see above)
	dmgPerTick1				= 12.0,
	dmgPerTick2				= 12.0,
	dmgPerTick3				= 16.0,
	dmgPerTick4				= 24.0,
	dmgPerTick5				= 34.0,

	-- not used
	dmgToBuilding			= 0
}

---------------------------------------------------------------------
-- WebThrow

WebThrow =
{
	-- the minimum number of entities needed for the bonus to kick in
	enduranceCost			= 100.0,

	-- this is the duration of the attack in ticks, and modifies the base damage
	-- new damage is this * normal damage
	duration				= 88.0,

	-- attack range : C = (creature size (in meters)) O = offset, S = scale
	-- Final radius = O + C*S  (where O is the minimum radius)
	rangeOffset				= 12.0,
	rangeScale				= 0.0,

	-- damage radius : C = (creature size (in meters)) O = offset, S = scale
	-- Final radius = O + C*S  (where O is the minimum radius)
	radiusOffset			= 9.0,
	radiusScale				= 0.0,
	
	-- height above the ground that the initial web effect will happen
	effectTargetHeight		= 8.0,
}


---------------------------------------------------------------------
-- Assassinate

Assassinate =
{
	-- Endurance cost
	enduranceCost		= 100.0,

	-- Attack damage done by the assassination attempt.
	-- This is a percentage of the victims maximum hitpoints and it ignores
	-- all defenses and damage reduction (valid values 0.0 to 1.0).
	damageTotalBaseSize1	= 0.75,
	damageTotalBaseSize2	= 0.75,
	damageTotalBaseSize3	= 0.75,
	damageTotalBaseSize4	= 0.65,
	damageTotalBaseSize5	= 0.65,
	damageTotalBaseSize6	= 0.65,
	damageTotalBaseSize7	= 0.50,
	damageTotalBaseSize8	= 0.50,
	damageTotalBaseSize9	= 0.50,
	damageTotalBaseSize10	= 0.50,
}

---------------------------------------------------------------------
-- Infestation

Infestation =
{
	-- Endurance cost to use this ability
	enduranceCost		= 50.0,

	-- Duration of the attack in ticks, and modifies the base damage
	-- new damage is this * normal damage
	duration				= 500.0,
	
	-- Damage per tick to the bulding
	dmgPerTick				= 4.25,
	
	-- Is infestation fully effective on building with struture defence ON?
	-- 1 = YES, infest with full damage ; 0 = NO, cannot infest
	canInfestOnStructureDefence	= 1;
	
}

---------------------------------------------------------------------
-- Night

Night =
{
	-- the scalar that will be applied to all animals sight radius at night
	sightModifier			= 0.5
}

---------------------------------------------------------------------
-- Gyrocopter

Gyrocopter =
{
	-- the number of seconds it take to lift a crate (animation tweak)
	airliftTime			= 2.5,

	-- the number of seconds it takes the passenger to fade out
	liftFadeTime		= 2.0,

	-- the number of seconds it take to drop a crate (animation tweak)
	dropTime			= 5.0, --was 3

	-- the number of seconds it take to during the drop animation before the creature appears
	dropFadeTime		= 4.0, --was 2

	-- the number of seconds it take before validating pending pick up passengers
	pendingAirliftTime	= 2.0,
	
	-- the radius around gyrocopter that will pick up incoming pending passengers
	pendingAirliftRadius	=	15.0,
	
	-- the number of health point the gyro is repaired per second when landed on the pad
	landedRepairPerSecond	=	3.0,

	-- the number of seconds it takes the gyrocopter to land once it is over the pad
	landingSeconds			=	3.0
}


---------------------------------------------------------------------
-- Digging

Digging =
{
	-- 
	speedMultiplier			= 1.0,

	-- the scalar that will be applied to all sight radius when underground
	sightMultiplier			= 0.1,

	-- the defense bonus that will be applied when the creatures is underground 0.0 to 1.0
	defenseBonus			= 0.5,

	-- the amount of time to get underground
	digDownTime			= .1,

	-- the amount of time to get out of underground
	digUpTime			= .1,
	
	-- the image file to use for the digging
	dugDecal			= "Data:Sigma/Decals/dug_in_ring.tga",
	
	-- a multiplier for endurance regeneration when underground
	-- e.g. 0.5 means half, 2.0 means double
	enduranceRegenMultiplier 	= 0.75,
	
	-- a multiplier for health regeneration when underground
	-- e.g. 0.5 means half, 2.0 means double
	healthRegenMultiplier		= 0.75,
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

	impact_fx = "COMBAT_IMPACT_COMBO",
	impact_location = "random",
	impact_count = 1,

	poison_fx =  "POISON_SPRAY_halo",
	poison_location = "random",
	poison_count = 2,

	electric_fx = "ELECTRIC_DMG_SPRAY",
	electric_location = "centre",
	electric_count = 1,

	sonic_fx = "SONIC_ATTACK_DMG_spray",
	sonic_location = "random",
	sonic_count = 1,

	stink_fx = "damage_stink",
	stink_location = "centre",
	stink_count = 1,

	plague_fx = "disease_SPRAY_halo",
	plague_location = "random",
	plague_count = 2,

	pack_fx = "pack_ring2",
	pack_location = "centre",
	pack_count = 1,

	herd_fx = "herd_ring2",
	herd_location = "centre",
	herd_count = 1,

	digging_fx = "digging_dirtmound_spray",
	digging_location = "centre",
	digging_count = 1,

	dig_stationary_fx = "null",
	dig_stationary_location = "centre",
	dig_stationary_count = 1,

	dig_moving_fx = "dig_combo",
	dig_moving_location = "centre",
	dig_moving_count = 1,

	frenzy_fx = "FRENZY_ring",
	frenzy_location = "centre",
	frenzy_count = 1,

	sonarpulse_fx = "sonar_pulse_combo",
	sonarpulse_location = "",
	sonarpulse_count = 1,

	barrier_fx = "null",
	barrier_location = "random",
	barrier_count = 1,

	gyrodmg_fx = "COMBAT_IMPACT_COMBO",
	gyrodmg_location = "random",
	gyrodmg_count = 1,

	henchmandmg_fx = "henchmen_combat_impact_combo",
	henchmandmg_location = "random",
	henchmandmg_count = 1,

	loner_fx = "lonercombo02",
	loner_location = "centre",
	loner_count = 1,

	flash_fx =  "flash_combo_head",
	flash_location = "head",
	flash_count = 2,
}


---------------------------------------------------------------------
-- AttackBonus

AttackBonus =
{
	-- multiplier of damage applied to a building when striked with barrier destroy damage
	barrierBuildingBonusMult		= 1.5,

	-- multiplier of damage applied to a bramble fence when striked with barrier destroy damage
	barrierFenceBonusMult			= 2.0,

	-- multiplier of damage applied to a building when striked with electric type damage
	electricBuildingBonusMult		= 1.0,

	-- multiplier of damage applied to a building when striked with horn negate armour type damage
	negateArmourBuildingBonusMult	= 1.0,

	-- multiplier of damage applied to a creature when striked with horn negate armour type damage
	negateArmourCreatureBonusMult	= 0.5,

	-- minimum elevation difference (in meters) between the attacker and the attackee for the terrain-height bonus to kick in 
	terrainHeightBonusMinElevDiff		= 4.0,

	-- multiplier of damage applied to an entity when the attacker attacks from higher elevation
	terrainHeightBonusMult			= 1.2,

	-- multiplier of damage applied to a building when striked with direct range attack
	directRangeBuildingMult			= 0.5,

	-- multiplier of damage applied to a bramble fence when striked with direct range attack
	directRangeFenceMult			= 1.0,

	-- multiplier of damage applied to a building when striked with artillery attack
	artilleryBuildingMult			= 1.0,

	-- multiplier of damage applied to a flyer when it is attacked by a non-flyer unit using a direct ranged attack
	nonFlyerToFlyerDirectRangeDamageMult	= 1.4,

	-- fraction of a flyer's defense to remove when it is attacked by a non-flyer unit using a direct ranged attack
	nonFlyerToFlyerDirectRangeDefenseMult	= 0.0,

	-- multiplier of damage applied to a flyer when it is attacked by an artillery attack
	artilleryFlyerDamageMult		= 1.0,

	-- fraction of a flyer's defense to remove when it is attacked by an artillery attack
	artilleryFlyerDefenseMult		= 0.0,
}

---------------------------------------------------------------------
-- SonicAttack

SonicAttack =
{
	-- angle of the cone - should match up with effect
	cone_angle		= 34,
	
	-- duration of effect in seconds
	duration		= 32,
	
	-- reduction per hit
	speedReduction  = 0.02,

	-- how much percent of your speed can be reduced to
	speedMinPercentage = 0.5,
	
	-- reduction per hit for loner unit
	speedReductionForLoner  = 0.10,

	-- how much percent of loner's speed can be reduced to
	speedMinPercentageForLoner = 0.3,
	
	-- damage multiplier for loner unit (1.0 is par value)
	dmgMultiplierForLoner = 2.0,
}

---------------------------------------------------------------------
-- DoctorHeal

DoctorHeal =
{
	-- number of health points per second a henchman will heal
	healthPerSecond		= 2,

	-- maximum distance between the doctor and the patient
	healRange		= 10.0,
}

---------------------------------------------------------------------
-- GuardInfo

GuardInfo =
{
	-- The Inner Guard Radius is the area around the guard target the creature will try and stay within
	creatureInnerGuardRadius		= 8.0,
	-- The Outer Guard Radius is the area around the guard target the creature will chase down enemies to
	creatureOuterGuardRadius		= 30.0,

	-- size of the henchman guard radius, measured in sight radii.  1.0 means everything the henchman sees is in the guard radius
	henchmanGuardRadius		= 1.0,
	-- the fraction of the guard radius that the Entity will randomly wander in
	randomWanderFraction	= 0.5,
}

---------------------------------------------------------------------
-- ChargeInfo

ChargeInfo =
{
	-- Charge distance, the creature must be at least min meters away to turn charge on
	-- but can be up to max meters away to enable charge
	distchargemin		= 7.0,
	distchargemax		= 25.0,
	-- multiply to melee damage to get charge damage.
	damagemultiplier	= 2.0,
	-- Charge speed is a modifier to speed whilst charging..
	speedmultiplier		= 2.0,

	-- set to 1.0 if the charger requires a clear straight line to the target before charging
	requiresStraightLine = 0.0,

	-- set to 1.0 if the charge will abort when the charger changes direction ie. if has to go around an object
	directionChangeAborts = 0.0,

	-- the creature will charge (at most) every this number of seconds
	chargeRateSeconds	  = 10.0,
}

---------------------------------------------------------------------
-- LeapInfo

LeapInfo =
{
	-- Leap distance, the creature must be at least min meters away to turn charge on
	-- but can be up to max meters away to enable charge
	distleapmin			= 7.0,
	distleapmax			= 15.0,
	-- multiply to melee damage to get charge damage.
	damagemultiplier	= 1.0,
	-- Leap speed is a modifier to speed whilst charging..
	speedmultiplier		= 1.5,

	-- the creature will leap (at most) every this number of seconds
	leapRateSeconds		= 5.0,

	-- how long it takes a leap to reach its target once it started leaping ( in seconds )
	attackSeconds		= 0.5,

	-- how long it take a leaper to cool after a leap attack ( in seconds )
	coolSeconds			= 1.0,

	-- how many seconds of damage a leap attack does.  
	--	Used to determine the damage of a leap attack by multiplying the attackers damagePerSecond
	secondsOfDamage		= 2.0
}

---------------------------------------------------------------------
-- Henchman

Henchman =
{
	-- sight radius scalar for henchmen with binoculars
	binocularsSightModifier	= 1.5,
	
	-- distance to search for a gathersite after current one is depleted
	gatherSearchRadius = 35.0,

	-- 
	yokeBonus = 1.25,

	-- land speed bonus (in km/h) for motivated henchmen
	motivatedLandSpeedBonus = 10.0,

	-- water speed bonus (in km/h) for motivated henchmen
	motivatedWaterSpeedBonus = 10.0,

	-- damage multiplier henchman do against other henchman
	henchmanDmgBonus = 1.5,

	-- damage multiplier for sonic attack on henchman
	sonicDmgMult = 0.25,

	-- damage multiplier for burst attack on henchman
	burstDmgMult = 0.25,

	-- damage multiplier for artillery attack on henchmen
	artilleryDmgMult = 0.20,

	-- damage multiplier for direct ranged attack on henchmen
	directRangedDmgMult = 0.5,
	
	-- how much damage extra do we do against henchman when we are within our lab bonus radius
	hencmanLabDmgMult = 3.0,
	
	-- how large is the lab morale bonus area
	labMoraleRad = 40.0,
}

---------------------------------------------------------------------
-- Frenzy

Frenzy =
{

	-- Endurance cost per second for the frenzy attack
	endurancePerSecond	= 10,
	
	-- Damage issued multiplier, i.e. I do normal damage times x, when frenzied
	dmgIssuedMult		= 2.0,

	-- Damage received multiplier, i.e. I take normal damage times x, when frenzied
	dmgReceivedMult		= 1.25,

	-- When a creature is fenzied it's movement rate is multiplied by x,
	moveRateMult		= 1.1,

	-- Minimum ammount of endurance needed to trigger frenzy attack
	enduranceMinimum	= 35,
}

---------------------------------------------------------------------
-- SonarPulse

SonarPulse =
{
	-- Endurance cost for the sonar pulse
	enduranceCost		= 100.0,
	
	-- Reveal Radius, the radius of the area revealed by the pulse
	revealRadius		= 25.0,

	-- Reveal timeout, how many seconds does the reveal area last for
	duration			= 30.0,
}

---------------------------------------------------------------------
-- SoiledLand

SoiledLand =
{
	-- Endurance cost per tick for the soiled mode
	endurancePerTick			= 0.7,
	
	-- Endurance cost per tick for the soiled mode
	endurancePerTickForFlyer	= 1.2,
	
	-- Random drop zone size for flyer soiling land 
	-- (1=1x1 cell, 2=2x2 cells, 3=3x3 cells, etc around the position of the flyer)
	dropZoneSize		= 3,
	
	-- Number of misses before another soiled land generated from flyers
	dropMiss			= 1,

	-- Soiled land will damage creatures within this effective radius
	findTargetRadius	= 1.5,
	
	-- 1 = Percentage based damage (range 0-100) ; 0 = point based damage
	dmgIsPercentage		= 1,
	
	-- Damage (percentage or point-based) received by the soiled victim per tick
	dmgPerTick			= 1.2,
	
	-- The amount to multiply the creature's speed by when soiled (normal speed = 1.0)
	speedMultiplier		= 0.5,
	
	-- The time (in ticks) it takes the soiled damage on animal to expire 
	-- after leaving the soiled land
	durationDmg			= 50,

	-- The time (in ticks) it takes for the soiled land to expire 
	-- and change back to normal land
	durationLand		= 500,

	-- Minimum amount of endurance needed to trigger soiled land
	enduranceMinimum	= 35,
}

---------------------------------------------------------------------
-- Plague

Plague =
{
	-- How much damage the plague does per second, per attacker rank
	damagePerSecond1	= 6.0,
	damagePerSecond2	= 7.0,
	damagePerSecond3	= 8.0,
	damagePerSecond4	= 9.0,
	damagePerSecond5	= 10.0,
	
	-- How long (seconds) does the plague last
	timeSeconds			= 10.0,

	-- How far does a plagued entity search to spread the plague
	spreadRadius		= 12.5,

	-- How much endurance does the plague use
	enduranceCost		= 100.0,

	-- plague will transfer every this number of seconds
	transferRateSeconds	= 1.5
}

---------------------------------------------------------------------
-- Player Info

Player =
{
	-- starting resources for a player (Standard)
	starting_gather_res = 500.0,
	starting_renew_res  = 100.0,
	
	-- starting resources for a player (QuickStart)
	quickstart_gather_res = 2000.0,
	quickstart_renew_res  = 500.0,

	-- resource amount modifier for scrap yards
	resource_mod_low	  = 0.5,
	resource_mod_high	  = 2.0,

	-- percentage of the donation that is lost during the transaction
	donationPenaltyPercentage = 0.25,
}

---------------------------------------------------------------------
-- AIPlayer Info

AIPlayer =
{
	-- electricity bonus for AI
	resRenewBonusEasy = 1.0,
	resRenewBonusStandard = 1.0,
	resRenewBonusHard = 1.0,
	resRenewBonusHardest = 1.2,
	
	-- coal bonus for AI
	resGatherBonusEasy = 1.0,
	resGatherBonusStandard = 1.0,
	resGatherBonusHard = 1.0,
	resGatherBonusHardest = 1.2,
}

---------------------------------------------------------------------
-- Stance Cap

Stance =
{
	-- Radius that guys will attack within when in Territorial Stance
	territoryRadius		= 20.0,
}


---------------------------------------------------------------------
-- Diplomacy

Diplomacy =
{
	-- Radius that guys will attack within when in Territorial Stance
	scrapDonationInc	= 100.0,
	electricityDonationInc	= 50.0,
}

---------------------------------------------------------------------
-- Animal

Animal =
{
	-- maximum distance for wandering from starting point
	movementRadius   = 50,

	-- maximum delay after stopping for wandering 
	movementMaxDelay = 240,

	-- multiplier to slow down animals when they wander
	movementSpeedMult = 0.2,

	-- search radius for threat avoidance
	threatAvoidRadius = 16.0,

	-- time to wait between checking for threats
	threatAvoidCoolTicks = 4,
}

---------------------------------------------------------------------
-- Sabotage

Sabotage =
{
	-- damage per tick
	damagePerTick 	= 10.0,

	-- total amount of damage
	damage		= 2000.0,
}

---------------------------------------------------------------------
-- Flyer

Flyer =
{
	-- the height above terrain that flyers fly at.  In meters.
	flyingHeight 	= 8.0,

	-- stopped speed, meters per second
	stoppedSpeed 	= 1.5,

	-- fliers will attack every... this number of seconds
	secondsPerAttack = 4.0,

	-- number of ticks it takes a flyer to swoop down to deliver a triggered attack
	swoopDownTicks = 5,

	-- number of ticks it takes a flyer to follow thru after swooping down
	swoopUpTicks = 5,

	-- the number of seconds of damage a flyer's first attack counts as
	firstAttackSeconds = 1.0,

	-- melee damage multiplier
	meleeDmgMult = 1.0,

	-- ranged damage multiplier
	rangedDmgMult = 1.0,

	-- artillery damage multiplier
	artilleryDmgMult = 1.0,

	-- value to increase the min range of ranged attacks
	minRangeOffset = 0.0,
	
	-- damage multiplier for damage done to henchman during melee attack
	-- e.g. A value of 0.25 means henchmen receive 25% damage 
	meleeDmgMultHenchman = 0.5,	
}

---------------------------------------------------------------------
-- GarrisonHeal

GarrisonHeal =
{
	-- amount of healing to apply per tick to garrisoned entities
	healPerTick	= 2.0,
}

---------------------------------------------------------------------
-- AttackGround

AttackGround =
{
	-- the error radius, so that AttackGround aim isn't perfect
	errorRadius	= 3.0,
}

---------------------------------------------------------------------
-- CreatureUpgrade

CreatureUpgrade =
{
	-- amount to add to a creature's defense
	-- (defense is represented as a percentage)
	defenseBonus = 10,

	-- amount to add to a creature's speed
	speedBonus = 5,

	-- flag that indicates whether the melee damage bonuses
	--  should be interpreted as a multiplier or a flat number
	--  1 for multipliers.  
	--  0 for flat numbers.
	meleeDamageBonusAsMult = 1,

	-- melee damage bonuses
	meleeDamageBonusRank1 = 1.2,
	meleeDamageBonusRank2 = 1.2,
	meleeDamageBonusRank3 = 1.2,
	meleeDamageBonusRank4 = 1.2,
	meleeDamageBonusRank5 = 1.2,

	-- flag that indicates whether the hitpoints bonuses
	--  should be interpreted as a multiplier or a flat number
	--  1 for multipliers.  
	--  0 for flat numbers.
	hitpointBonusAsMult = 1,

	-- hitpoints bonuses
	hitpointsBonusRank1 = 1.2,
	hitpointsBonusRank2 = 1.2,
	hitpointsBonusRank3 = 1.2,
	hitpointsBonusRank4 = 1.2,
	hitpointsBonusRank5 = 1.2,

	-- amount to add to a creature's sight radius 
	sightRadiusBonus = 10,

	-- flag that indicates whether the ranged damage bonuses
	--  should be interpreted as a multiplier or a flat number
	--  1 for multipliers.  
	--  0 for flat numbers.
	rangedDamageBonusAsMult = 1,

	-- ranged damage bonuses
	rangedDamageBonusRank1 = 1.15,
	rangedDamageBonusRank2 = 1.15,
	rangedDamageBonusRank3 = 1.15,
	rangedDamageBonusRank4 = 1.15,
	rangedDamageBonusRank5 = 1.15,

	-- amount to reduce an entity's defense by when it is 
	--  attacked by an attacker with splash-damage upgrade
	splashDmgDefenseMultiplier = 0.15,

	-- area attack radius multiplier
	areaAttackRadiusMult = 1.25,
}
---------------------------------------------------------------------
-- AttackGround

Death =
{
	--	 ticks to fade out body
	deathTicks	= 3*8.0,
	--	((tick-startTick)/deathTicks)^deathFadeOutCurve ... determines the fade out curve
	deathFadeOutCurve = 2.0,
}

---------------------------------------------------------------------
-- Construction

Construction =
{
	--	 the max distance (in meters) to search in order to find a construction site to move to after the current one is finished
	constructionSiteSearchRadius = 15.0,

}

---------------------------------------------------------------------
-- FogOfWar

FogOfWar =
{
	--	 the number of seconds an attacker is reveal in the victims FoW
	attackerRevealTime = 4.0,

	--	 the number of seconds a projectile is reveal in the victims FoW
	projectileRevealTime = 10.0,

}

---------------------------------------------------------------------
-- UI

UI =
{
	-- delay between event cue henchman idle
	henchmanidle = 160,

	-- battle track
	btrackTimeTracked =  5,
	btrackTimeMin     = 30,
	btrackCountBegin  = 10,
	btrackCountEnd    =  5,
}

---------------------------------------------------------------------
-- AutoDefense

AutoDefense =
{
	radius = 4.0,
	rechargeTicks = 64,
	durationTicks = 64,
}

---------------------------------------------------------------------
-- Swamp Slowdown

SwampSlow =
{
	-- Swamp slowdown multiplier per size
	-- Valid range is  > 0.0 (terrain is impassible) 
	--                <= 1.0 (no slowdown)
	-- e.g. 0.75 is slowdown to 75% of previous speed
	slowdown1		= 1.0,
	slowdown2		= 1.0,
	slowdown3		= 1.0,
	slowdown4		= 0.9,
	slowdown5		= 0.8,
	slowdown6		= 0.7,
	slowdown7		= 0.6,
	slowdown8		= 0.4,
	slowdown9		= 0.2,
	slowdown10		= 0.1,
}

---------------------------------------------------------------------
-- Jumping (Short Range Flight)

Jumping =
{
	-- Endurance cost to use this ability
	enduranceCost		= 50.0,
	
	-- Maximum jump distance 
	maxDistance			= 100.0,
		
	-- Speed while jumping, in meters per tick
	speed				= 0.8,
	
	-- Height at the top of the parabola 
	maxHeight			= 7.0,
		
	-- The maximum distance of horizontal deviation from a straight line path
	-- (since the flight path is erratic)
	maxPathDeviation	= 1.5,
	
	-- A multiplier for the amplitude of the vertical wave-like flight behaviour 
	-- Valid range is >= 0.0 (no wave)
	--                <= 1.0 (hitting the ground)
	amplitudeMultiplier	= 0.25,
	
	-- Ther period of the vertical wave-like flight behaviour
	-- e.g. 8 means there it takes 8 ticks per wave
	periodTicks			= 8,
}

---------------------------------------------------------------------
-- Resource Conversion

ResourceConversion =
{
	-- A multiplier for the conversion of electricity to coal
	-- e.g. 0.25 means 50 electricity -> 12.5 coal
	elecToCoal = 0.30,
	
	-- A multiplier for the conversion of coal to electricity
	-- e.g. 0.25 means 50 coal -> 12.5 electricity
	coalToElec = 0.30,
}

---------------------------------------------------------------------
-- Friendly Fire

FriendlyFire =
{
	-- These are multipliers for damage reduction for friendly fire
	-- e.g. A value of 0.25 means friendly units receive 25% damage
	-- valid range is >= 0.0 (no damage received) 
	--                <= 1.0 (usual damaged received)
	
	-- burst attacks
	electricBurstMultiplier	= 0.5,
	quillBurstMultiplier	= 0.5,
	
	-- artillery attacks
	rockMultiplier			= .5,
	waterSpitMultiplier		= .5,
	chemSprayMultiplier		= .5,
}

---------------------------------------------------------------------
-- Chemical Spray

ChemicalSpray = 
{
	-- A multiplier for damage reduction for the first strike
	-- e.g. A value of 0.25 means 25% of usual damage on the first strike
	-- valid range is >= 0.0 (no damage received on first strike)
	--                <= 1.0 (same damage as usual on first strike)
	firstStrikeMultiplier = 1.0,
}

---------------------------------------------------------------------
-- Movement

Movement = 
{
	-- valid range is >= 1.0 ( minimun min speed all units can move)
	--                <= 20.0 (maximum min speed all units can move)
	minMoveSpeed = 3.0,
}
