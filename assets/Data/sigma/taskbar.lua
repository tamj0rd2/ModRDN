---------------------------------------------------------------------
-- File    : taskbar.lua
-- Desc    : 
-- Created : Wednesday, June 06, 2001
-- Author  : 
-- 
-- (c) 2001 Relic Entertainment Inc.
-- 

-- * in-game taskbar script

--
-- hotkeys	


	--GENERIC
	HK_System_Escape		= "keygroups.systemcommands.keys.escape"
	HK_System_Accept		= "keygroups.systemcommands.keys.accept"
	HK_System_CommandQueue	= "keygroups.systemcommands.keys.commandqueue"
	HK_System_Focus         = "keygroups.cameracontrol.keys.focus"

	HK_System_ZoomCameraIn  = "keygroups.cameracontrolfree.keys.keyzoomin"
	HK_System_ZoomCameraOut = "keygroups.cameracontrolfree.keys.keyzoomout"

	HK_Generic_Menu				= "keygroups.gamecommands.keys.showpausemenu"
	HK_Generic_Ally				= "keygroups.gamecommands.keys.showallymenu"
	HK_Generic_ArmyBuilder		= "keygroups.gamecommands.keys.showarmybuilder"
	HK_Generic_Players			= "keygroups.gamecommands.keys.showdiplomacymenu"
	HK_Generic_Objectives		= "keygroups.gamecommands.keys.showobjectivesmenu"
	HK_Generic_Event			= "keygroups.gamecommands.keys.gotolastevent"
	HK_Generic_TeamObjective	= "keygroups.gamecommands.keys.teamobjective"
	HK_Generic_RecallSpeech		= "keygroups.gamecommands.keys.recallspeech"

	-- ARMY BUTTONS
	HK_Build_Creature_1		= "keygroups.chambercommands.keys.buildcreature1"
	HK_Build_Creature_2		= "keygroups.chambercommands.keys.buildcreature2"
	HK_Build_Creature_3		= "keygroups.chambercommands.keys.buildcreature3"
	HK_Build_Creature_4		= "keygroups.chambercommands.keys.buildcreature4"
	HK_Build_Creature_5		= "keygroups.chambercommands.keys.buildcreature5"
	HK_Build_Creature_6		= "keygroups.chambercommands.keys.buildcreature6"
	HK_Build_Creature_7		= "keygroups.chambercommands.keys.buildcreature7"
	HK_Build_Creature_8		= "keygroups.chambercommands.keys.buildcreature8"
	HK_Build_Creature_9		= "keygroups.chambercommands.keys.buildcreature9"

	-- CREATURE
	HK_Generic_Attack		= "keygroups.basicunitcommands.keys.attack"
	HK_Generic_Guard		= "keygroups.creaturecommands.keys.guard"
	HK_Generic_Patrol		= "keygroups.creaturecommands.keys.patrol"
	HK_Creature_NoStance		= "keygroups.creaturecommands.keys.stance_none"
	HK_Creature_Passive		= "keygroups.creaturecommands.keys.stance_passive"
	HK_Creature_Territorial		= "keygroups.creaturecommands.keys.stance_territorial"
	HK_Creature_Aggressive		= "keygroups.creaturecommands.keys.stance_aggressive"
	HK_Creature_RangeAttack		= "keygroups.creaturecommands.keys.rangeattack"
	HK_Creature_Special_Stink	= "keygroups.creaturecommands.keys.special_stink"
	HK_Creature_Special_Burst	= "keygroups.creaturecommands.keys.special_burst"
	HK_Creature_Special_Quill   	= "keygroups.creaturecommands.keys.special_quill"
	HK_Creature_Special_Frenzy	= "keygroups.creaturecommands.keys.special_frenzy"
	HK_Creature_Special_Digin	= "keygroups.creaturecommands.keys.special_digin"
	HK_Creature_Special_Digout	= "keygroups.creaturecommands.keys.special_digin"
	HK_Creature_Special_Jumping	= "keygroups.creaturecommands.keys.special_jumping"
	HK_Creature_Special_Dam		= "keygroups.creaturecommands.keys.special_dam"
	HK_Creature_Special_Sonar	= "keygroups.creaturecommands.keys.special_sonar"
	HK_Creature_Special_Plague	= "keygroups.creaturecommands.keys.special_plague"
	HK_Creature_Special_Web		= "keygroups.creaturecommands.keys.special_web"
	HK_Creature_Special_Assassinate	= "keygroups.creaturecommands.keys.special_assassinate"
	HK_Creature_Special_Flash	= "keygroups.creaturecommands.keys.special_flash"
	HK_Creature_Special_Infestation	= "keygroups.creaturecommands.keys.special_infestation"
	HK_Creature_Special_SoiledLand	= "keygroups.creaturecommands.keys.special_soiledland"

	-- HENCHMEN
	HK_Henchman_Build			= "keygroups.basicunitcommands.keys.buildmenu"
	HK_Henchman_BuildAdvanced		= "keygroups.basicunitcommands.keys.buildmenuadvanced"
	HK_Henchman_Repair			= "keygroups.basicunitcommands.keys.repair"
	HK_Henchman_Gyrocopter		= "keygroups.henchmancommands.keys.gyrocopter"
	HK_Henchman_Heal			= "keygroups.henchmancommands.keys.heal"
	HK_Henchman_Gather			= "keygroups.basicunitcommands.keys.gather"
	HK_Henchman_Tag			= "keygroups.henchmancommands.keys.tag"
	HK_Henchman_BuildToggle		= "keygroups.henchmanbuildcommands.keys.buildtoggle"
	HK_Henchman_Unload			= "keygroups.henchmancommands.keys.unload"
	HK_Henchman_Airlift			= "keygroups.henchmancommands.keys.airlift"
	HK_Henchman_Untag			= "keygroups.henchmancommands.keys.untag"

	-- HENCHMEN BUILD MENU
	HK_Henchman_Build_Lightning			= "keygroups.henchmanbuildcommands.keys.lightning"
	HK_Henchman_Build_Electrical		= "keygroups.henchmanbuildcommands.keys.electrical"
	HK_Henchman_Build_Bramble			= "keygroups.henchmanbuildcommands.keys.bramble"
	HK_Henchman_Build_Remote			= "keygroups.henchmanbuildcommands.keys.remote"
	HK_Henchman_Build_Water				= "keygroups.henchmanbuildcommands.keys.water"
	HK_Henchman_Build_Aviary			= "keygroups.henchmanbuildcommands.keys.aviary"
	HK_Henchman_Build_VetClinic			= "keygroups.henchmanbuildcommands.keys.vetclinic"
	HK_Henchman_Build_Foundry			= "keygroups.henchmanbuildcommands.keys.foundry"
	HK_Henchman_Build_AntiAirTower		= "keygroups.henchmanbuildcommands.keys.antiairtower"
	HK_Henchman_Build_SoundBeamTower	= "keygroups.henchmanbuildcommands.keys.soundbeamtower"
	HK_Henchman_Build_LandingPad		= "keygroups.henchmanbuildcommands.keys.landingpad"
	HK_Henchman_Build_GeneticAmplifier	= "keygroups.henchmanbuildcommands.keys.geneticamp"

	-- REX
	HK_Rex_Heal			= "keygroups.rexcommands.keys.heal"
	HK_Rex_GatherDNA	= "keygroups.rexcommands.keys.gatherdna"
	HK_Rex_Garrison		= "keygroups.rexcommands.keys.garrison"

	-- LUCY
	HK_Lucy_Repair			= "keygroups.basicunitcommands.keys.repair"
	HK_Lucy_Garrison		= "keygroups.lucycommands.keys.garrison"
	HK_Lucy_Build			= "keygroups.basicunitcommands.keys.buildmenu"
	HK_Lucy_BuildAdvanced	= "keygroups.basicunitcommands.keys.buildmenuadvanced"
	HK_Lucy_GatherTech		= "keygroups.lucycommands.keys.gathertech"
	HK_Lucy_Sabotage		= "keygroups.lucycommands.keys.sabotage"
	HK_Lucy_Gather			= "keygroups.basicunitcommands.keys.gather"

	-- LAB
	HK_Lab_CreateHenchman	= "keygroups.labcommands.keys.createhenchman"
	HK_Lab_ReleaseRex		= "keygroups.labcommands.keys.releaserex"
	HK_Lab_ReleaseLucy		= "keygroups.labcommands.keys.releaselucy"
	HK_Lab_RadarPulse		= "keygroups.labcommands.keys.radarpulse"
	HK_Lab_DefenseTurnOn	= "keygroups.labcommands.keys.defenseturnon"
	HK_Lab_DefenseTurnOff	= "keygroups.labcommands.keys.defenseturnoff"

	-- FOUNDRY
	HK_Foundry_ElecToCoal100 = "keygroups.foundrycommands.keys.electocoal100"
	HK_Foundry_ElecToCoal500 = "keygroups.foundrycommands.keys.electocoal500"
	HK_Foundry_CoaltoElec100 = "keygroups.foundrycommands.keys.coaltoelec100"
	HK_Foundry_CoaltoElec500 = "keygroups.foundrycommands.keys.coaltoelec500"

	-- LANDINGPAD
	HK_LandingPad_CreateGyrocopter = "keygroups.landingpadcommands.keys.creategyrocopter"


	-- basic unit
	HK_Generic_Stop	= "keygroups.basicunitcommands.keys.stop"
	HK_Generic_Move	= "keygroups.basicunitcommands.keys.move"
	HK_Generic_Kill = "keygroups.basicunitcommands.keys.kill"

	-- selection
	HK_Select_UnitsOnScreen				= "keygroups.select.keys.unitsonscreen"
	HK_Select_HenchmenOnScreen			= "keygroups.select.keys.henchmenonscreen"
	HK_Select_HenchmenInWorld			= "keygroups.select.keys.henchmeninworld"
	HK_Select_HenchmenNextIdle			= "keygroups.select.keys.henchmennextidle"
	HK_Select_HenchmenAllIdle			= "keygroups.select.keys.henchmenallidle"
	HK_Select_CombatUnitsOnScreen		= "keygroups.select.keys.combatonscreen"
	HK_Select_CombatUnitsInWorld		= "keygroups.select.keys.combatinworld"
	HK_Select_GroundCombatUnitsOnScreen	= "keygroups.select.keys.groundcombatonscreen"
	HK_Select_GroundCombatUnitsInWorld	= "keygroups.select.keys.groundcombatinworld"
	HK_Select_AirCombatUnitsOnScreen	= "keygroups.select.keys.aircombatonscreen"
	HK_Select_AirCombatUnitsInWorld		= "keygroups.select.keys.aircombatinworld"
	HK_Select_WaterCombatUnitsOnScreen	= "keygroups.select.keys.watercombatonscreen"
	HK_Select_WaterCombatUnitsInWorld	= "keygroups.select.keys.watercombatinworld"
	HK_Select_NextGroundCombiner		= "keygroups.select.keys.nextgroundcombiner"
	HK_Select_NextWaterCombiner			= "keygroups.select.keys.nextwatercombiner"
	HK_Select_NextAirCombiner			= "keygroups.select.keys.nextaircombiner"
	HK_Select_Lab						= "keygroups.select.keys.lab"
	HK_Select_Rex						= "keygroups.select.keys.rex"
	HK_Select_Lucy						= "keygroups.select.keys.lucy"
	HK_Select_NextSubSelection			= "keygroups.select.keys.nextsubselect"

	-- assignment to hotkeygroups
	HK_Assign_ToGroup0					= "keygroups.hotkeygroups.keys.assigntogroup0"
	HK_Assign_ToGroup1					= "keygroups.hotkeygroups.keys.assigntogroup1"
	HK_Assign_ToGroup2					= "keygroups.hotkeygroups.keys.assigntogroup2"
	HK_Assign_ToGroup3					= "keygroups.hotkeygroups.keys.assigntogroup3"
	HK_Assign_ToGroup4					= "keygroups.hotkeygroups.keys.assigntogroup4"
	HK_Assign_ToGroup5					= "keygroups.hotkeygroups.keys.assigntogroup5"
	HK_Assign_ToGroup6					= "keygroups.hotkeygroups.keys.assigntogroup6"
	HK_Assign_ToGroup7					= "keygroups.hotkeygroups.keys.assigntogroup7"
	HK_Assign_ToGroup8					= "keygroups.hotkeygroups.keys.assigntogroup8"
	HK_Assign_ToGroup9					= "keygroups.hotkeygroups.keys.assigntogroup9"
	HK_Unassign_FromAllGroups				= "keygroups.select.keys.unassignallgroups"

	-- basic building units
	-- lab
	HK_Research_AdvancedStruct	= "keygroups.labcommands.keys.research_advancedstruct"
	HK_Research_Rank2		= "keygroups.labcommands.keys.research"
	HK_Research_Rank3		= "keygroups.labcommands.keys.research"
	HK_Research_Rank4		= "keygroups.labcommands.keys.research"
	HK_Research_Rank5		= "keygroups.labcommands.keys.research"
	
	-- research clinic
	HK_Research_HenchmanBinoculars	= "keygroups.researchcliniccommands.keys.research_henchmanbinoculars"
	HK_Research_HenchmanTag			= "keygroups.researchcliniccommands.keys.research_henchmantag"
	HK_Research_HenchmanYoke		= "keygroups.researchcliniccommands.keys.research_henchmanyoke"
	HK_Research_HenchmanHeal		= "keygroups.researchcliniccommands.keys.research_henchmanheal"
	HK_Research_HenchmanMotSpeech	= "keygroups.researchcliniccommands.keys.research_henchmanmotspeech"
	HK_Research_IncBuildIntegrity	= "keygroups.researchcliniccommands.keys.research_incbuildintegrity"
	HK_Research_TowerUpgrade		= "keygroups.researchcliniccommands.keys.research_towerupgrade"
	HK_Research_StrengthenFences	= "keygroups.researchcliniccommands.keys.research_strengthenfences"
	HK_Research_StrengthenElecGrid	= "keygroups.researchcliniccommands.keys.research_strengthenelecgrid"
	HK_Research_LabDefense			= "keygroups.researchcliniccommands.keys.research_labdefense"
	
	-- sound beam tower
	HK_SoundBeam_SonicBoom			= "keygroups.soundbeamcommands.keys.sonicboom"
	
	-- anti air tower
	HK_AntiAir_AirBurst				= "keygroups.antiaircommands.keys.airburst"
	
	-- electrical generator
	HK_AddOn_EGen1			= "keygroups.egencommands.keys.research"
	HK_AddOn_EGen2			= "keygroups.egencommands.keys.research"
	HK_AddOn_EGen3			= "keygroups.egencommands.keys.research"
	
	-- genetic amplifier
	HK_Upgrade_Defense			= "keygroups.geneticamplifiercommands.keys.upgrade_defense"
	HK_Upgrade_Speed			= "keygroups.geneticamplifiercommands.keys.upgrade_speed"
	HK_Upgrade_MeleeDamage		= "keygroups.geneticamplifiercommands.keys.upgrade_meleedamage"
	HK_Upgrade_HitPoints		= "keygroups.geneticamplifiercommands.keys.upgrade_hitpoints"
	HK_Upgrade_SightRadius		= "keygroups.geneticamplifiercommands.keys.upgrade_sightradius"
	HK_Upgrade_RangedDamage		= "keygroups.geneticamplifiercommands.keys.upgrade_rangeddamage"
	HK_Upgrade_SplashDamage		= "keygroups.geneticamplifiercommands.keys.upgrade_splashdamage"
	HK_Upgrade_AreaAttackRadius	= "keygroups.geneticamplifiercommands.keys.upgrade_areaattackradius"
	
	HK_Generic_Rally	= "keygroups.basicbuildingcommands.keys.rally"

	-- playback
	HK_Playback_SpeedNormal		= "keygroups.playbackcommands.keys.speednormal"
	HK_Playback_SpeedFast		= "keygroups.playbackcommands.keys.speedfast"

	-- hotkeygroup assignment
	hotkeygroups = 
	{
		HK_Assign_ToGroup1,
		HK_Assign_ToGroup2,
		HK_Assign_ToGroup3,
		HK_Assign_ToGroup4,
		HK_Assign_ToGroup5,
		HK_Assign_ToGroup6,
		HK_Assign_ToGroup7,
		HK_Assign_ToGroup8,
		HK_Assign_ToGroup9,
		HK_Assign_ToGroup0,
	}

	-- creatures buttons
	armybuttons =
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
	}

	armyunit_hotkeys = 
	{
		HK_Build_Creature_1,	
		HK_Build_Creature_2,
		HK_Build_Creature_3,
		HK_Build_Creature_4,
		HK_Build_Creature_5,
		HK_Build_Creature_6,
		HK_Build_Creature_7,
		HK_Build_Creature_8,
		HK_Build_Creature_9,
	}

	--
	armyupgradebuttons =
	{
		{ "upgrade01_label", "upgrade_icon01", "upgrade01_complete" },
		{ "upgrade02_label", "upgrade_icon02", "upgrade02_complete" },
		{ "upgrade03_label", "upgrade_icon03", "upgrade03_complete" },
		{ "upgrade04_label", "upgrade_icon04", "upgrade04_complete" },
		{ "upgrade05_label", "upgrade_icon05", "upgrade05_complete" },
		{ "upgrade06_label", "upgrade_icon06", "upgrade06_complete" },
		{ "upgrade07_label", "upgrade_icon07", "upgrade07_complete" },
		{ "upgrade08_label", "upgrade_icon08", "upgrade08_complete" },
		{ "upgrade09_label", "upgrade_icon09", "upgrade09_complete" },
	}
	
	--
	menu_commands = 
	{
		{ 40950, HK_Generic_Menu,		42380, },
		{ 40951, HK_Generic_ArmyBuilder,42381, }, 
		{ 40952, HK_Generic_Players,	42383, },
		{ 40953, HK_Generic_Ally,		42382, }, 
		{ 40954, HK_Generic_Objectives,	42384, },
		{ 40955, HK_Generic_TeamObjective, 42385, },
		{ 40956, HK_Generic_RecallSpeech, 42386, },
	}

	-- scrap,electricity,population cap
	resource_indicator_tooltips = 
	{
		{ 40750, "",	42400, },
		{ 40751, "",	42401, }, 
		{ 40752, "",	42402, },
	}

	henchman_commands = 
	{
		{ 40820,	HK_Generic_Move,			42320,	"ui/ingame/henchmen_move.tga" },
		{ 40821,	HK_Henchman_Gather,			42322,	"ui/ingame/gather.tga" },
		{ 40822,	HK_Generic_Guard,			42323,	"ui/ingame/henchmen_guard.tga" },
		{ 40823,	HK_Henchman_Repair,			42324,	"ui/ingame/repair.tga" },
		{ 40824,	HK_Generic_Stop,			42326,	"ui/ingame/henchmen_stop.tga" },

		{ 40825,	HK_Henchman_Build,			42327,	"ui/ingame/build_structure.tga" },
		{ 40834,	HK_Henchman_BuildAdvanced,	42345,	"ui/ingame/build_structure_advanced.tga" },

		{ 40826,	HK_Henchman_Heal,			42325,	"ui/ingame/heal.tga" },
		{ 40827,	HK_Henchman_Tag,			42332,	"ui/ingame/tag.tga" },
		{ 40829,	HK_Generic_Kill,			42329,	"ui/ingame/kill.tga" },
		{ 40835,	HK_Henchman_Unload,			42346,	"ui/ingame/unload.tga" },
		{ 40856,	HK_Generic_Attack,			42330,	"ui/ingame/henchmen_attack.tga" },
		{ 40836,	HK_Henchman_Airlift,		42348,	"ui/ingame/airlift.tga" },
		{ 40837,	HK_Henchman_Untag,		42349,  "ui/ingame/untag.tga" },
	}

	creature_commands = 
	{
		{ 40850,	HK_Generic_Move,		42320,	"ui/ingame/waypoints.tga" },
		{ 40851,	HK_Generic_Guard,		42323,	"ui/ingame/guard.tga" },
		{ 40852,	HK_Generic_Patrol,		42331,	"ui/ingame/patrol.tga" },
		{ 40853,	HK_Generic_Stop,		42326,	"ui/ingame/stop.tga" },
		{ 40854,	HK_Creature_RangeAttack,42333,	"ui/ingame/range_attack.tga" },

		{ 40855,	HK_Generic_Kill,		42329,	"ui/ingame/kill.tga" },
		{ 40856,	HK_Generic_Attack,		42330,	"ui/ingame/special.tga" },
	}

	stance_commands = 
	{
		{ 40870,	HK_Creature_Aggressive,	42340,	"ui/ingame/aggressive.tga",		STANCE_Aggressive	},
		{ 40871,	HK_Creature_Territorial,42341,	"ui/ingame/territorial.tga",	STANCE_Territorial	},
		{ 40872,	HK_Creature_Passive,	42342,	"ui/ingame/passive.tga",		STANCE_Passive		},
		{ 40873,	HK_Creature_NoStance,	42343,	"ui/ingame/stance_none.tga",	STANCE_None			},
	}

	-- Note: the entries in ability_commands MUST follow the order of
	--       the SpecialAbilities enumerated type
	ability_commands = 
	{
		{ 40893,	HK_Creature_Special_Frenzy,		42423, "ui/ingame/frenzy_attack.tga",	Ability_Frenzy,		0	},
		{ 40910,	HK_Creature_Special_Sonar,		42440, "ui/ingame/sonar_attack.tga",	Ability_SonarPulse,	1	},
		{ 40903,	HK_Creature_Special_SoiledLand,	42433, "ui/ingame/soiled_land.tga",		Ability_SoiledLand,	0	},
		{ 40912,	HK_Creature_Special_Jumping,	42442, "ui/ingame/jumping.tga",			Ability_Jumping,	1	},
		{ 40894,	HK_Creature_Special_Digin,		42424, "ui/ingame/digin.tga",			Ability_Digin,		0	},
		{ 40898,	HK_Creature_Special_Digout,		42428, "ui/ingame/digout.tga",			Ability_Digout,		0	},
	}

	modal_special_commands =
	{
		-- Modal Special commands
		{ 40911,	HK_Creature_Special_Plague,         42441, "ui/ingame/plague.tga",		    ATTACKTRG_Plague		},
		{ 40891,	HK_Creature_Special_Burst,          42421, "ui/ingame/burst_attack.tga",	ATTACKTRG_ElectricBurst },
		{ 40897,	HK_Creature_Special_Quill,          42427, "ui/ingame/quill_attack.tga",	ATTACKTRG_QuillBurst	},
		{ 40890,	HK_Creature_Special_Stink,          42420, "ui/ingame/stink_attack.tga",	ATTACKTRG_StinkBurst	},
		{ 40899,	HK_Creature_Special_Web,            42429, "ui/ingame/web_throw.tga",	    ATTACKTRG_WebThrow		},
		{ 40900,	HK_Creature_Special_Assassinate,    42430, "ui/ingame/assassinate.tga",	    ATTACKTRG_Assassinate	},
		{ 40901,	HK_Creature_Special_Flash,          42431, "ui/ingame/flash.tga",	        ATTACKTRG_Flash			},
		{ 40902,	HK_Creature_Special_Infestation,	42432, "ui/ingame/infestation.tga",		ATTACKTRG_Infestation	},
		{ 40833,	HK_Lucy_Sabotage,                   42344, "ui/ingame/sabotage.tga",	    ATTACKTRG_Sabotage		},
	}

	rex_commands = 
	{
		{ 40820,	HK_Generic_Move,			42320, "ui/ingame/henchmen_move.tga" },
		{ 40822,	HK_Generic_Guard,			42323, "ui/ingame/henchmen_guard.tga" },
		{ 40824,	HK_Generic_Stop,			42326, "ui/ingame/henchmen_stop.tga" },
		{ 40826,	HK_Rex_Heal,				42325, "ui/ingame/heal.tga" },

		{ 40830,	HK_Rex_GatherDNA,			42334, "ui/ingame/gather_dna.tga" },
		{ 40831,	HK_Rex_Garrison,			42335, "ui/ingame/garrison.tga" },
	}

	lucy_commands = 
	{
		{ 40820,	HK_Generic_Move,			42320, "ui/ingame/henchmen_move.tga" },
		{ 40822,	HK_Generic_Guard,			42323, "ui/ingame/henchmen_guard.tga" },
		{ 40824,	HK_Generic_Stop,			42326, "ui/ingame/henchmen_stop.tga" },
		{ 40823,	HK_Lucy_Repair,				42324, "ui/ingame/repair.tga" },
		{ 40832,	HK_Lucy_GatherTech,			42339, "ui/ingame/gather_tech.tga" },
		{ 40833,	HK_Lucy_Sabotage,			42344, "ui/ingame/sabotage.tga" },

		{ 40831,	HK_Lucy_Garrison,			42335, "ui/ingame/garrison.tga" },

		{ 40825,	HK_Lucy_Build,				42338, "ui/ingame/build_structure.tga" },
		{ 40834,	HK_Lucy_BuildAdvanced,		42347, "ui/ingame/build_structure_advanced.tga" },	

		{ 40821,	HK_Lucy_Gather,				42322, "ui/ingame/gather.tga" },
	}

	commands = 
	{
		{ 40930,	HK_Generic_Rally,				42370,	"ui/ingame/rallypoint.tga" },
		{ 40931,	HK_Lab_ReleaseRex,				42336,	"ui/ingame/release_rex.tga" },
		{ 40932,	HK_Lab_ReleaseLucy,				42337,	"ui/ingame/release_lucy.tga" },
		{ 40829,	HK_Generic_Kill,				42329,	"ui/ingame/structure_scuttle.tga" },
	}

	radarpulsecommand = 
	{ 42444,	HK_Lab_RadarPulse,		42443,	"ui/ingame/radar_pulse.tga" }

	soundbeamcommand =
	{ 42464,	HK_SoundBeam_SonicBoom,	42463,	"ui/ingame/tower_boost.tga" }
	
	antiaircommand =
	{ 42466,	HK_AntiAir_AirBurst,	42465,	"ui/ingame/tower_boost.tga" }
	
	labcommands = 
	{
		-- Name		Hokey					TT Text		Texture									Command
		{ 42462,	HK_Lab_DefenseTurnOff,	42461,		"ui/ingame/lab_defense_stop.tga",		0 },
		{ 42460,	HK_Lab_DefenseTurnOn,	42459,		"ui/ingame/lab_defense.tga",			1 },
	}

	foundry_commands = 
	{
		-- Name		Hotkey							TT Text		Texture							TypeID
		{ 40940,	HK_Foundry_ElecToCoal100,		42467,		"ui/ingame/electocoal100.tga",	RESOURCECONV_ElecToCoal100 },
		{ 40941,	HK_Foundry_ElecToCoal500,		42468,		"ui/ingame/electocoal500.tga",	RESOURCECONV_ElecToCoal500 },
		{ 40942,	HK_Foundry_CoaltoElec100,		42469,		"ui/ingame/coaltoelec100.tga",	RESOURCECONV_CoalToElec100 },
		{ 40943,	HK_Foundry_CoaltoElec500,		42470,		"ui/ingame/coaltoelec500.tga",	RESOURCECONV_CoalToElec500 },
	}

	togglebuildmenucommands = 
	{ 
		{40834,	HK_Henchman_BuildToggle,	42345 },
		{40825,	HK_Henchman_BuildToggle,	42327 },
	}

	backbuttoncommand = 
	{ 
		{42350,	HK_System_Escape,	42351 },
	}

	playback_commands = 
	{
		{ 40933, HK_Playback_SpeedNormal, 42445, "ui/ingame/speednormal.tga"},
		{ 40934, HK_Playback_SpeedFast,   42446, "ui/ingame/speedfast.tga"  },
	}
	
	unitattributes = 
	{
		{4273, 4274},	-- Rank
		{4261, 4262},	-- Health	
		{4263, 4264},	-- Armor
		{4265, 4266}, 	-- Speed
		{4267, 4268},	-- Sight of Radius
		{4269, 4270},	-- Size
		{4277, 4278},	-- Melee Damage
		{0, 0},			-- Range attack 0
		{0, 0},			-- Range attack 1
	}

	henchman_modalmodes = 
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_Gather },
		{ MM_Cursor,	MC_Guard },
		{ MM_Cursor,	MC_Repair },
		{ MM_None,		MC_None },				-- stop
		{ MM_Cursor,	MC_BuildStructure },	-- build basic
		{ MM_Cursor,	MC_BuildStructure },	-- build advanced
		{ MM_Cursor,	MC_Heal },
		{ MM_Cursor,	MC_Tag },
		{ MM_None,		MC_None },				-- kill
		{ MM_Cursor,	MC_Unload },			-- unload
		{ MM_Cursor,	MC_AttackMove },
		{ MM_Cursor,	MC_Airlift },
		{ MM_Cursor,	MC_UnTag },
	}

	creature_modalmodes = 
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_Guard },
		{ MM_Cursor,	MC_Patrol },
		{ MM_None,		MC_None },			-- stop
		{ MM_Cursor,	MC_AttackGround },
		{ MM_None,		MC_None },			-- kill
		{ MM_Cursor,	MC_AttackMove },
	}

	-- Note: the entries in creature_abilitymodalmodes MUST follow the order of
	--       the SpecialAbilities enumerated type
	creature_abilitymodalmodes = 
	{
		{ MM_None,		MC_None },
		{ MM_Cursor,	MC_SonarPulse },
		{ MM_None,		MC_None },
		{ MM_Cursor,	MC_Jumping },
		{ MM_None,		MC_None },
		{ MM_None,		MC_None },
	}

	-- Note: the entries in creature_specialmodalmodes MUST follow the order of
	--       the modal_special_commands enumerated type
	creature_specialmodalmodes = 
	{
		{ MM_Cursor,	MC_Plague },
		{ MM_Cursor,	MC_ElectricalBurst },
		{ MM_Cursor,	MC_QuillBurst },
		{ MM_Cursor,	MC_StinkBurst },
		{ MM_Cursor,	MC_WebThrow },
		{ MM_Cursor,	MC_Assassinate },
		{ MM_Cursor,	MC_Flash },
		{ MM_Cursor,	MC_Infestation },
		{ MM_Cursor,	MC_Sabotage },
	}

	-- Note: the entries in rex_modalmodes MUST follow the order of
	--       the rex_commands enumerated type
	rex_modalmodes = 
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_Guard },
		{ MM_None,		MC_None },			-- stop
		{ MM_Cursor,	MC_Heal },
		{ MM_Cursor,	MC_GatherAnimal },
		{},	-- placeholder
	}

	-- Note: the entries in lucy_modalmodes MUST follow the order of
	--       the lucy_commands enumerated type
	lucy_modalmodes = 
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_Guard },
		{ MM_None,		MC_None },			-- stop
		{ MM_Cursor,	MC_Repair },
		{ MM_Cursor,	MC_GatherTech },
		{ MM_Cursor,	MC_Sabotage },
		{},	-- placeholder
		{},	-- placeholder
		{},	-- placeholder
		{ MM_Cursor,	MC_Gather },
	}

	teamobj_modalmode = { MM_Cursor, MC_TeamObjective }

	structures = 
	{
		{ ResourceRenew_EC,		HK_Henchman_Build_Lightning, 		42250 },
		{ RemoteChamber_EC,		HK_Henchman_Build_Remote, 			42253 },
		{ SoundBeamTower_EC,	HK_Henchman_Build_SoundBeamTower, 	42259 },
		{ BrambleFence_EC,		HK_Henchman_Build_Bramble, 			42252 },	
		{ Foundry_EC,			HK_Henchman_Build_Foundry, 			42257 },
		{ ElectricGenerator_EC,	HK_Henchman_Build_Electrical,		42251 },
		{ Aviary_EC,			HK_Henchman_Build_Aviary, 			42255 },
		{ WaterChamber_EC,		HK_Henchman_Build_Water, 			42254 },
	}

	advancedstructures = 
	{
		{ AntiAirTower_EC,		HK_Henchman_Build_AntiAirTower, 	42258 },
		{ VetClinic_EC,			HK_Henchman_Build_VetClinic, 		42256 },
		{ GeneticAmplifier_EC,		HK_Henchman_Build_GeneticAmplifier,	42276 },
		{ LandingPad_EC,		HK_Henchman_Build_LandingPad,		42269 },
	}

	--
	-- Research
	--

	labresearch = 
	{
		{RESEARCH_AdvancedStructure,	HK_Research_AdvancedStruct, 42260 },
		{RESEARCH_Rank2,			HK_Research_Rank2, 42264 },
		{RESEARCH_Rank3,			HK_Research_Rank3, 42265 },
		{RESEARCH_Rank4,			HK_Research_Rank4, 42266 },
		{RESEARCH_Rank5,			HK_Research_Rank5, 42267 },
	}

	vetresearch = 
	{
		{RESEARCH_HenchmanBinoculars,			HK_Research_HenchmanBinoculars, 42262 },
		{RESEARCH_HenchmanTag,				HK_Research_HenchmanTag, 42263 },
		{RESEARCH_HenchmanYoke,				HK_Research_HenchmanYoke, 42270 },
		{RESEARCH_HenchmanHeal,				HK_Research_HenchmanHeal, 42268 },
		{RESEARCH_HenchmanMotivationalSpeech,	HK_Research_HenchmanMotSpeech, 42271 },
		{RESEARCH_IncBuildingIntegrity,			HK_Research_IncBuildIntegrity, 42274 },
		{RESEARCH_TowerUpgrade,				HK_Research_TowerUpgrade, 42284 },
		{RESEARCH_StrengthenFences,			HK_Research_StrengthenFences, 42272 },
		{RESEARCH_StrengthenElectricalGrid,		HK_Research_StrengthenElecGrid, 42273 },
		{RESEARCH_LabDefense,				HK_Research_LabDefense, 42458 },
	}

	foundaryresearch = 
	{
	}
	
	--
	-- Add-Ons
	--

	generatoraddons = 
	{
		{ADDON_EGen1,	HK_AddOn_EGen1, 42277 },
		{ADDON_EGen2,	HK_AddOn_EGen2, 42277 },
		{ADDON_EGen3,	HK_AddOn_EGen3, 42277 },
	}

	--
	-- Creature Upgrades
	--

	creatureupgrades = 
	{
		{CREATUREUPGRADE_Defense,		HK_Upgrade_Defense, 42260 },
		{CREATUREUPGRADE_Speed,			HK_Upgrade_Speed, 42260 },
		{CREATUREUPGRADE_MeleeDamage,	HK_Upgrade_MeleeDamage, 42260 },
		{CREATUREUPGRADE_HitPoints,		HK_Upgrade_HitPoints, 42260 },
		{CREATUREUPGRADE_SightRadius,		HK_Upgrade_SightRadius, 42260 },
		{CREATUREUPGRADE_RangedDamage,	HK_Upgrade_RangedDamage, 42260 },
		{CREATUREUPGRADE_SplashDamage,	HK_Upgrade_SplashDamage, 42260 },
		{CREATUREUPGRADE_AreaAttackRadius,	HK_Upgrade_AreaAttackRadius, 42260 },
	}

	rex_abilities = 
	{
		{ CHARABILITYTYPE_RexPoisonTouch, 	38400, 38401 },
		{ CHARABILITYTYPE_RexRegeneration, 	38402, 38403 },
		{ CHARABILITYTYPE_RexKeenSense, 	38404, 38405 },
		{ CHARABILITYTYPE_RexNeuroToxin, 	38406, 38407 },
		{ CHARABILITYTYPE_RexPack, 		38408, 38409 },
	}

	singleselectinfotable = 
	{
		{38410, 38411},		-- healthbar/healthlabel
		{38412, 38413},		-- entity icon
		{38414, 38415},		-- entity damage
		{38416, 38417},		-- entity endurance
		{38418, 38419},		-- entity name
	}

	-- Record the taskbar menu context so that we can refresh the menu without losing
	-- the context.  The menu is made up of 3 parts: the Lua expression to call to
	-- recover the context, and the Lua function to call to ensure that at least one
	-- of the selected entities qualifies for this context, and the command to end
	-- any modal UI state.  The 1st part is the context refresher
	-- and the 2nd part is the context qualifier.
	-- 
	menucontext = { "", "", "" }

	-- for the building menu we store a global that will either point to the 'structures' or 'advancedstructures' table
	currentstructures = structures

	-- creature to upgrade (for the genetic amplifier)
	creature_to_upgrade = -1

	-- menu context for special modal commands
	special_modal_index = -1
	special_modal_type  = -1

