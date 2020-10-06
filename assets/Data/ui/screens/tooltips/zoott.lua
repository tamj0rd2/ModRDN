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
	{ "checkbox_1",				"generic_mouseover",	"", 3750, 0,	0,	 Self_Above },
	{ "checkbox_2",				"generic_mouseover",	"", 3751, 0,	0,	 Self_Above },
	{ "checkbox_3",				"generic_mouseover",	"", 3752, 0, 	0,	 Self_Above },
	{ "checkbox_4",				"generic_mouseover",	"", 3753, 0, 	0,	 Self_Above },
	{ "checkbox_5",				"generic_mouseover",	"", 3754, 0, 	0,	 Self_Above },
	{ "checkbox_6",				"generic_mouseover",	"", 3770, 0, 	0,	 Self_Above },
	{ "ability_combo",			"generic_mouseover",	"abilitiesfilter_placeholder", 3754, 0, 0,	 Point_Above },
	{ "deletecreature_button",		"generic_mouseover",	"",	3756, 0, 0, Self_Above },
	{ "accept",				"statusbar_helptext",	"statusbar_helptext", 0, 3757, 0, Point_Below },
	{ "cancel",				"statusbar_helptext",	"statusbar_helptext", 0, 3758, 0, Point_Below },
	{ "name_sort",				"generic_mouseover",	"",	3759, 0, 0, Self_Above },
	{ "cost1_sort",				"generic_mouseover",	"",	3760, 0, 0, Self_Above },
	{ "rank_sort",				"generic_mouseover",	"",	3761, 0, 0, Self_Above },
	{ "health_sort",			"generic_mouseover",	"",	3762, 0, 0, Self_Above },
	{ "armor_sort",				"generic_mouseover",	"",	3763, 0, 0, Self_Above },
	{ "landspeed_sort",			"generic_mouseover",	"",	3764, 0, 0, Self_Above },
	{ "waterspeed_sort",			"generic_mouseover",	"",	3765, 0, 0, Self_Above },
	{ "sightradius_sort",			"generic_mouseover",	"",	3766, 0, 0, Self_Above },
	{ "size_sort",				"generic_mouseover",	"",	3767, 0, 0, Self_Above },
	{ "damage_sort",			"generic_mouseover",	"",	3768, 0, 0, Self_Above },
	{ "abilities_sort",			"generic_mouseover",	"",	3769, 0, 0, Self_Above },
	{ "screen_mask",			"invisible_tooltip",	"",	3769, 0, 0, Self_Above },
	{ "cost2_sort",				"generic_mouseover",	"",	3771, 0, 0, Self_Above },
	{ "rangedamage_sort",			"generic_mouseover",	"",	3772, 0, 0, Self_Above },
	{ "animalinfo_button",			"generic_mouseover",	"", 	3773, 0, 0, Self_Above },	
	
	{ "electricitysort_column",		"generic_mouseover",	"",	3250, 0, 0, Self_Above },
	{ "coalsort_column",			"generic_mouseover",	"",	3251, 0, 0, Self_Above },
	{ "ranksort_column",				"generic_mouseover",	"",	3252, 0, 0, Self_Above },
	{ "healthsort_column",			"generic_mouseover",	"",	3253, 0, 0, Self_Above },
	{ "defensesort_column",			"generic_mouseover",	"",	3254, 0, 0, Self_Above },
	{ "speedsort_column",			"generic_mouseover",	"",	3255, 0, 0, Self_Above },
	{ "waterspeedsort_column",		"generic_mouseover",	"",	3256, 0, 0, Self_Above },
	{ "sightsort_column",			"generic_mouseover",	"",	3257, 0, 0, Self_Above },
	{ "sizesort_column",			"generic_mouseover",	"",	3258, 0, 0, Self_Above },
	{ "damagesort_column",			"generic_mouseover",	"",	3259, 0, 0, Self_Above },
	{ "rangedamagesort_column",		"generic_mouseover",	"",	3260, 0, 0, Self_Above },
}
