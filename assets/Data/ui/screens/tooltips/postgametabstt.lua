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
	{ "scores_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27801, 0, Point_Below },
	{ "compare_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27802, 0, Point_Below },
	{ "combat_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27803, 0, Point_Below },
	{ "resourcestech_button",	"statusbar_helptext",	"statusbar_helptext", 0, 27804, 0, Point_Below },
	{ "unitsarmies_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27805, 0, Point_Below },
	{ "button1",			"statusbar_helptext",	"statusbar_helptext", 0, 27806, 0, Point_Below },
	{ "button2",			"statusbar_helptext",	"statusbar_helptext", 0, 27807, 0, Point_Below },
	{ "button3",			"statusbar_helptext",	"statusbar_helptext", 0, 27808, 0, Point_Below },
	{ "button4",			"statusbar_helptext",	"statusbar_helptext", 0, 27809, 0, Point_Below },
	{ "button5",			"statusbar_helptext",	"statusbar_helptext", 0, 27810, 0, Point_Below },
	{ "button6",			"statusbar_helptext",	"statusbar_helptext", 0, 27811, 0, Point_Below },
	{ "button7",			"statusbar_helptext",	"statusbar_helptext", 0, 27812, 0, Point_Below },
	{ "button8",			"statusbar_helptext",	"statusbar_helptext", 0, 27813, 0, Point_Below },
	{ "button9",			"statusbar_helptext",	"statusbar_helptext", 0, 27814, 0, Point_Below },
	{ "gametime_text",		"statusbar_helptext",	"statusbar_helptext", 0, 27815, 0, Point_Below },
	{ "statlist_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27817, 0, Point_Below },
	{ "tab_1_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27818, 0, Point_Below },
	{ "tab_2_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27819, 0, Point_Below },
	{ "tab_3_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27820, 0, Point_Below },
	{ "tab_4_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27821, 0, Point_Below },
	{ "tab_5_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27822, 0, Point_Below },
	{ "tab_6_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27823, 0, Point_Below },
	{ "tab_7_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27824, 0, Point_Below },

	
}