--
cleartaskbar = function()

	Clear()

	-- background bitmaps
	ShowHud( "picturelabel" )

	BindHotkey( HK_System_Escape, "deselectall", 0 )
	BindHotkey( HK_System_Focus, "focusonselection", 0 )
	BindHotkey( HK_System_ZoomCameraIn, "zoomin", 0 )
	BindHotkey( HK_System_ZoomCameraOut, "zoomout", 0 )
	BindHotkey( HK_Unassign_FromAllGroups, "unhotkeygroup", 0 )

	BindLabelToGameTime( "gametime_label" )

	if not( LocalPlayer() == 0 ) then

		BindHotkeyToEvent( HK_Generic_Event )	

	end

	-- quick keyboard selection
	if not( LocalPlayer() == 0 ) then

		BindHotkey       ( HK_Select_UnitsOnScreen, "selectallunitsonscreen", 0 )
		BindHotkey       ( HK_Select_HenchmenOnScreen, "selectallhenchmenonscreen", 0 )
		BindHotkey       ( HK_Select_HenchmenInWorld, "selectallhenchmeninworld", 0 )
		BindHotkey       ( HK_Select_HenchmenNextIdle, "selectnextidlehenchman", 0 )
		BindHotkey       ( HK_Select_HenchmenAllIdle, "selectallidlehenchman", 0 )
		BindHotkey       ( HK_Select_CombatUnitsOnScreen, "selectallcombatunitsonscreen", 0 )
		BindHotkey       ( HK_Select_CombatUnitsInWorld, "selectallcombatunitsinworld", 0 )
		BindHotkey       ( HK_Select_GroundCombatUnitsOnScreen, "selectallgroundcombatunitsonscreen", 0 )
		BindHotkey       ( HK_Select_GroundCombatUnitsInWorld, "selectallgroundcombatunitsinworld", 0 )
		BindHotkey       ( HK_Select_AirCombatUnitsOnScreen, "selectallaircombatunitsonscreen", 0 )
		BindHotkey       ( HK_Select_AirCombatUnitsInWorld, "selectallaircombatunitsinworld", 0 )
		BindHotkey       ( HK_Select_WaterCombatUnitsOnScreen, "selectallwatercombatunitsonscreen", 0 )
		BindHotkey       ( HK_Select_WaterCombatUnitsInWorld, "selectallwatercombatunitsinworld", 0 )
		BindHotkey       ( HK_Select_NextGroundCombiner, "selectnextgroundcombiner", 0 )
		BindHotkey       ( HK_Select_NextWaterCombiner, "selectnextwatercombiner", 0 )
		BindHotkey       ( HK_Select_NextAirCombiner, "selectnextaircombiner", 0 )
		BindHotkey       ( HK_Select_Lab, "selectlab", 0 )
		BindHotkey       ( HK_Select_Rex, "selectrex", 0 )
		BindHotkey       ( HK_Select_Lucy, "selectlucy", 0 )
		BindHotkey       ( HK_Select_NextSubSelection, "nextsubselect", 0 )

	end


	-- player resources
	if not( LocalPlayer() == 0 ) then

		ShowHud( "background1" )
	
		BindLabelToPlayerRenew	( "resource_electricity_label",	"resourceindicatorstooltip", 2,	LocalPlayer() )
		BindHudToTooltip		( "resource_electricity_icon",	"resourceindicatorstooltip", 1, 2 )

		BindLabelToPlayerGather	( "resource_scrap_label",		"resourceindicatorstooltip", 1,	LocalPlayer() )
		BindHudToTooltip		( "resource_scrap_icon",		"resourceindicatorstooltip", 1, 1 )
		
		BindLabelToPlayerPop	( "resource_unitcap_label",		"resourceindicatorstooltip", 3,	LocalPlayer() )
		BindHudToTooltip		( "resource_unitcap_icon",		"resourceindicatorstooltip", 1, 3 )

	end

	-- button
	if 1 == 1 then
		BindButton( "menu_button", menu_commands[1][2], "escapemenu",  "menutooltip", "", 1 )
	end

	if ArmyBuilderAllowed() == 1 then
		BindButtonToArmyBuilder( "buildarmy_button",  menu_commands[2][2], "armybuilder", "menutooltip", "", 2 )
	end

	if SpeechRecallAllowed() == 1 then
		BindButton( "speechrecall_button", menu_commands[7][2], "speechrecall", "menutooltip", "", 7 )
	end

	if AllyAllowed() == 1 then
		BindButton( "ally_button", menu_commands[4][2], "allymenu", "menutooltip", "", 4 )
	end

	if ObjectivesAllowed() == 1 then
		BindButtonToObjectives( "objectives_button", menu_commands[5][2], "objvmenu", "menutooltip", "", 5 )
	end

	if TeamObjectiveAllowed() == 1 then
		BindHotkey( HK_Generic_TeamObjective, "doteamobjmodal", 0 )
	end

	local chat = ChatAllowed()

	if (chat == CHATALLOW_Ok) or (chat == CHATALLOW_Dead) or (chat == CHATALLOW_COPPA) then
		BindButtonToChat( "chat_button", menu_commands[3][2], "chat", "chattooltip" )
	end

	-- dummy button (2)
	BindButton( "group_button", "", "dummy", "", "", 0 )
	BindButtonToGroup( "hotkey_1",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 0 )
	BindButtonToGroup( "hotkey_2",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 1 )
	BindButtonToGroup( "hotkey_3",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 2 )
	BindButtonToGroup( "hotkey_4",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 3 )
	BindButtonToGroup( "hotkey_5",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 4 )
	BindButtonToGroup( "hotkey_6",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 5 )
	BindButtonToGroup( "hotkey_7",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 6 )
	BindButtonToGroup( "hotkey_8",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 7 )
	BindButtonToGroup( "hotkey_9",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 8 )
	BindButtonToGroup( "hotkey_0",	"selecthotkeygroup_callback", "assignhotkeygroup_callback", "selecthotkeygroup_tooltipcb", 9 )

	-- minimap
	BindLabelToTooltip( "minimaptooltip", "minimap_tooltipcb" )

	-- infocenter
	infocenter()

	-- playback buttons
	if FastSpeedAllowed() == 1 then
	
		if IsFastSpeed() == 1 then
		
			-- Show play button (normal speed)
			BindButton( "command_modal_icon01", playback_commands[1][2], "dospeedfast", "playbacktooltip", playback_commands[1][4], 1 )
		
		else
		
			-- Show fast forward button (fast speed)
			BindButton( "command_modal_icon01", playback_commands[2][2], "dospeednormal",   "playbacktooltip", playback_commands[2][4], 2 )
		
		end
	end
