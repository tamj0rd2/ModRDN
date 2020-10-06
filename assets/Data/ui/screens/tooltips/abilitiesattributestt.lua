-- tooltip modes

-- tooltip is generated around the hud that generates it
Self_Above			= 65
Self_Below			= 66
Self_Left			= 68
Self_Right			= 72

Self_CenteredAbove	= 81
Self_CenteredBelow	= 82
Self_CenteredLeft	= 84
Self_CenteredRight	= 88

-- tooltip is generated around the pos of the spawn point hud
Point_Above			= 33
Point_Below			= 34
Point_Left			= 36
Point_Right			= 40

Point_CenteredAbove	= 49
Point_CenteredBelow	= 50
Point_CenteredLeft	= 52
Point_CenteredRight	= 56

tooltips = 
{
	-- each entry consists of 
	--  name of the hud to apply a tooltip
	--  name of the hud to use as a tooltip
	--  name of the hud to extract the position of a spawn point for tooltips with the appropriate mode set
	--  id of the text to use for title/single-line
	--  id of the text to use for body 
	--  0/1 to auto-resize
	--  tooltip mode, See choices above 
	{ "camera_icon",			"generic_mouseover",	"",	4251, 0,	0,	Self_CenteredBelow },
	{ "meleedamage_staticon",		"generic_mouseover",	"",	4252, 0,	0,	Self_Above },
	{ "resource_electricity_icon",		"ability_tooltip",	"",	4255, 4256,	0,	Self_Above },
	{ "electricity_number",			"ability_tooltip",	"",	4255, 4256,	0,	Self_Above },
	{ "resource_scrap_icon",		"ability_tooltip",	"",	4257, 4258,	0,	Self_Above },
	{ "scrap_number",			"ability_tooltip",	"",	4257, 4258,	0,	Self_Above },
	{ "health_label",			"generic_mouseover",	"",	4262, 0,	0,	Self_Above },
	{ "armor_label",			"generic_mouseover",	"",	4264, 0,	0,	Self_Above },
	{ "speed_label",			"generic_mouseover",	"",	4266, 0,	0,	Self_Above },
	{ "sightradius_label",			"generic_mouseover",	"",	4268, 0,	0,	Self_Above },
	{ "size_label",				"generic_mouseover",	"",	4270, 0,	0,	Self_Above },
	{ "waterspeed_number",			"generic_mouseover",	"",	4343, 0,	0,	Self_Left },
	{ "purewaterspeed_number",		"generic_mouseover",	"",	4343, 0,	0,	Self_Left },
	{ "rank_icon",				"ability_tooltip",	"",	4273, 4274,	0, 	Self_Above },
	{ "rangedamage_label",			"ability_tooltip",	"attackstooltip_placeholder",	4275, 4276,0,	Point_Above },
	{ "meleedamage_label",			"ability_tooltip",	"attackstooltip_placeholder",	4277, 4278, 0,	Point_Above },
	{ "electricity_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4330, 0,	Point_Below },
	{ "electricity_plus",			"statusbar_helptext",	"statusbar_helptext",	0, 4330, 0,	Point_Below },
	{ "scrap_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4331, 0,	Point_Below },
	{ "scrap_plus",				"statusbar_helptext",	"statusbar_helptext",	0, 4331, 0,	Point_Below },
	{ "rank_minus"	,			"statusbar_helptext",	"statusbar_helptext",	0, 4332, 0,	Point_Below },
	{ "rank_plus"	,			"statusbar_helptext",	"statusbar_helptext",	0, 4332, 0,	Point_Below },
	{ "meleedamage_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4333, 0,	Point_Below },
	{ "meleedamage_plus",			"statusbar_helptext",	"statusbar_helptext",	0, 4333, 0,	Point_Below },
	{ "health_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4334, 0,	Point_Below },
	{ "health_plus",			"statusbar_helptext",	"statusbar_helptext",	0, 4334, 0,	Point_Below },
	{ "armor_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4335, 0,	Point_Below },
	{ "armor_plus",				"statusbar_helptext",	"statusbar_helptext",	0, 4335, 0,	Point_Below },
	{ "speed_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4336, 0,	Point_Below },
	{ "speed_plus",				"statusbar_helptext",	"statusbar_helptext",	0, 4336, 0,	Point_Below },
	{ "waterspeed_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4337, 0,	Point_Below },
	{ "waterspeed_plus",			"statusbar_helptext",	"statusbar_helptext",	0, 4337, 0,	Point_Below },
	{ "sightradius_minus",			"statusbar_helptext",	"statusbar_helptext",	0, 4338, 0,	Point_Below },
	{ "sightradius_plus",			"statusbar_helptext",	"statusbar_helptext",	0, 4338, 0,	Point_Below },
	{ "size_minus",				"statusbar_helptext",	"statusbar_helptext",	0, 4339, 0,	Point_Below },
	{ "size_plus",				"statusbar_helptext",	"statusbar_helptext",	0, 4339, 0,	Point_Below },
	{ "size_plus",				"statusbar_helptext",	"statusbar_helptext",	0, 4339, 0,	Point_Below },
	{ "requires_nothing",			"generic_mouseover",	"",	4340, 0,	0,	Self_Above },
	{ "requires_aviary",			"generic_mouseover",	"",	4341, 0,	0,	Self_Above },
	{ "requires_waterchamber",		"generic_mouseover",	"",	4342, 0,	0,	Self_Above },
	{ "rangeattack_slot0",			"statusbar_helptext",	"statusbar_helptext",	0, 4280, 0,	Point_Below },
	{ "rangeattack_slot1",			"statusbar_helptext",	"statusbar_helptext",	0, 4280, 0,	Point_Below },	
	{ "picture_icon",			"ability_tooltip",	"",	4281, 4282, 	0,	Self_CenteredAbove },
	{ "creaturename_label",			"generic_mouseover",	"",	4283, 0,	0,	Self_CenteredAbove },
	{ "movementtype_label",			"ability_tooltip",	"",	4284, 4285,	0,	Self_Above },
	{ "speed_staticon",			"generic_mouseover",	"",	4302, 0,	0,	Self_Left },
	{ "health_staticon",			"generic_mouseover",	"",	4300, 0,	0,	Self_Left },
	{ "armor_staticon",			"generic_mouseover",	"",	4301, 0,	0,	Self_Left },
	{ "sightradius_staticon",		"generic_mouseover",	"",	4303, 0,	0,	Self_Left },
	{ "size_staticon",			"generic_mouseover",	"",	4304, 0,	0,	Self_Left },
	{ "health_number",			"generic_mouseover",	"",	4300, 0,	0,	Self_Left },
	{ "armor_number",			"generic_mouseover",	"",	4301, 0,	0,	Self_Left },
	{ "sightradius_number",			"generic_mouseover",	"",	4303, 0,	0,	Self_Left },
	{ "size_number", 			"generic_mouseover",	"",	4304, 0,	0,	Self_Left },
	{ "airspeed_number",			"generic_mouseover",	"",	4344, 0,	0,	Self_Left },
	{ "landspeed_number",			"generic_mouseover",	"",	4345, 0,	0,	Self_Left },
	
}
