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
	{ "cost_electricity_label",		"generic_mouseover",	"",	3250, 0, 0, Self_Above },
	{ "cost_scrap_label",			"generic_mouseover",	"",	3251, 0, 0, Self_Above },
	{ "rank_icon",				"generic_mouseover",	"",	3252, 0, 0, Self_Above },
	{ "health_staticon",			"generic_mouseover",	"",	3253, 0, 0, Self_Above },
	{ "armor_staticon",			"generic_mouseover",	"",	3254, 0, 0, Self_Above },
	{ "speed_staticon",			"generic_mouseover",	"",	3255, 0, 0, Self_Above },
	{ "waterspeed_staticon",		"generic_mouseover",	"",	3256, 0, 0, Self_Above },
	{ "sightradius_staticon",		"generic_mouseover",	"",	3257, 0, 0, Self_Above },
	{ "size_staticon",			"generic_mouseover",	"",	3258, 0, 0, Self_Above },
	{ "meleedamage_staticon",		"generic_mouseover",	"",	3259, 0, 0, Self_Above },
	{ "rangedamage_staticon",		"generic_mouseover",	"",	3260, 0, 0, Self_Above },
}