end

--
armybuilder = function( dummy )

	ArmyBuilderShow()

end

--
chat = function( dummy )

	ChatShow()

end

-- playback at normal speed
dospeednormal = function( dummy )

	SetFastSpeed( 1 )
	
	cleartaskbar()

end

-- playback at fast speed
dospeedfast = function( dummy )

	SetFastSpeed( 0 )
	
	cleartaskbar()
	
end

-- 
chattooltip = function( enabled, index )

	HelpTextTitle( 40952 )
	HelpTextChat ()

end

-- 
tooltip_command = function( enabled, index, table )

	if index >= 1 and index <= getn( table ) then

	HelpTextTitle		( table[ index ][1] )
	HelpTextShortcut	( table[ index ][2] )
	HelpTextTextWithoutRequirements( table[index][3] )

	end

end

--
speechrecall = function( dummy )

	SpeechRecallShow()

end

-- 
resourceindicatorstooltip = function( enabled, index )

	if index >= 1 and index <= getn( resource_indicator_tooltips ) then

		HelpTextTitle( resource_indicator_tooltips[ index ][1] )
		HelpTextTextWithoutRequirements( resource_indicator_tooltips[index][3] )

	end

end
 
-- 
menutooltip = function( enabled, index )

	tooltip_command( enabled, index, menu_commands )

end

--
playbacktooltip = function( enabled, index )

	tooltip_command( enabled, index, playback_commands )

end


--
infocentersinglebasicstats = function( id )

	local ebpid = EntityEBP( id )
	local owner = EntityOwner( id )

	-- basic stats
	BindLabelToEntityName  ( "singlselect_name_label",    id, "singleselectinfotooltip", 5 )

	if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_HitPoints ) == 0 then	
		BindLabelToEntityHealth( "singlselect_statbar_label", id, "singleselectinfotooltip", 1 )
	else
		BindLabelToEntityHealth( "singlselect_statbar_upgrade", id, "singleselectinfotooltip", 1 )
	end

	BindImageToEntityIcon  ( "singlselect_icon", id, "singleselectinfotooltip", 2 )

	BindBarToEntityHealth  ( "singlselect_statbar", id, "singleselectinfotooltip", 1 )

	BindLabelToEntityDamage( "textlabel_status", id, "singleselectinfotooltip", 3 )

	if EntityHasEndurance( id ) == 1 then

		BindBarToEntityEndurance( "singlselect_statbar0", id, "singleselectinfotooltip", 4  )

	end

	-- owner
	if EntityBelongsToPlayer( id ) == 0 then

		-- owner
		BindLabelToPlayerName  ( "textlabel_playerinfo1", EntityOwner( id ) )
		BindLabelToPlayerColour( "color_label",           EntityOwner( id ) )
		
		-- ally/enemy
		if not( LocalPlayer() == 0) then
			if SelectionIsEnemy() == 1 then
				BindLabelToText( "textlabel_infoline02", 40971 )

			elseif SelectionIsAlly() == 1 then
				BindLabelToText( "textlabel_infoline02", 40970 )

			end
		end	

	end

end

--
infocenterresearch = function( id, enable )

	-- research stuff
	BindLabelToResearchProgress ( "buildque_progress_label", id )
	BindLabelToInResearch		( "buildque_name_label", id )
	BindButtonToInResearch		( "buildque_icon00", "", "docancelresearch", "", id, enable )
	BindBarToResearch			( "buildque_progress_statbar", id )

end

--
infocentercreatureupgrade = function( id, enabled )

	-- creature upgrade stuff
	BindLabelToCreatureUpgradeProgress ( "buildque_progress_label", id )
	BindLabelToInCreatureUpgrade	( "buildque_name_label", id )
	BindButtonToInCreatureUpgrade	( "buildque_icon00", "", "docancelcreatureupgrade", "", id, enabled )
	BindBarToCreatureUpgrade		( "buildque_progress_statbar", id )

end

--
infocenterconstruction = function( id, enabled )

	-- construction
	BindLabelToConstructionProgress( "buildque_progress_label", id )
	BindButtonToInConstruction     ( "buildque_icon00", "", "docancelconstruction", "", id, enabled )
	BindBarToConstruction          ( "buildque_progress_statbar", id )

end

--
infocenteraddon = function( id, enabled )

	-- add-on
	BindLabelToInAddOn ( "buildque_name_label", id )
	BindButtonToInAddOn( "buildque_icon00", "", "docanceladdon", "", id, enabled )
	BindBarToAddOn     ( "buildque_progress_statbar", id )
	BindLabelToAddOn   ( "buildque_progress_label", id )

end

--
infocenterbuildqueue = function( id, enabled )

	--
	local bqbuttons =
	{
		"buildque_icon00",
		"buildque_icon01",
		"buildque_icon02",
		"buildque_icon03",
		"buildque_icon04",
		"buildque_icon05",
		"buildque_icon06",
		"buildque_icon07"
	}

	local n = getn( bqbuttons )
	local bqlength = BuildQueueLength( id )

	local count = 0

	if( n < bqlength ) then
		count = n
	else
		count = bqlength
	end

	for i = 0, count - 1
	do

		BindButtonToBuildQueue( bqbuttons[ i + 1 ], "", "docancelbuildunit", "", id, i, enabled )

	end

	BindLabelToBuildQueue   ( "buildque_name_label", id, 0 )
	BindLabelToBuildProgress( "buildque_progress_label", id )
	BindBarToBuildQueue     ( "buildque_progress_statbar", id )

