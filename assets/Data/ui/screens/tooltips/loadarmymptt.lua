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

	-- character armies
	{ "creature1_icon1",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon2",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon3",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon4",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon5",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon6",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon7",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon8",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature1_icon9",			"creatureinfo_mouseover",	"prebuiltarmy_placeholder",	0, 0, 0,	Point_Below },	
	-- custom armies
	{ "creature2_icon1",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon2",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon3",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon4",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon5",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon6",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon7",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon8",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },
	{ "creature2_icon9",			"creatureinfo_mouseover",	"customarmy_placeholder",	0, 0, 0,	Point_Below },

	{ "random_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28157, 0, Point_Below },
	{ "rex_button",				"statusbar_helptext",	"statusbar_helptext", 0, 28150, 0, Point_Below },
	{ "lucy_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28151, 0, Point_Below },
	{ "whitey_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28152, 0, Point_Below },
	{ "velika_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28153, 0, Point_Below },
	{ "ganglion_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28154, 0, Point_Below },
	{ "julius_button",			"statusbar_helptext",	"statusbar_helptext", 0, 28155, 0, Point_Below },
	{ "customarmies_button",		"statusbar_helptext",	"statusbar_helptext", 0, 28156, 0, Point_Below },
	{ "accept",				"statusbar_helptext",	"statusbar_helptext", 0, 28158, 0, Point_Below },
	{ "cancel",				"statusbar_helptext",	"statusbar_helptext", 0, 28159, 0, Point_Below },
}