end

--
unitattrtooltip = function( index )

	if index >= 1 and index <= getn( unitattributes ) then

		local titleid	= unitattributes[ index ][1]
		local descid	= unitattributes[ index ][2] 
		
		HelpTextTitle( titleid )
		HelpTextTextWithoutRequirements( descid )

	end
	
end

speedtooltip = function( )
	unitattrtooltip(UATTR_Speed+1)
end

--
unitabilitytooltip = function( ebpid, ability )
	HelpTextCreatureAbility( ebpid, ability )
end

--
unitrangeattacktooltip = function( ebpid, rangeattack )
	HelpTextCreatureRangeAttack( ebpid, rangeattack )
end

--
piercetooltip = function( ebpid, rangeattack )
	HelpTextTitle(39539)
	HelpTextTextWithoutRequirements(39540)
end

--
rexabilitytooltip = function( ability, dummy )

	local abilities = rex_abilities

	for i = 1, getn( abilities )
	do
		if abilities[ i ][ 1 ] == ability then

			HelpTextRexAbility( abilities[i][2], abilities[i][3] )

		end

	end

end


--
infocentercreature = function( id )

	local ebpid = EntityEBP( id )
	local owner = EntityOwner( id )

	-- 
	ShowHud( "attribute_backlabel" )

	-- rank
	BindIconToCreatureAttribute( "rank_icon",				"unitattrtooltip", ebpid, UATTR_Rank    )

	-- attributes icons
	BindIconToCreatureAttribute( "armor_staticon",			"unitattrtooltip", ebpid, UATTR_Armor	 )
	BindIconToCreatureAttribute( "speed_staticon",			"unitattrtooltip", ebpid, UATTR_Speed	 )
	BindIconToCreatureAttribute( "sightradius_staticon",	"unitattrtooltip", ebpid, UATTR_Sight   )
	BindIconToCreatureAttribute( "meleedamage_staticon",	"unitattrtooltip", ebpid, UATTR_Melee   )

	-- attribute labels

		-- defense / armour
	if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_Defense ) == 0 then
		BindLabelToCreatureAttribute( "armor_number", "unitattrtooltip", owner, ebpid, UATTR_Armor	 )
	else
		BindLabelToCreatureAttribute( "armor_upgrade", "unitattrtooltip", owner, ebpid, UATTR_Armor	 )
	end

		-- sight radius
	if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_SightRadius ) == 0 then
		BindLabelToCreatureAttribute( "sightradius_number", "unitattrtooltip", owner, ebpid, UATTR_Sight   )
	else
		BindLabelToCreatureAttribute( "sightradius_upgrade", "unitattrtooltip", owner, ebpid, UATTR_Sight   )
	end

		-- melee damage
	if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_MeleeDamage ) == 0 then
		BindLabelToCreatureAttribute( "meleedamage_number", "unitattrtooltip", owner, ebpid, UATTR_Melee   )
	else
		BindLabelToCreatureAttribute( "meleedamage_upgrade", "unitattrtooltip", owner, ebpid, UATTR_Melee   )
	end

		-- speed labels
	if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_Speed ) == 0 then
		BindLabelsToCreatureSpeed	( 
								"speedtooltip",
								"speedtooltip",
								"speedtooltip",
								"speedtooltip",
								"landspeed_number",
								"waterspeed_number",
								"airspeed_number",
								"purewaterspeed_number",
								owner,
								ebpid
								);
	else
		BindLabelsToCreatureSpeed	( 
								"speedtooltip",
								"speedtooltip",
								"speedtooltip",
								"speedtooltip",
								"landspeed_upgrade",
								"waterspeed_upgrade",
								"airspeed_upgrade",
								"purewaterspeed_upgrade",
								owner,
								ebpid
								);
	end		

	-- range attacks
	local rangeAttackCount = GetRangeAttackCount(ebpid)
	if rangeAttackCount == 1 then
		ShowHud("attribute_backlabel1")
		if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_RangedDamage ) == 0 then
			BindLabelToCreatureRangeAttack( "range1damage_staticon", "range1damage_number", "range1damage_pierce", "unitrangeattacktooltip" , "piercetooltip", owner, ebpid, 0 )	
		else
			BindLabelToCreatureRangeAttack( "range1damage_staticon", "range1damage_upgrade", "range1damage_pierce", "unitrangeattacktooltip" , "piercetooltip", owner, ebpid, 0 )	
		end
	elseif rangeAttackCount == 2 then
		ShowHud("attribute_backlabel1")
		ShowHud("attribute_backlabel2")

		if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_RangedDamage ) == 0 then
			BindLabelToCreatureRangeAttack( "range2damage_staticon", "range2damage_number", "range2damage_pierce", "unitrangeattacktooltip" , "piercetooltip", owner, ebpid, 0 )
		else
			BindLabelToCreatureRangeAttack( "range2damage_staticon", "range2damage_upgrade", "range2damage_pierce", "unitrangeattacktooltip" , "piercetooltip", owner, ebpid, 0 )
		end

		if CreatureHasUpgrade( owner, ebpid, CREATUREUPGRADE_RangedDamage ) == 0 then
			BindLabelToCreatureRangeAttack( "range1damage_staticon", "range1damage_number", "range1damage_pierce", "unitrangeattacktooltip" , "piercetooltip", owner, ebpid, 1 )
		else
			BindLabelToCreatureRangeAttack( "range1damage_staticon", "range1damage_upgrade", "range1damage_pierce", "unitrangeattacktooltip" , "piercetooltip",  owner, ebpid, 1 )
		end
	end

	-- abilities
	if IsAbilityUpgraded( owner, ebpid, 0 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline03",	"unitabilitytooltip", ebpid, 0, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline03",	"unitabilitytooltip", ebpid, 0, 0 )
	end

	if IsAbilityUpgraded( owner, ebpid, 1 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline04",	"unitabilitytooltip", ebpid, 1, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline04",	"unitabilitytooltip", ebpid, 1, 0 )
	end

	if IsAbilityUpgraded( owner, ebpid, 2 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline05",	"unitabilitytooltip", ebpid, 2, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline05",	"unitabilitytooltip", ebpid, 2, 0 )
	end

	if IsAbilityUpgraded( owner, ebpid, 3 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline06",	"unitabilitytooltip", ebpid, 3, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline06",	"unitabilitytooltip", ebpid, 3, 0 )
	end

	if IsAbilityUpgraded( owner, ebpid, 4 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline07",	"unitabilitytooltip", ebpid, 4, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline07",	"unitabilitytooltip", ebpid, 4, 0 )
	end

	if IsAbilityUpgraded( owner, ebpid, 5 ) == 1 then
		BindLabelToCreatureAbility	( "textlabel_upgradeline08",	"unitabilitytooltip", ebpid, 5, 1 )
	else
		BindLabelToCreatureAbility	( "textlabel_infoline08",	"unitabilitytooltip", ebpid, 5, 0 )
	end

end

--
infocenterhenchman = function( id )

	BindLabelToHenchmanState( "henchmenaction_icon", id )

	-- tag reload
	if EntityHasTagReload( id ) == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40780 )
		BindBarToEntityTagReload( "progress_statbar01", id )

	end

end

--
infocentergyrocopter = function( id )

	BindLabelToHenchmanState( "henchmenaction_icon", id )

	-- tag reload
	if EntityHasTagReload( id ) == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40780 )
		BindBarToEntityTagReload( "progress_statbar01", id )

	end

end


--
infocenterrex = function( id )

	-- tag reload
	if EntityHasTagReload( id ) == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40782 )
		BindBarToEntityTagReload( "progress_statbar01", id )

	end

	local abilities = rex_abilities

	local labels = 
	{
		"textlabel_infoline03",
		"textlabel_infoline04",
		"textlabel_infoline05",
		"textlabel_infoline06",
		"textlabel_infoline07",
		"textlabel_infoline08",
	}

	local count = getn( abilities )
	local labelIndex = 1

	for i = 1, count
	do
		if HasCharacterAbility( abilities[ i ][ 1 ] ) == 1 then

			BindLabelToRexAbility( labels[ labelIndex ], "rexabilitytooltip", abilities[ i ][ 1 ], abilities[ i ][ 2 ] )
			labelIndex = labelIndex + 1
		end
	end

end

--
infocentersoundbeamtower = function( id )

	infocentertower( id )

	-- sonic boom
	if SonicBoomIsOpen() == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40784 )
		BindBarToSonicBoomRecharge( "progress_statbar01", id )

	end

end

--
infocenterantiairtower = function( id )

	infocentertower( id )

	-- air burst
	if AirBurstIsOpen() == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40785 )
		BindBarToAirBurstRecharge( "progress_statbar01", id )

	end

end

--
infocentertower = function( id )

	BindLabelToTowerRange  ( "textlabel_infoline04", id )
	BindLabelToTowerDamage ( "textlabel_infoline05", id )

end

--
infocenterlab = function( id )

	-- radar pulse
	if RadarPulseIsOpen() == 1 then

		BindLabelToText		    ( "textlabel_infoline01", 40781 )
		BindBarToRadarPulseRecharge( "progress_statbar01", id )

	end

	-- lab defense
	if IsLabDefenseEnabled( id ) == 1 then

		BindLabelToText		    ( "textlabel_infoline03", 40786 )
		BindBarToLabDefense( "progress_statbar02", id )
	
	end
end

--
infocenterlucy = function( id )

	-- tag reload
	if EntityHasTagReload( id ) == 1 then

		BindLabelToText		( "textlabel_infoline01", 40783 )
		BindBarToEntityTagReload( "progress_statbar01", id )

	end

end


--
infocenterenemy = function()

	if SelectionCount() == 1 then

		local id = SelectionId( 0 )
		
		local type = EntityType( id ) 

		if	type == Creature_EC or
			type == Animal_EC or
			type == Henchman_EC or
			type == Rex_EC or
			type == Lucy_EC or
			type == Gyrocopter_EC or
			type == Lab_EC or
			type == RemoteChamber_EC or
			type == WaterChamber_EC or
			type == Aviary_EC or
			type == ElectricGenerator_EC or
			type == ResourceRenew_EC or
			type == VetClinic_EC or
			type == Foundry_EC or
			type == AntiAirTower_EC or
			type == SoundBeamTower_EC or
			type == BrambleFence_EC or
			type == GenStruct_EC or
			type == GeneticAmplifier_EC or
			type == LandingPad_EC or
			type == Fire_EC
		then
			
			infocentersinglebasicstats( id )

			if type == Creature_EC then

				infocentercreature( id )
			
			end

		end

	end

end

--
infocenterfriendly = function()

	if SelectionCount() == 1 then

		-- single
		infocentersingle( SelectionId( 0 ) )

	else

		-- multi
		infocentermulti()

	end

end

--
infocenterworld = function()

	if SelectionCount() == 1 then

		local id = SelectionId( 0 )

		local type = EntityType( id )

		--			
		if type == ResourceGather_EC then

			-- basic stats
			BindLabelToEntityName( "singlselect_name_label", id, "singleselectinfotooltip", 5 )
			BindImageToEntityIcon( "singlselect_icon",       id, "singleselectinfotooltip", 2 )

			BindLabelToText		 ( "textlabel_infoline01", 40766 )
			BindLabelToResource  ( "textlabel_infoline02", id )

		elseif type == Animal_EC then

			BindLabelToEntityName       ( "singlselect_name_label", id, "singleselectinfotooltip", 5 )
			BindImageToEntityIcon       ( "singlselect_icon",       id, "singleselectinfotooltip", 2 )
			BindLabelToAnimalGatherState( "textlabel_infoline02",   id )

		elseif type == 0 then

			BindLabelToEntityName( "singlselect_name_label", id, "singleselectinfotooltip", 5 )
			BindImageToEntityIcon( "singlselect_icon",       id, "singleselectinfotooltip", 2 )

		elseif type == GenStruct_EC then

			BindLabelToEntityName       ( "singlselect_name_label", id, "singleselectinfotooltip", 5 )
			BindImageToEntityIcon       ( "singlselect_icon",       id, "singleselectinfotooltip", 2 )

		elseif type == Geyser_EC then

			BindLabelToEntityName       ( "singlselect_name_label", id, "singleselectinfotooltip", 5 )
			BindImageToEntityIcon       ( "singlselect_icon",       id, "singleselectinfotooltip", 2 )

			BindLabelToText				( "textlabel_infoline01", 40767 )

		else

			if	type == Creature_EC or
				type == Henchman_EC or
				type == Gyrocopter_EC or
				type == Lab_EC or
				type == RemoteChamber_EC or
				type == WaterChamber_EC or
				type == Aviary_EC or
				type == ElectricGenerator_EC or
				type == ResourceRenew_EC or
				type == VetClinic_EC or
				type == Foundry_EC or
				type == AntiAirTower_EC or
				type == SoundBeamTower_EC or
				type == BrambleFence_EC or
				type == GeneticAmplifier_EC or
				type == Rex_EC or
				type == Fire_EC or
				type == LandingPad_EC or
				type == Villager_EC
				then

				-- basic stats
				infocentersinglebasicstats( id )

			end

		end

	end -- count == 1

end

--
infocenter = function()

	if SelectionCount() == 0 then
	
		-- display nothing
		
	else
	
		if LocalPlayer() == 0 then

			if SelectionCount() == 1 then

				-- single
				infocentersingleobserver( SelectionId( 0 ) )

			end

		elseif SelectionBelongsToPlayer() == 1 then

			infocenterfriendly()

		elseif SelectionIsEnemy() == 1 or SelectionIsAlly() == 1 then

			infocenterenemy()
			
		else

			infocenterworld()

		end

	end

end

--
infocentersingle = function( id )

	-- basic stats
	infocentersinglebasicstats( id )

	-- special states
	if EntityType( id ) == Creature_EC then
		
		infocentercreature( id )

	elseif EntityType( id ) == Henchman_EC then
		
		infocenterhenchman( id )

	elseif EntityType( id ) == Gyrocopter_EC then
		
		infocentergyrocopter( id )

	elseif EntityType( id ) == Rex_EC then
		
		infocenterrex( id )

	elseif EntityType( id ) == Lucy_EC then
		
		infocenterlucy( id )

	elseif EntityInResearch( id ) == 1 then

		infocenterresearch( id, 1 )

	elseif EntityInConstruction( id ) == 1 then

		infocenterconstruction( id, 1 )

	elseif EntityInExtension( id ) == 1 then

		infocenteraddon( id, 1 )

	elseif EntityInSpawning( id ) == 1 then

		infocenterbuildqueue( id, 1 )

	elseif EntityType( id ) == Lab_EC then

		infocenterlab( id )

	elseif EntityType( id ) == SoundBeamTower_EC then
	
		infocentersoundbeamtower( id )
	
	elseif EntityType( id ) == AntiAirTower_EC then
	
		infocenterantiairtower( id )
		
	elseif EntityInCreatureUpgrade( id ) == 1 then

		infocentercreatureupgrade( id, 1 )

	end

end

--
infocentersingleobserver = function( id )

	-- basic stats
	infocentersinglebasicstats( id )

	-- special states
	if EntityType( id ) == Creature_EC then
		
		infocentercreature( id )

	elseif EntityType( id ) == Henchman_EC then
		
		infocenterhenchman( id )

	elseif EntityType( id ) == Gyrocopter_EC then
		
		infocentergyrocopter( id )

	elseif EntityType( id ) == Rex_EC then
		
		infocenterrex( id )

	elseif EntityType( id ) == Lucy_EC then
		
		infocenterlucy( id )

	elseif EntityInResearch( id ) == 1 then

		infocenterresearch( id, 0 )

	elseif EntityInConstruction( id ) == 1 then

		infocenterconstruction( id, 0 )

	elseif EntityInExtension( id ) == 1 then

		infocenteraddon( id, 0 )

	elseif EntityInSpawning( id ) == 1 then

		infocenterbuildqueue( id, 0 )

	elseif EntityType( id ) == Lab_EC then

		infocenterlab( id )

	elseif EntityInCreatureUpgrade( id ) == 1 then

		infocentercreatureupgrade( id, 0 )

	else
		
	end

end

--
infocentermulti = function()

	-- info center
	local multibuttons =
	{
		{ "multiselect_icon01", "multiselect_statbar01" },
		{ "multiselect_icon02",	"multiselect_statbar02" },
		{ "multiselect_icon03",	"multiselect_statbar03" },
		{ "multiselect_icon04",	"multiselect_statbar04" },
		{ "multiselect_icon05",	"multiselect_statbar05" },
		{ "multiselect_icon06",	"multiselect_statbar06" },
		{ "multiselect_icon07",	"multiselect_statbar07" },
		{ "multiselect_icon08",	"multiselect_statbar08" },
		{ "multiselect_icon09",	"multiselect_statbar09" },
		{ "multiselect_icon10",	"multiselect_statbar10" },
		{ "multiselect_icon11",	"multiselect_statbar11" },
		{ "multiselect_icon12",	"multiselect_statbar12" },
		{ "multiselect_icon13",	"multiselect_statbar13" },
		{ "multiselect_icon14",	"multiselect_statbar14" },
		{ "multiselect_icon15",	"multiselect_statbar15" },
		{ "multiselect_icon16",	"multiselect_statbar16" },
		{ "multiselect_icon17",	"multiselect_statbar17" },
		{ "multiselect_icon18",	"multiselect_statbar18" },
		{ "multiselect_icon19",	"multiselect_statbar19" },
		{ "multiselect_icon20",	"multiselect_statbar20" },
		{ "multiselect_icon21",	"multiselect_statbar21" },
		{ "multiselect_icon22",	"multiselect_statbar22" },
		{ "multiselect_icon23",	"multiselect_statbar23" },
		{ "multiselect_icon24",	"multiselect_statbar24" },
		{ "multiselect_icon25",	"multiselect_statbar25" },
		{ "multiselect_icon26",	"multiselect_statbar26" },
		{ "multiselect_icon27", "multiselect_statbar27" },
		{ "multiselect_icon28", "multiselect_statbar28" },
		{ "multiselect_icon29", "multiselect_statbar29" },
		{ "multiselect_icon30", "multiselect_statbar30" },
		{ "multiselect_icon31", "multiselect_statbar31" },
		{ "multiselect_icon32", "multiselect_statbar32" },
		{ "multiselect_icon33", "multiselect_statbar33" },
		{ "multiselect_icon34", "multiselect_statbar34" },
		{ "multiselect_icon35", "multiselect_statbar35" },
		{ "multiselect_icon36", "multiselect_statbar36" },
	}

	local count = SelectionCount()

	if  count > getn( multibuttons ) then
		
		count = getn( multibuttons )

	end

	for i = 0, count - 1
	do

		local id = SelectionId( i )
		
		BindButtonToEntity   ( multibuttons[ i + 1 ][ 1 ], "", "selectentity", "", id )
		BindBarToEntityHealth( multibuttons[ i + 1 ][ 2 ], id, "", 0 );

	end

end

--
dummy = function( tmp )
end

--
selecthotkeygroup_callback = function( groupNb )
	SelectHotkeyGroup( groupNb )
end

selecthotkeygroup_tooltipcb = function( enabled, groupNb )
	HelpTextTitle(39535)
	HelpTextTextWithoutRequirements(39536)
	HelpTextShortcut( hotkeygroups[groupNb+1] )

end

minimap_tooltipcb = function( enabled, index )
	HelpTextTitle(39537)
	HelpTextTextWithoutRequirements(39538)
end

--
assignhotkeygroup_callback = function( groupNb )
	AssignHotkeyGroup( groupNb )
end

--
commandstooltip = function( enabled, index )

	tooltip_command( enabled, index, commands )

end

--
foundrytooltip = function( enabled, index )

	tooltip_command( enabled, index, foundry_commands )

end

--
sonicboomtooltip = function( enabled, ebpid )

	if enabled == 0 then
		HelpTextSonicBoomPrerequisite()
	end

	HelpTextTitle( soundbeamcommand[1] )
	HelpTextSonicBoomCost(ebpid)
	HelpTextTextWithRequirements( soundbeamcommand[3] )
	HelpTextShortcut( soundbeamcommand[2] )

end

--
airbursttooltip = function( enabled, ebpid )

	if enabled == 0 then
		HelpTextAirBurstPrerequisite()
	end

	HelpTextTitle( antiaircommand[1] )
	HelpTextAirBurstCost(ebpid)
	HelpTextTextWithRequirements( antiaircommand[3] )
	HelpTextShortcut( antiaircommand[2] )

end

--
tooltip_radarpulse = function( enabled, ebpid )

	if enabled == 0 then
		HelpTextRadarPulsePrerequisite()
	end

	HelpTextTitle( radarpulsecommand[1] )
	HelpTextRadarPulseCost(ebpid)
	HelpTextTextWithRequirements( radarpulsecommand[3] )
	HelpTextShortcut( radarpulsecommand[2] )

end

--
radarpulsetooltip = function( enabled, ebpid )

	tooltip_radarpulse( enabled, ebpid )

end

--
dolabdefense_turnoff_tooltip = function( enabled, index, ebpid )

-- do appopriate tool tip info

	HelpTextTitle( labcommands[1][1] )
	HelpTextTextWithRequirements( labcommands[1][3] )
	HelpTextShortcut( labcommands[1][2] )

end

dolabdefense_turnon_tooltip = function( enabled, index, ebpid )

-- do appopriate tool tip info	

	HelpTextTitle( labcommands[2][1] )
	
	if enabled == 0 then
		HelpTextLabDefensePrerequisite()
	end
	
	HelpTextLabDefenseCost( ebpid )
	HelpTextTextWithRequirements( labcommands[2][3] )
	HelpTextShortcut( labcommands[2][2] )

end

--
soundbeamtowerselection = function()

	local id = SelectionId( 0 )
	
	--
	cleartaskbar()
	
	-- background
	ShowBitmapLabel( "command_bigicon_back" )
	
	-- sonicboom on sound team tower
	if SonicBoomIsAllowed( id ) == 1 then
		BindButtonToSonicBoom( "command_big_icon01", soundbeamcommand[2], "dosonicboom", "sonicboomtooltip", soundbeamcommand[4], id )
	end
	
	-- kill building button
	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )
	
end

--
antiairtowerselection = function()

	local id = SelectionId( 0 )
	
	--
	cleartaskbar()
	
	-- background
	ShowBitmapLabel( "command_bigicon_back" )
	
	-- airburst on anti air tower
	if AirBurstIsAllowed( id ) == 1 then
		BindButtonToAirBurst( "command_big_icon01", antiaircommand[2], "doairburst", "airbursttooltip", antiaircommand[4], id )
	end

	-- kill building button
	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )
	
end

--
labselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- rally
	BindButton( "command_modal_icon01", commands[1][2], "dospawnmodal", "commandstooltip", commands[1][4], 1 )

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

		-- henchman
	BindButtonToUnitEBP( "command_buildhenchman", HK_Lab_CreateHenchman, "dobuildunit", "tooltip_henchmanbutton", id, GathererEBP() )

	-- ungarrison Rex
	if EntityTypeGarrisoned( id, Rex_EC ) == 1 then

		BindButton( "command_big_icon10", commands[2][2], "doungarrison", "commandstooltip", commands[2][4], 2 )

	end

	-- ungarrison Lucy
	if EntityTypeGarrisoned( id, Lucy_EC ) == 1 then

		BindButton( "command_big_icon09", commands[3][2], "doungarrison", "commandstooltip", commands[3][4], 3 )

	end

		-- research
	if EntityInSpawning( id ) == 0 then

		commandarearesearch( id, labresearch, "tooltip_labresearch" )

	end

	-- radar pulse
	if EntityInSpawning( id ) == 0 and RadarPulseIsAllowed( id ) == 1 then

		BindButtonToRadarPulse( "command_big_icon08", radarpulsecommand[2], "doradarpulse", "radarpulsetooltip", radarpulsecommand[4], id )

	end
	
	-- lab defense
	if IsLabDefenseOn( id ) == 1 then
		-- Show turn off button (current state = on)
		BindButtonToLabDefense( "command_big_icon09", labcommands[1][2], "dolabdefense", "dolabdefense_turnoff_tooltip", labcommands[1][4], 0, id )
	else
		-- Show turn on button (current state = off)
		BindButtonToLabDefense( "command_big_icon09", labcommands[2][2], "dolabdefense", "dolabdefense_turnon_tooltip", labcommands[2][4], 1, id )
	end

end


--
tooltip_gyrocopterunit = function( enabled, ebpid )

	tooltip_ebp(enabled,ebpid)
	HelpTextShortcut(HK_LandingPad_CreateGyrocopter)
	HelpTextTextWithRequirements(42391)

	if HasBuildLimit(ebpid) and (enabled == 1 or IsAtBuildLimit(ebpid) == 1) then

		HelpTextBuildLimit( ebpid )	
	
	end

end


landingpadselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

	-- gyrocopter
	BindButtonToUnitEBP( "command_big_icon01", HK_LandingPad_CreateGyrocopter, "dobuildunit", "tooltip_gyrocopterunit", id, BuildingEBPFromType( Gyrocopter_EC ) )

	-- kill self
	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

--
tooltipbuilding = function( enabled, ebpid )

	tooltip_ebp(enabled,ebpid)

	-- find short cut key
	local type = TypeFromEBP(ebpid)


	local countstructures = getn( currentstructures )
	for i = 1, countstructures
	do
		if currentstructures[i][1] == type then

			HelpTextShortcut( currentstructures[i][2] )
			HelpTextTextWithRequirements( currentstructures[i][3] )

			if HasBuildLimit(ebpid) and (enabled == 1 or IsAtBuildLimit(ebpid) == 1) then

				HelpTextBuildLimit( ebpid )	
			
			end

		end
	end

end

tooltipbuildingtoggle = function( enabled, ndx )

	tooltip_command( enabled, ndx+1, togglebuildmenucommands )

end

backbuttontooltipcb = function( enabled, ndx )

	tooltip_command( enabled, 1, backbuttoncommand )

end

singleselectinfotooltip = function( index )

	if index >= 1 and index <= getn( singleselectinfotable ) then

	HelpTextTitle		( singleselectinfotable[ index ][1] )
	HelpTextTextWithoutRequirements( singleselectinfotable[index][2] )

	end


end



-- check to see if the given entity can build buildings
mcqualifier_buildbuilding = function( id )

	local type = EntityType( id )

	-- only Henchman and Lucy can build
	if type == Henchman_EC then
		return 1
	elseif type == Lucy_EC then
		return 1
	elseif type == Gyrocopter_EC then
		return 1
	end
	
	return 0

end

--
advancedbuildingtoggle = function()

	if currentstructures == structures then
		
		-- switch to advanced structures
		buildadvancedbuildingsmenu()

	else

		-- switch to basic structures
		buildbasicbuildingsmenu()

	end

end

--
buildbasicbuildingsmenu  = function()

	-- post event to indicate that the build button was pressed
	BuildButtonPressed()

	-- register function for refresh calls
	menucontext = { "buildbasicbuildingsmenu()", "mcqualifier_buildbuilding", "" }

	currentstructures = structures

	--
	cleartaskbar()

	-- toggle to advanced building
	if ( IsHudEnabled( ENABLE_HenchmanAdvancedBuild ) == 1) then
		BindButtonToHenchmanAdvancedBuild( "command_buildhenchman", HK_Henchman_BuildToggle, "buildadvancedbuildingsmenu", "tooltipbuildingtoggle", henchman_commands[7][4], 0 )
	end

	buildingsmenu()

end

--
buildadvancedbuildingsmenu  = function()

	-- register function for refresh calls
	menucontext = { "buildadvancedbuildingsmenu()", "mcqualifier_buildbuilding", "" }

	currentstructures = advancedstructures

	--
	cleartaskbar()

	-- toggle to basic building
	if ( IsHudEnabled( ENABLE_HenchmanBuild ) == 1) then
		BindButtonToHenchmanBuild( "command_buildhenchman", HK_Henchman_BuildToggle, "buildbasicbuildingsmenu", "tooltipbuildingtoggle", henchman_commands[6][4], 1 )
	end

	buildingsmenu()

end

--
buildingsmenu  = function()

	local id = SelectionId( 0 )

	-- command area

		-- back
	BindButton( "command_formation_icon01", HK_System_Escape, "on_selection", "backbuttontooltipcb", "UI/InGame/Back.tga", 0 )

		-- background
	ShowBitmapLabel( "command_bigicon_back" )

		-- buildings
	local bigbuttons = 
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10"
	}

	local countbuttons    = getn( bigbuttons )
	local countstructures = getn( currentstructures )

	local count

	if countbuttons < countstructures then

		count = countbuttons

	else

		count = countstructures

	end

	for i = 1, count
	do

		BindButtonToBuildingEBP( bigbuttons[i], currentstructures[i][2], "dobuildbuilding", "tooltipbuilding", BuildingEBPFromType( currentstructures[i][1] ) )

	end

end



--
tooltip_ebp = function( enabled, ebpid )

	if enabled == 1 then

		HelpTextEBPName(ebpid)
		HelpTextEBPCost(ebpid)

	else

		HelpTextEBPName(ebpid)
		HelpTextEBPCost(ebpid)
		HelpTextEBPPrerequisite(ebpid)

	end

end

getarmyunit_index = function( ebpid )

	local id = SelectionId( 0 )

	local countarmy   = ArmyCount()
	local countbutton = getn( armybuttons )

	local idxarmy   = 0
	local idxbutton = 1

	while idxarmy <= countarmy - 1 and idxbutton <= countbutton
	do

		local unit = ArmyAt( idxarmy )
		
		if unit == ebpid then
			return idxbutton
		end

		if UnitCanBeBuiltHere( id, unit ) == 1 then 

			idxbutton = idxbutton + 1
		end

		idxarmy = idxarmy + 1

	end

	return 1;
	
end

getarmyupgrade_index = function( ebpid )

	local id = SelectionId( 0 )

	local countarmy   = ArmyCount()
	local countbutton = getn( armybuttons )

	local idxarmy   = 0
	local idxbutton = 1

	while idxarmy <= countarmy - 1 and idxbutton <= countbutton
	do

		local unit = ArmyAt( idxarmy )
		
		if unit == ebpid then
			return idxbutton
		end

		idxbutton = idxbutton + 1
		idxarmy = idxarmy + 1

	end

	return 1;
	
end

tooltip_armyunit = function( enabled, ebpid )

	-- convert network id to index
	local ndx = getarmyunit_index(ebpid)

	HelpTextCreature( ebpid, enabled, armyunit_hotkeys[ndx] );

end

tooltip_upgradearmyunit = function( enabled, ebpid )

	-- convert network id to index
	local ndx = getarmyupgrade_index(ebpid)

	HelpTextCreatureToUpgrade( ebpid, armyunit_hotkeys[ndx] );

end

tooltip_upgrades = function( enabled, upgrade )

	-- validity check
	if ( creature_to_upgrade == -1 ) then
		return
	end

	local id = SelectionId( 0 )

	if enabled == 1 then

		HelpTextCreatureUpgrade(id, creature_to_upgrade, upgrade)
		HelpTextShortcut( creatureupgrades[ upgrade + 1 ][ 2 ] )
	
	else

		HelpTextCreatureUpgrade(id, creature_to_upgrade, upgrade)
		HelpTextCreatureUpgradePrerequisite(upgrade)
		
	end		

end

--
tooltip_henchmanbutton = function( enabled, ebpid )

	tooltip_ebp(enabled,ebpid)
	HelpTextShortcut(HK_Lab_CreateHenchman)
	HelpTextTextWithRequirements(42390)

end

--
henchmantooltip = function( enabled, index )

	tooltip_command( enabled, index, henchman_commands )

end

--
unloadtooltip = function( enabled )

	tooltip_command( enabled, 11, henchman_commands )

end
--
henchmanselection = function( id )

	-- command area
	BindButton( "command_modal_icon01",		henchman_commands[ 1][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 1][4],  1 )		-- move
	BindButton( "command_modal_icon02",		henchman_commands[ 2][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 2][4],  2 )		-- gather
	BindButton( "command_modal_icon03",		henchman_commands[ 3][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 3][4],  3 )		-- guard
	BindButton( "command_modal_icon04",		henchman_commands[ 4][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 4][4],  4 )		-- repair
	BindButton( "command_modal_icon05",		henchman_commands[12][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[12][4], 12 )		-- attack
	BindButton( "command_modal_icon07",		henchman_commands[ 5][2],  "dostop",			"henchmantooltip", henchman_commands[ 5][4],  5 )		-- stop

	if ( IsHudEnabled( ENABLE_HenchmanBuild ) == 1) then
		BindButtonToHenchmanBuild( "command_normal_icon01",	henchman_commands[ 6][2],  "buildbasicbuildingsmenu", "henchmantooltip", henchman_commands[ 6][4],  6 )		-- basic build
	end
	if ( IsHudEnabled( ENABLE_HenchmanAdvancedBuild ) == 1) then
		BindButtonToHenchmanAdvancedBuild( "command_normal_icon02",	henchman_commands[ 7][2],  "buildadvancedbuildingsmenu", "henchmantooltip", henchman_commands[ 7][4],  7 )		-- advanced build
	end

	if (IsResearched( RESEARCH_HenchmanHeal ) == 1) then
		BindButton( "command_normal_icon03",	henchman_commands[ 8][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[ 8][4],  8 )		-- heal
	end
	
	if (IsResearched( RESEARCH_HenchmanTag ) == 1) then
		BindButtonToTag( "command_normal_icon04",	henchman_commands[ 9][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[ 9][4],  id, 9 )		-- tag
		BindButtonToUnTag( "command_normal_icon05",	henchman_commands[14][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[14][4],  id, 14 )		-- untag
	end

	if ( IsHudEnabled( ENABLE_HenchmanKill ) == 1 ) then
		BindButton( "command_formation_icon07", henchman_commands[10][2],  "dokillconfirm", "henchmantooltip", henchman_commands[10][4], 10 )		-- kill
	end

end

--
selectpassenger = function( id )

	-- Check to see if there's no active modal cursor
	if ( GetModalCommandMode() == 0 ) then
	
		DoPendingUnload( id, 1 );			-- clear pending list and add passenger in
		dohenchmanmodal( 11 )				-- do unload modal
		
		local gyro_id = GetVehicleId( id )
		
	else
	
		local result = DoPendingUnload( id, 0 );
	
		-- we have deselected last of the passengers, quit the modal UI
		if (result == 0) then
			-- stop ui
			dounloadmodalcancel()
		end
	
	end
end

--
selectallpassenger = function( gyro_id )

	local count = PassengerCount( gyro_id )

	-- Process in the first passenger
	local first_id = PassengerId( gyro_id, 0 )
	DoPendingUnload( first_id, 1 );			-- clear pending list and add passenger in

	-- Do the rest of the passengers
	for i = 1, count - 1
	do

		local id = PassengerId( gyro_id, i )
		DoPendingUnload( id, 0 );

	end
	
	dohenchmanmodal( 11 )		-- do unload modal mode
end

--
gyrocopterpassengermulti = function( gyro_id )

	-- gyro passenger multi list
	local multibuttons =
	{
		{ "multiselect_icon15",	"multiselect_statbar15" },
		{ "multiselect_icon16",	"multiselect_statbar16" },
		{ "multiselect_icon17",	"multiselect_statbar17" },
		{ "multiselect_icon18",	"multiselect_statbar18" },

		{ "multiselect_icon24",	"multiselect_statbar24" },
		{ "multiselect_icon25",	"multiselect_statbar25" },
		{ "multiselect_icon26",	"multiselect_statbar26" },
		{ "multiselect_icon27", "multiselect_statbar27" },

		{ "multiselect_icon33", "multiselect_statbar33" },
		{ "multiselect_icon34", "multiselect_statbar34" },
		{ "multiselect_icon35", "multiselect_statbar35" },
		{ "multiselect_icon36", "multiselect_statbar36" },
	}

	local count = PassengerCount( gyro_id )

	if  count > getn( multibuttons ) then
		
		count = getn( multibuttons )

	end

	for i = 0, count - 1
	do

		local id = PassengerId( gyro_id, i )
		
		BindButtonToUnloadPassenger( multibuttons[ i + 1 ][ 1 ], "", "selectpassenger", "", id, gyro_id )
		BindBarToEntityHealth( multibuttons[ i + 1 ][ 2 ], id, "", 0 );

	end

end

--
gyrocopterselection = function( id )

	-- command area
	BindButton( "command_modal_icon01",		henchman_commands[ 1][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 1][4],  1 )		-- move
	BindButton( "command_modal_icon07",		henchman_commands[ 5][2],  "dostop",			"henchmantooltip", henchman_commands[ 5][4],  5 )		-- stop
	
	if ( CanBuild( id ) == 1 ) then
		BindButtonToHenchmanBuild( "command_normal_icon01",	henchman_commands[ 6][2],  "buildbasicbuildingsmenu",	"henchmantooltip", henchman_commands[ 6][4],  6 )		-- build
		BindButton( "command_normal_icon02",	henchman_commands[ 7][2],  "buildadvancedbuildingsmenu","henchmantooltip", henchman_commands[ 7][4],  7 )		-- build
	else
		BindButtonDisabled( "command_normal_icon01",	henchman_commands[ 6][2],  "buildbasicbuildingsmenu",	"henchmantooltip", henchman_commands[ 6][4],  6 )		-- build
		BindButtonDisabled( "command_normal_icon02",	henchman_commands[ 7][2],  "buildadvancedbuildingsmenu","henchmantooltip", henchman_commands[ 7][4],  7 )		-- build
	end

	if (IsResearched( RESEARCH_HenchmanTag ) == 1) then
		BindButtonToTag( "command_normal_icon04",	henchman_commands[ 9][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[ 9][4],  id, 9 )		-- tag
		BindButtonToTag( "command_normal_icon05",	henchman_commands[14][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[14][4],  id, 14 )		-- untag
	end
	
	if ( CanUnload( id ) == 1 ) then
		BindButtonToUnload( "command_normal_icon03",	henchman_commands[11][2],  "selectallpassenger", "unloadtooltip", henchman_commands[11][4], id, 11 )		-- unload
		
		if SelectionCount() == 1 then
			gyrocopterpassengermulti( id )	-- passenger multiselection UI
		end
		
		-- Disable these buttons only if we currently do not carry any passengers
		BindButtonDisabled( "command_modal_icon02",		henchman_commands[ 2][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 2][4],  2 )		-- gather
		BindButtonDisabled( "command_modal_icon03",		henchman_commands[ 3][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 3][4],  3 )		-- guard
		BindButtonDisabled( "command_modal_icon04",		henchman_commands[ 4][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 4][4],  4 )		-- repair
	else 
		if ( CanAirlift( id ) == 1 ) then
			BindButtonToAirlift( "command_normal_icon03",	henchman_commands[13][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[13][4], id, 13 )		-- airlift
		else
			BindButtonDisabled( "command_normal_icon03",	henchman_commands[13][2],  "dohenchmanmodal", "henchmantooltip", henchman_commands[13][4], 13 )			-- disable airlift
		end

		-- Enable these buttons only if we currently do not carry any passengers
		BindButton( "command_modal_icon02",		henchman_commands[ 2][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 2][4],  2 )		-- gather
		BindButton( "command_modal_icon03",		henchman_commands[ 3][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 3][4],  3 )		-- guard
		BindButton( "command_modal_icon04",		henchman_commands[ 4][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 4][4],  4 )		-- repair
	end

	if ( IsHudEnabled( ENABLE_HenchmanKill ) == 1 ) then 
		BindButton( "command_formation_icon07", henchman_commands[10][2],  "dokillconfirm", "henchmantooltip", henchman_commands[10][4], 10 )		-- kill
	end

end

--
rextooltip = function( enabled, index )

	tooltip_command( enabled, index, rex_commands )

end

--
rexselection = function( id )

	-- command area
	BindButton( "command_modal_icon01",		rex_commands[ 1][2],  "dorexmodal",		"rextooltip", rex_commands[ 1][4],  1 )		-- move
	BindButton( "command_modal_icon07",		rex_commands[ 3][2],  "dostop",			"rextooltip", rex_commands[ 3][4],  3 )		-- stop

	if ( CanBeLabGarrisoned( id ) == 1 ) then
		BindButton( "command_normal_icon05", rex_commands[ 6][2],  "dogarrison",		"rextooltip", rex_commands[ 6][4],	6 )		-- garrison
	end

	if ( CanGatherDNA() == 1 ) then
		BindButtonToTag( "command_normal_icon06",	rex_commands[ 5][2],  "dorexmodal",		"rextooltip", rex_commands[ 5][4], id, 5 )		-- gather DNA
	end

end


--
lucytooltip = function( enabled, index )

	tooltip_command( enabled, index, lucy_commands )

end


--
lucyselection = function( id )

	-- command area
	BindButton( "command_modal_icon01",		lucy_commands[ 1][2],  "dolucymodal",		"lucytooltip", lucy_commands[ 1][4],  1 )		-- move
	BindButton( "command_modal_icon02",		lucy_commands[ 10][2], "dolucymodal",	 	"lucytooltip", lucy_commands[ 10][4],  10 )		-- gather
	BindButton( "command_modal_icon03",		lucy_commands[ 2][2],  "dolucymodal",		"lucytooltip", lucy_commands[ 2][4],  2 )		-- guard
	BindButton( "command_modal_icon04",		lucy_commands[ 4][2],  "dolucymodal",		"lucytooltip", lucy_commands[ 4][4],  4 )		-- repair
	BindButton( "command_modal_icon07",		lucy_commands[ 3][2],  "dostop",			"lucytooltip", lucy_commands[ 3][4],  3 )		-- stop

	if ( CanBeLabGarrisoned( id ) == 1 ) then
		BindButton( "command_normal_icon05", lucy_commands[ 7][2],  "dogarrison",		"lucytooltip", lucy_commands[ 7][4],  7 )		-- garrison
	end

	BindButton( "command_normal_icon07",	lucy_commands[ 5][2],  "dolucymodal",		"lucytooltip", lucy_commands[ 5][4],  5 )		-- gather tech

	-- build
	BindButtonToHenchmanBuild( "command_normal_icon01",	lucy_commands[ 8][2],  "buildbasicbuildingsmenu",	"lucytooltip", lucy_commands[ 8][4],  8 )
	BindButton( "command_normal_icon02",	lucy_commands[ 9][2],  "buildadvancedbuildingsmenu", "lucytooltip", lucy_commands[ 9][4],  9 )		-- advanced build

	-- sabotage is a triggered ability and its corresponding button is handled
	-- elsewhere

end


--
commandqueuecancel = function()

	domodalcancel()

end

--
commandqueuecancelignore = function()

	-- Empty on purpose

end

--
mcqualifier_rex = function( id )

	local type = EntityType( id )
	if type == Rex_EC then
		return 1
	end

	return 0

end

--
dorexmodal = function( index )

	-- register function for refresh calls
	menucontext = { "dorexmodal(" .. index .. ")", "mcqualifier_rex", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= rex_modalmodes[ index ][1];
	local command	= rex_modalmodes[ index ][2];
	
	-- let rex's modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

--
mcqualifier_lucy = function( id )

	local type = EntityType( id )
	if type == Lucy_EC then
		return 1
	end

	return 0

end

--
dolucymodal = function( index )

	-- register function for refresh calls
	menucontext = { "dolucymodal(" .. index .. ")", "mcqualifier_lucy", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= lucy_modalmodes[ index ][1];
	local command	= lucy_modalmodes[ index ][2];
	
	-- let lucy's modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

--
mcqualifier_henchman = function( id )

	local type = EntityType( id )
	if 	(type == Henchman_EC) then
		return 1
	elseif (type == Gyrocopter_EC) then
		if (UpdatePendingUnloadGroup( id ) == 1) then
			return 1
		end
	end
	
	return 0

end

--
dohenchmanmodal = function( index )

	-- register function for refresh calls
	menucontext = { "dohenchmanmodal(" .. index .. ")", "mcqualifier_henchman", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= henchman_modalmodes[ index ][1];
	local command	= henchman_modalmodes[ index ][2];
	
	-- let henchman modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	-- inplace commands
	local result = 0
	
	if (command == MC_Unload) then
	
		result = ModalUIBegin( "domodalclick", "dounloadmodalcancel", mode, command )
	else
	
		result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )
	end

	if result == 0 then

		--
		cleartaskbar()

		-- command area
		
		-- inplace commands
		if (command == MC_Unload) then

			-- Need to refresh passenger icons on the left-hand-side hud
			-- Get selection id's (there should only be one entity selected)
			local count = SelectionCount()
			local id = SelectionId( 0 )
			
			-- Make sure it is a gyrocopter and only one selection
			local type = EntityType( id )
			if (type == Gyrocopter_EC) and (count == 1) then
				gyrocopterpassengermulti( id );
			end
		
			local t = henchman_commands[ index ];
			BindButton( "command_normal_icon03", t[2], "dohenchmanunloadnow", "henchmantooltip", t[4], index )
					
			-- cancel button
			BindButton( "command_formation_icon07", HK_System_Escape, "dounloadmodalcancel", "", "UI/InGame/Cancel.tga", 0 )
		
		else
			-- cancel button
			BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )
		end
		
	else

		-- failed
		failedcommand( result )
	
	end

end

-- check to see if an entity can self-destruct
mcqualifier_killconfirm = function( id )

	local type = EntityType( id )

	-- Rex and Lucy cannot self destruct
	if type == Rex_EC then
		return 0
	elseif type == Lucy_EC then
		return 0
	end

	return 1

end

--
dokillconfirm = function( index )

	-- register function for refresh calls
	menucontext = { "dokillconfirm( " .. index .. " )", "mcqualifier_killconfirm", "" }
	
	--
	cleartaskbar()

	ShowHud( "textlabel_kill_unit" )

	-- command area
	BindButton( "command_formation_icon06", HK_Generic_Kill,   "dodestroy",     "", "UI/InGame/accept.tga", 0 )
	BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

end

--
dostop = function( dummy )

	DoCommandStop()

end

--
dostance = function( stanceindx )

	DoCommandStance( stance_commands[ stanceindx ][ 5 ] )

end

--
dodestroy = function( dummy )
	
	DoDestroy()

end

--
dogarrison = function( dummy )
	
	DoGarrison()

end

--
mcqualifier_special = function( id )

	-- validate the command context
	if ( special_modal_type == -1 ) then
		return 0
	end

	-- check to see if entity has the given ability	
	if SelectionHasSpecialCommand( ability_commands[ special_modal_type ][ 5 ] ) == 1 then
		return 1
	end

	return 0

end

--
dospecial = function( type )

	if ability_commands[type][6] == 0 then

		CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

		-- are we in command queue mode?
		local queue = ModalCommandQueueRequest()
		
		local result = DoCommandSpecial( ability_commands[ type ][ 5 ], queue )

		if result == 0 then

		else

			failedcommand( result )

		end

	else

		-- register function for refresh calls
		menucontext = { "dospecial(" .. type .. ")", "mcqualifier_special", "ModalUIEnd()" }

		-- remember the special command index
		special_modal_type = type

		-- translate mode in game usable mode
		local mode		= creature_abilitymodalmodes[ type ][1];
		local command	= creature_abilitymodalmodes[ type ][2];
		
		CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancelignore" )
		
		--
		local result = ModalUIBegin( "dospecialmodalclick", "domodalcancel", mode, command )

		if result == 0 then

			--
			cleartaskbar()

			-- command area
				-- cancel button
			BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

		else

			-- failed
			failedcommand( result )
		
		end

	end

end

--
mcqualifier_specialmodal = function( id )

	-- validate the command context
	if ( special_modal_index == -1 ) then
		return 0
	end

	local index	= special_modal_index
	local command	= creature_specialmodalmodes[ index ][2];

	-- check to see if the entity has the triggered ability
	local t = modal_special_commands[ index ]
	if SelectionHasTriggeredAbility( t[ 5 ] ) == 1 then
		return 1
	end

	return 0

end

--
dospecialmodal = function( index )

	-- register function for refresh calls
	menucontext = { "dospecialmodal(" .. index .. ")", "mcqualifier_specialmodal", "ModalUIEnd()" }

	-- remember the special command index
	special_modal_index = index	

	-- translate mode in game usable mode
	local mode	= creature_specialmodalmodes[ index ][1];
	local command	= creature_specialmodalmodes[ index ][2];

	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancelignore" )
	
	--
	local result = ModalUIBegin( "dospecialmodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		local t = modal_special_commands[ index ]

		-- command area
			
			-- inplace attack

		-- no inplace attack for plague, assassinate & sabotage
		local inplace = 1

		if 	command ~= MC_Plague and command ~= MC_Sabotage and command ~= MC_Assassinate and command ~= MC_Infestation
		then
			BindButton( "command_formation_icon01", t[2], "domodalnow", "modalspecialstooltip", t[4], index )
		end

		-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

--
specialstooltip = function( enabled, index )

	tooltip_command( enabled, index, ability_commands )

end

--
modalspecialstooltip = function( enabled, index )

	tooltip_command( enabled, index, modal_special_commands )

end

--
specialselection = function( )

	-- clear the special selection index
	special_modal_index = -1
	special_modal_type  = -1

	local buttons = 
	{
		"command_formation_icon01",
		"command_formation_icon02",
		"command_formation_icon03",
		"command_formation_icon04",
		"command_formation_icon05",
		"command_formation_icon06",
	}

	local countSpecial = getn( ability_commands )
	local countButton  = getn( buttons )

	local idxSpecial = 1
	local idxButton  = 1

	while idxSpecial <= countSpecial and idxButton <= countButton
	do

		if SelectionHasSpecialCommand( ability_commands[ idxSpecial ][ 5 ] ) == 1 then

			BindButtonToTriggeredAbility( buttons[ idxButton ], ability_commands[ idxSpecial ][2],  "dospecial",	"specialstooltip", ability_commands[ idxSpecial ][4], ability_commands[ idxSpecial ][ 5 ], idxSpecial )

			idxButton = idxButton  + 1

		end

		idxSpecial = idxSpecial + 1

	end


	-- Modal Special commands
	local countModalSpecial = getn( modal_special_commands )

	local idxModalSpecial = 1

	while idxModalSpecial <= countModalSpecial and idxButton <= countButton
	do

		local t = modal_special_commands[ idxModalSpecial ]

		if SelectionHasTriggeredAbility( t[ 5 ] ) == 1 then

			BindButtonToTriggeredAtk( buttons[ idxButton ], t[2],  "dospecialmodal", "modalspecialstooltip", t[4], t[5], idxModalSpecial )

			idxButton = idxButton  + 1

		end

		idxModalSpecial = idxModalSpecial + 1

	end

end

--
creaturetooltip = function( enabled, index )

	tooltip_command( enabled, index, creature_commands )

end

--
stancetooltip = function ( enabled, index )

	tooltip_command( enabled, index, stance_commands )

end

--
creaturemodalselection = function( id )

	if (EntityIsDigging( id ) == 0) then
		
		-- command area
		BindButton( "command_modal_icon01",		creature_commands[1][2],	"docreaturemodal",	"creaturetooltip", creature_commands[1][4], 1 )		-- move
		BindButton( "command_modal_icon02",		creature_commands[3][2],	"docreaturemodal",	"creaturetooltip", creature_commands[3][4], 3 )		-- patrol
		BindButton( "command_modal_icon03",		creature_commands[2][2],	"docreaturemodal",	"creaturetooltip", creature_commands[2][4], 2 )		-- guard
		BindButton( "command_modal_icon05",		creature_commands[7][2],	"docreaturemodal",	"creaturetooltip", creature_commands[7][4], 7 )		-- attack

			-- attack ground
		if SelectionHasAttackGround( ) == 1 then 

			BindButtonToAttackGround( "command_modal_icon06",	creature_commands[5][2],	"docreaturemodal",	"creaturetooltip", creature_commands[5][4],  5 )		-- range attack

		end

	end

	-- code common to dug and undug goes here

	BindButton( "command_modal_icon07",		creature_commands[4][2],	"dostop",			"creaturetooltip", creature_commands[4][4],  4 )		-- stop

	BindButton( "command_formation_icon07",	creature_commands[6][2],	"dokillconfirm",	"creaturetooltip", creature_commands[6][4],  6 )

end

--
creaturestanceselection = function( id )

	if (EntityIsDigging( id ) == 0) then

		BindButtonToCreatureStance( "command_normal_icon01",	stance_commands[1][2],		"dostance",			"stancetooltip", stance_commands[1][4],  1, stance_commands[1][5] )
		BindButtonToCreatureStance( "command_normal_icon02",	stance_commands[2][2],		"dostance",			"stancetooltip", stance_commands[2][4],  2, stance_commands[2][5] )
		BindButtonToCreatureStance( "command_normal_icon03",	stance_commands[3][2],		"dostance",			"stancetooltip", stance_commands[3][4],  3, stance_commands[3][5] )
		BindButtonToCreatureStance( "command_normal_icon04",	stance_commands[4][2],		"dostance",			"stancetooltip", stance_commands[4][4],  4, stance_commands[4][5] )

	end
	
end

--
inresearchselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- should remain empty

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill
end

--
inlabresearchselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

	-- ungarrison Rex
	if EntityTypeGarrisoned( id, Rex_EC ) == 1 then

		BindButton( "command_big_icon10", commands[2][2], "doungarrison", "commandstooltip", commands[2][4], 2 )

	end

	-- ungarrison Lucy
	if EntityTypeGarrisoned( id, Lucy_EC ) == 1 then

		BindButton( "command_big_icon09", commands[3][2], "doungarrison", "commandstooltip", commands[3][4], 3 )

	end

end

--
inconstructionselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- should remain empty

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill
end

--
inextensionselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- should remain empty

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

--
increatureupgradeselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- should remain empty

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill
end

--
chamberselection = function()

	local id = SelectionId( 0 )

	--
	buildunitsmenu()

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

-- 
vetclinicselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

	commandarearesearch( id, vetresearch, "tooltip_vetresearch" )

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

-- 
foundryselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )	

	commandarearesearch( id, foundaryresearch, "tooltip_foundaryresearch" )

	BindButton( "command_formation_icon01", commands[4][2], "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

	-- rally
	BindButton( "command_modal_icon01", commands[1][2], "dospawnmodal", "commandstooltip", commands[1][4], 1 )

	-- resource conversion
	BindButtonToResourceConversion( "command_big_icon01", foundry_commands[1][2], "doresourceconversion", "foundrytooltip", foundry_commands[1][4], foundry_commands[1][5] )
	BindButtonToResourceConversion( "command_big_icon02", foundry_commands[2][2], "doresourceconversion", "foundrytooltip", foundry_commands[2][4], foundry_commands[2][5] )
	BindButtonToResourceConversion( "command_big_icon03", foundry_commands[3][2], "doresourceconversion", "foundrytooltip", foundry_commands[3][4], foundry_commands[3][5] )
	BindButtonToResourceConversion( "command_big_icon04", foundry_commands[4][2], "doresourceconversion", "foundrytooltip", foundry_commands[4][4], foundry_commands[4][5] )

end

--
generatorselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

	-- uncomment this to give addons to the generator
	commandareaaddon( id, generatoraddons, "tooltip_generatoraddon" )

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

--

resourcerenewselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )
	
	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

-- 
geneticampselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- clear the creature to upgrade selection
	creature_to_upgrade = -1

	-- command area

		-- background
	ShowBitmapLabel( "command_bigicon_back" )

		
	local countarmy   = ArmyCount()
	local countbutton = getn( armybuttons )

	local idxarmy   = 0
	local idxbutton = 1

	while idxarmy <= countarmy - 1 and idxbutton <= countbutton
	do

		local unit = ArmyAt( idxarmy )
		
		BindButtonToUpgradeEBP( armyupgradebuttons[ idxbutton ][1], armyupgradebuttons[ idxbutton ][2], armyupgradebuttons[ idxbutton ][3], armyunit_hotkeys[ idxbutton ], "upgradeunitsmenu", "tooltip_upgradearmyunit", id, unit )

		idxbutton = idxbutton + 1
		idxarmy = idxarmy + 1

	end

	--
	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

--
buildingselection = function( id, type )

	-- display building's rally point, if any
	RallyPointShow( id )
	
	if EntityInConstruction( id ) == 1 then

		-- in construction
		inconstructionselection()

	elseif EntityInResearch( id ) == 1 then
		
		-- researching
		if type == Lab_EC then

			inlabresearchselection()

		else

			inresearchselection()

		end

	elseif EntityInExtension( id ) == 1 then
		
		-- add-ons
		inextensionselection()

	elseif EntityInCreatureUpgrade( id ) == 1 then

		-- doing creature upgrade
		increatureupgradeselection()		

	else

		if type == Lab_EC then
		
			labselection()

		elseif type == ElectricGenerator_EC then

			generatorselection()

		elseif type == ResourceRenew_EC then
			
			resourcerenewselection()
		
		elseif type == VetClinic_EC then
			
			vetclinicselection()
		
		elseif type == RemoteChamber_EC or type == WaterChamber_EC or type == Aviary_EC then

			chamberselection()

		elseif type == Foundry_EC then

			foundryselection()

		elseif type == LandingPad_EC then

			landingpadselection()

		elseif type == GeneticAmplifier_EC then
			
			geneticampselection()
			
		elseif type == SoundBeamTower_EC then
		
			soundbeamtowerselection()
			
		elseif type == AntiAirTower_EC then
		
			antiairtowerselection()
			
		else
			
			miscbuildingselection()

		end

	end

end

--
miscbuildingselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- should remain empty

	BindButton( "command_formation_icon01", commands[4][2],  "dokillconfirm", "commandstooltip", commands[4][4], 4 )		-- kill

end

--
selectentity = function( id )

	-- check to see if the select similar entity button is pressed
	-- currently this button is 'Shift'
	local actOnSimilar = IsSelectSimilarPressed()
	
	-- check to see if the select single entity button is being pressed
	-- currently this button is 'Ctrl'
	if (IsSelectSinglePressed() == 1) then
		DeSelectEntity( id, actOnSimilar )
	else
		SelectEntity( id, actOnSimilar )
	end
	

end

--
friendlyselection = function()

	--
	cleartaskbar()

	-- just need one id for each type
	creatureId		= -1
	rexId			= -1
	lucyId			= -1
	henchmanId		= -1
	gyrocopterId	= -1
	buildingId		= -1
	buildingType	= -1
	showStance		= 1

	-- check what's in our selection
	for i = 1, SelectionCount()
	do

		local id = SelectionId( i - 1 )

		-- per type stuff
		local type = EntityType( id )

		if type == Henchman_EC then

			henchmanId = id

		elseif type == Gyrocopter_EC then

			gyrocopterId = id

		elseif type == Rex_EC then

			rexId = id

		elseif type == Lucy_EC then

			lucyId = id

		elseif type == Creature_EC or type == Animal_EC then

			if (EntityIsDigging( id ) == 0) then

				creatureId = id

			elseif creatureId == -1 then

				creatureId = id

			end

		else

			-- building
			buildingId = id
			buildingType = type

		end

	end


	if not (buildingId == -1) then

		buildingselection( buildingId, buildingType)

	else
	
		WayPointPathShow()

		if not (creatureId == -1) then

			creaturemodalselection( creatureId )

			if SelectionAllSameType() then

				PatrolPathShow()

			end

		end

		if not (henchmanId == -1) then

			henchmanselection( henchmanId )
			showStance = 0

		end
		if not (gyrocopterId == -1) then

			gyrocopterselection( gyrocopterId )
			showStance = 0

		end
		if not (rexId == -1) then

			rexselection( rexId )
			showStance = 0

		end
		if not (lucyId == -1) then

			lucyselection( lucyId )
			showStance = 0

		end

		if not (creatureId == -1) and showStance == 1 then

			creaturestanceselection( creatureId )

		end

		specialselection()

	end

end

--
enemyselection = function()

	--
	cleartaskbar()

	-- command area
		-- empty

end

--
worldselection = function()

	--
	cleartaskbar()

	-- command area
		-- empty

end

--
emptyselection = function()

	cleartaskbar()

end

--
failedcommand = function( reason )

	local errmsg = {}
		errmsg[ FC_NeedScrap       ] = { 40800,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_NeedElectricity ] = { 40801,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_BuildQueueFull  ] = { 40802,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_TooManyUnit     ] = { 40803,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_CantDig		   ] = { 40804,	"audio/ui/AlertShort.pat" }		
		errmsg[ FC_CantUnload	   ] = { 40805,	"audio/ui/AlertShort.pat" }		
		errmsg[ FC_Other		   ] = { 0, "" }

	BindLabelToTextTimer( "contextarea_line01", errmsg[ reason ][ 1 ], 5 )

	PlaySound( errmsg[ reason ][ 2 ] )

end

--
dobuildunit = function( ebpid )

	local result = DoBuildUnit( ebpid )

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
docreatureupgrade = function( upgradeid )

	local result = DoCreatureUpgrade( creature_to_upgrade, upgradeid )

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
docancelconstruction = function( dummy )

	DoCancelConstruction()

end

--
docancelbuildunit = function( unitindex )

	DoCancelBuildUnit( unitindex )

end

--
docancelresearch = function( building )

	DoCancelResearch()

end


--
docanceladdon = function( building )

	DoCancelAddOn()

end

--
docancelcreatureupgrade = function( building )

	DoCancelCreatureUpgrade()

end

--
doresearch = function( research )

	local result = DoResearch( research )

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
doaddon = function( addon )

	local result = DoAddOn( addon )

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
dobuildbuildingcancel = function( dummy )

	-- stop ui
	BuildUIEnd()

	--
	on_selection()

end

-- 
buildbuilding_updateui = function()

	--
	cleartaskbar()

	-- command area
		-- cancel button
	BindButton( "command_formation_icon07", HK_System_Escape, "dobuildbuildingcancel", "", "UI/InGame/Cancel.tga", 0 )

end

--
dobuildbuilding = function( ebpid )

	-- register function for refresh calls
	--   we want the verify the context but we don't want to recall this function;
	--   thus the dummy function
	menucontext = { "buildbuilding_updateui()", "mcqualifier_buildbuilding", "BuildUIEnd()" }

	-- post event to indicate that a build-building button is pressed
	BuildEBPButtonPressed( ebpid )		

	local result

	-- let building modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	-- detemine if this ebpid is a fence or not
	if TypeFromEBP( ebpid ) == BrambleFence_EC then
		
		-- start fence ui
		result = BuildUIBegin( "dobuildfenceclick", "dobuildbuildingabort", ebpid )

	else

		-- start plain building ui
		result = BuildUIBegin( "dobuildbuildingclick", "dobuildbuildingabort", ebpid )

	end

	if result == 0 then

		buildbuilding_updateui( )


	else

		-- failed
		failedcommand( result )
	
	end


end

--
dobuildbuildingclick = function( ebpid, x, y, z, dummy )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()

	if queue == 0 then

		-- stop ui, we aren't queuing
		BuildUIEnd()

	end

	-- send command
	local result
	
	result = DoBuildBuilding( ebpid, x, y, z, queue )

	if not( result == 0 ) then

		failedcommand( result )

	end

end

--
dobuildfenceclick = function( ebpid, x, y, z, x2, y2, z2, dummy )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()

	if queue == 0 then

		-- stop ui, we aren't queuing
		BuildUIEnd()

	end

	-- send command
	DoBuildFence( ebpid, x, y, z, x2, y2, z2, queue )

end

--
dogyrocopterupgradeclick = function( ebpid, x, y, z, dummy )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()

	if queue == 0 then

		-- stop ui, we aren't queuing
		BuildUIEnd()

	end

	-- send command
	DoGyrocopterUpgrade( ebpid, x, y, z, queue )

end

--
dobuildbuildingabort = function( ebpid )

	-- stop ui
	BuildUIEnd()

	-- reset info center & command area
	on_selection()

end


--
doradarpulse = function( )

	-- fire radar pulse
	local result = DoRadarPulse()

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
dosonicboom = function( )

	-- fire sonic boom
	local result = DoSonicBoom()

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
doairburst = function( )

	-- fire air burst
	local result = DoAirBurst()

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
dolabdefense = function( turnon )

	-- toggle lab defense
	local result = DoLabDefense( turnon )

	if result == 0 then
		
		-- success

	else

		-- failed
		failedcommand( result )

	end

end

-- check to see if the given entity can call the buildunitsmenu
mcqualifier_buildunits = function( id )

	local type = EntityType( id )

	-- only remote chamber can do
	if type == RemoteChamber_EC then
		return 1
	end
	
	return 0

end

--
buildunitsmenu = function()

	-- register function for refresh calls
	menucontext = { "buildunitsmenu()", "mcqualifier_buildunits", "" }

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- command area
		-- rally
	BindButton( "command_modal_icon01", commands[1][2], "dospawnmodal", "commandstooltip", commands[1][4], 1 )

		-- background
	ShowBitmapLabel( "command_bigicon_back" )

		
	local countarmy   = ArmyCount()
	local countbutton = getn( armybuttons )

	local idxarmy   = 0
	local idxbutton = 1

	while idxarmy <= countarmy - 1 and idxbutton <= countbutton
	do

		local unit = ArmyAt( idxarmy )
		
		if UnitCanBeBuiltHere( id, unit ) == 1 then 

			BindButtonToUnitEBP( armybuttons[ idxbutton ], armyunit_hotkeys[ idxbutton ], "dobuildunit", "tooltip_armyunit", id, unit )

			idxbutton = idxbutton + 1
		end

		idxarmy = idxarmy + 1

	end

	if ArmyBuilderAllowed() == 1 then

		BindButtonToArmyBuilder( "command_big_icon10",  HK_Generic_ArmyBuilder, "armybuilder", "menutooltip", "ui/ingame/armybuilder.tga", 2 )

	end

end

-- check to see if the given entity can call the upgradeunitsmenu
mcqualifier_upgradeunits = function( id )

	local type = EntityType( id )

	-- only genetic amplifier can do
	if type == GeneticAmplifier_EC then
		return 1
	end
	
	return 0

end

--
upgradeunitsmenu = function( ebpnetid )

	-- validate argument
	if ebpnetid == -1 then
		return
	end

	-- register function for refresh calls
	menucontext = { "upgradeunitsmenu( " .. ebpnetid .. " )", "mcqualifier_upgradeunits" , "" }

	creature_to_upgrade = ebpnetid

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- back
	BindButton( "command_formation_icon01", HK_System_Escape, "on_selection", "backbuttontooltipcb", "UI/InGame/Back.tga", 0 )

	-- background
	ShowBitmapLabel( "command_bigicon_back" )

	-- command area
	commandareaupgrade( id, ebpnetid, creatureupgrades, "tooltip_upgrades" )

end

--
doungarrison = function( index )

	if index == 2 then
		DoUngarrison( Rex_EC )
	else
		DoUngarrison( Lucy_EC )
	end

end

-- 
dospawnmodal = function( index )

	-- register function for refresh calls
	menucontext = { "dospawnmodal(" .. index .. ")", "", "ModalUIEnd()" }

	-- translate mode in game usable mode (only one here)
	local mode		= MM_LockCursor
	local command	= MC_SetRallyPoint
	
	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

--
mcqualifier_creaturemodal = function( id )

	-- check to see if the entity is dugged in
	if (EntityIsDigging( id ) == 0) then
		return 1
	end

	return 0

end

--
docreaturemodal = function( index )

	-- register function for refresh calls
	menucontext = { "docreaturemodal(" .. index .. ")", "mcqualifier_creaturemodal", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= creature_modalmodes[ index ][1];
	local command	= creature_modalmodes[ index ][2];
	
	-- let creature modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

--
doteamobjmodal = function()

	-- register function for refresh calls
	menucontext = { "doteamobjmodal()", "", "ModalUIEnd()" }

	local mode 	= teamobj_modalmode[1];
	local command	= teamobj_modalmode[1];

	--
	local result = ModalUIBegin( "doteamobjmodalclick", "domodalcancel", mode, command )
	
	if result == 0 then

		--
		cleartaskbar()

		-- remove hotkey for team objective so we can't re-enter the team objective placement mode
		-- while we are in it
		BindHotkey( HK_Generic_TeamObjective, "", 0 )

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )
	
	end

end

-- 
doteamobjmodalclick = function( mode, x, y, z, ebpid )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()
	
	if queue == 0 then

		-- stop ui (taskbar)
		ModalUIEnd()

	end
		
	-- send command based on modal mode	(proxy)
	SetTeamObjPosition( x, y, z  )

end

--
dospecialmodalclick = function( mode, x, y, z, entityID )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()

	if mode == MC_SonarPulse then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_Plague then

		DoModalCommand( mode, 0, 0, 0, entityID, queue )

	elseif mode == MC_ElectricalBurst then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_QuillBurst then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_StinkBurst then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_Sabotage then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_WebThrow then

		DoModalCommand( mode, x, y, z, entityID, queue )

	elseif mode == MC_Assassinate then

		DoModalCommand( mode, 0, 0, 0, entityID, queue )

	elseif mode == MC_Flash then

		DoModalCommand( mode, x, y, z, entityID, queue )
		
	elseif mode == MC_Infestation then

		DoModalCommand( mode, 0, 0, 0, entityID, queue )

	elseif mode == MC_Jumping then

		DoModalCommand( mode, x, y, z, entityID, queue )
	end

	-- stop ui (taskbar)
	ModalUIEnd()

	--
	on_selection()

end


--
domodalclick = function( mode, x, y, z, ebpid )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()
	
	if queue == 0 then

		-- stop ui (taskbar)
		ModalUIEnd()

	end

	-- send command based on modal mode	(proxy)
	DoModalCommand( mode, x, y, z, ebpid, queue )
end

domodalnow = function( index )

	local t = modal_special_commands[ index ]

	local queue = ModalCommandQueueRequest()
	
	DoCommandTriggered( t[ 5 ], queue )

	-- stop ui
	ModalUIEnd()

	--
	on_selection()
	
end

--
domodalcancel = function( dummy )

	-- stop ui
	ModalUIEnd()

	--
	on_selection()

end

--
dounloadmodalcancel = function( dummy )

	-- clear out pending unload list
	DoCancelPendingUnload()
	
	-- stop ui
	ModalUIEnd()

	--
	on_selection()

end

--
dohenchmanunloadnow = function( index )

	-- unload
	local result = DoUnloadHere()

	if result == 0 then
		
		-- success

		-- stop ui
		ModalUIEnd()

		--
		on_selection()

	else

		-- failed
		failedcommand( result )

	end

end

--
doresourceconversion = function( type )

	DoResourceConversion( type )

end

--
tooltipresearch = function( enabled, research, infotable )

	if enabled == 1 then

		HelpTextResearchName(research)
		HelpTextResearchCost(research)

	else

		HelpTextResearchName(research)
		HelpTextResearchCost(research)
		HelpTextResearchPrerequisite(research)

	end


	local count = getn( infotable )
	for i = 1, count
	do
		if infotable[i][1] == research then
			HelpTextShortcut( infotable[i][2] )
			HelpTextTextWithRequirements( infotable[i][3] )
		end
	end

end

tooltip_labresearch = function( enabled, research )

	tooltipresearch(enabled,research,labresearch)
end

tooltip_vetresearch = function( enabled, research )

	tooltipresearch(enabled,research,vetresearch)
end

tooltip_foundaryresearch = function( enabled, research )

	tooltipresearch(enabled,research,foundaryresearch)
end

-- 
commandareaaddon = function( id, addon, tooltipcb )

	--
	local buttons = 
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10",
	}

	local countbuttons  = getn( buttons  )
	local countaddon = getn( addon )

	local count

	if countbuttons < countaddon then

		count = countbuttons

	else

		count = countaddon

	end

	for i = 1, count 
	do
		if AddOnIsAllowed( addon[ i ][1] ) == 1 then

			if AddOnIsInstalled( id, addon[ i ][1] ) == 0 then
			
				BindButtonToAddOn( buttons[i], addon[ i ][ 2 ], "doaddon", tooltipcb, id, addon[ i ][1] )

			end

		end
	end

end

-- 
commandarearesearch = function( id, research, tooltipcb )

	--
	local buttons = 
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10",
	}

	local countbuttons  = getn( buttons  )
	local countresearch = getn( research )

	local count

	if countbuttons < countresearch then

		count = countbuttons

	else

		count = countresearch

	end

	for i = 1, count
	do

		if ResearchIsOpen( research[ i ][1] ) == 1 then

			BindButtonToResearch( buttons[ i ], research[ i ][2], "doresearch", tooltipcb, id, research[ i ][1] )

		end

	end

end

-- 
commandareaupgrade = function( id, ebpnetid, upgrade, tooltipcb )

	--
	local buttons = 
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10",
	}

	local countbuttons  = getn( buttons  )
	local countupgrade = getn( upgrade )

	local count

	if countbuttons < countupgrade then

		count = countbuttons

	else

		count = countupgrade

	end

	for i = 1, count
	do

		if CreatureUpgradeIsOpen( ebpnetid, upgrade[ i ][1] ) == 1 then

			BindButtonToCreatureUpgrade( buttons[ i ], upgrade[ i ][2], "docreatureupgrade", tooltipcb, id, ebpnetid, upgrade[ i ][1] )

		end

	end

end

--
tooltipaddon = function( enabled, addon, infotable )

	local id = SelectionId( 0 )

	if enabled == 1 then

		HelpTextAddOnName(id,addon)
		HelpTextAddOnCost(id,addon)

	else

		HelpTextAddOnName(id,addon)
		HelpTextAddOnCost(id,addon)
		HelpTextAddOnPrerequisite(id,addon)

		
	end

	local count = getn( infotable )
	for i = 1, count
	do
		if infotable[i][1] == addon then
			HelpTextShortcut( infotable[i][2] )
			HelpTextTextWithRequirements( infotable[i][3] )
		end
	end

end

 

tooltip_generatoraddon = function( enabled, addon )

	tooltipaddon(enabled,addon,generatoraddons)

end

--
menucontext_valid = function()

	-- context is invalid if nothing is selected
	if ( SelectionCount() == 0 ) then
		return 0
	end

	-- check to see if there is a context to recover
	if ( menucontext[1] == "" ) then
		return 0
	end

	-- no qualifier means the context is always valid
	if menucontext[2] == "" then
		return 1
	end

	-- loop through all the selected entities and return true as soon as one of the
	-- entities is valid
	local count = SelectionCount()
	for i = 0, count - 1
	do

		local id = SelectionId( i )
		dostring( "menucontext_valid_var = " .. menucontext[2] .. "( " .. id .. " )" )
		if ( menucontext_valid_var == 1 ) then
			return 1
		end

	end

	return 0

end

--
menucontext_refresh = function()

	dostring( menucontext[1] )

end

--
menucontext_clear = function()

	menucontext = {"", "", ""}

end

-- 
menucontext_cancelmodal = function()

	if menucontext[3] ~= "" then
	
		dostring( menucontext[3] )

	end

end

--
focusonselection = function()

	FocusOnSelection()

end

--
zoomin = function()

	ZoomCameraMouse( -0.70 )

end

--
zoomout = function()

	ZoomCameraMouse( 1.45 )

end


--
deselectall = function()

	DeSelectAll()

end

--
escapemenu = function()

	PauseMenuShow()

end

--
allymenu = function()

	AllyShow()

end

--
objvmenu = function()
	ObjectivesShow()
end

--
unhotkeygroup = function()
	UnassignFromAllHotkeyGroups()
end

--
preloadall = function()

	PreloadTexture( "ui/ingame/resource_electricity.tga" )
	PreloadTexture( "ui/ingame/resource_scrap.tga" )
	PreloadTexture( "ui/ingame/back.tga" )

	for i = 1, getn(henchman_commands)
	do

		PreloadTexture( henchman_commands[ i ][ 4 ] )

	end

	for i = 1, getn(creature_commands)
	do

		PreloadTexture( creature_commands[ i ][ 4 ] )

	end

	for i = 1, getn(ability_commands)
	do

		PreloadTexture( ability_commands[ i ][ 4 ] )

	end

	for i = 1, getn(commands)
	do

		PreloadTexture( commands[ i ][ 4 ] )

	end

end

--
on_initial = function()

	-- preload texture
	preloadall()

	-- create minimap
	CreateMinimap( "minimap" )

	--
	SetPlayerArmyOrder( AO_Rank )

	-- focus on player's lab
	FocusOnEntity( LocalPlayerLabId(), 0, 1 )

	-- Initialize UI Prefs
	LoadUIOptions()

end

--
on_selection = function()

	-- selection has changed, clear visible rally point
	RallyPointHide();

	PatrolPathHide()

	WayPointPathHide()

	-- clear menu context
	menucontext_clear()

	--
	if SelectionCount() == 0 then

		emptyselection()

	else

		if SelectionBelongsToPlayer() == 1 then

			friendlyselection()

		elseif SelectionIsEnemy() == 1 then

			enemyselection()

		else

			worldselection()

		end

	end


end

--
on_refresh = function()

	if ( menucontext_valid() == 1 ) then

		-- refresh the context
		menucontext_refresh()

	else

		-- clear all modal UI context
		menucontext_cancelmodal()

		-- otherwise, just treat this as a fresh selection
		on_selection()

	end

end

--
on_gamestart = function()

	-- announce game mode (only has effects in MP games)
	Announce( "gametype_announce.start", 1.5 )
	
end

--
on_playerwin = function()

	-- announce game mode (only has effects in MP games)
	Announce( "gametype_announce.win", 0.5 )
	
end

--
on_playerlose = function()

	-- announce game mode (only has effects in MP games)
	Announce( "gametype_announce.lose", 0.5 )
	
end

-- stubs to dump the default parameter
selectallunitsonscreen = function()
	SelectAllUnitsOnScreen()
end

selectallhenchmenonscreen = function()
	SelectAllHenchmenOnScreen()
end

selectallhenchmeninworld = function()
	SelectAllHenchmenInWorld()
end

selectnextidlehenchman = function()
	SelectNextIdleHenchman()
end

selectallidlehenchman = function()
	SelectAllIdleHenchman()
end

selectallcombatunitsonscreen = function()
	SelectAllCombatUnitsOnScreen()
end

selectallcombatunitsinworld = function()
	SelectAllCombatUnitsInWorld()
end

selectallgroundcombatunitsonscreen = function()
	SelectAllGroundCombatUnitsOnScreen()
end

selectallgroundcombatunitsinworld = function()
	SelectAllGroundCombatUnitsInWorld()
end

selectallaircombatunitsonscreen = function()
	SelectAllAirCombatUnitsOnScreen()
end

selectallaircombatunitsinworld = function()
	SelectAllAirCombatUnitsInWorld()
end

selectallwatercombatunitsonscreen = function()
	SelectAllWaterCombatUnitsOnScreen()
end

selectallwatercombatunitsinworld = function()
	SelectAllWaterCombatUnitsInWorld()
end

selectnextgroundcombiner = function()
	SelectNextGroundCombiner()
end

selectnextwatercombiner = function()
	SelectNextWaterCombiner()
end

selectnextaircombiner = function()
	SelectNextAirCombiner()
end

selectlab = function()
	SelectLab()
end

selectrex = function()
	SelectRex()
end

selectlucy = function()
	SelectLucy()
end

nextsubselect = function()
	SelectNextSubSelect()
end
