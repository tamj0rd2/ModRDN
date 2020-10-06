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
	{ "newarmybutton",			"generic_mouseover",	"",	3000, 0,	0,	Self_Left },
	{ "loadarmybutton",			"generic_mouseover",	"",	3001, 0,	0,	Self_Left },
	{ "savearmybutton",			"generic_mouseover",	"",	3002, 0,	0,	Self_Left },
	{ "buildcreaturebutton",		"generic_mouseover",	"",	3003, 0,	0,	Self_Right },
	{ "add",				"generic_mouseover",	"",	3004, 0,	0,	Self_Right },
	{ "remove",				"generic_mouseover",	"",	3005, 0,	0,	Self_Right },
	{ "emptyslot0",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot1",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot2",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot3",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot4",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot5",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot6",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot7",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "emptyslot8",				"generic_mouseover",	"",	3007, 0,	0,	Self_Below },
	{ "armynamelabel",			"generic_mouseover",	"",	3008, 0,	0,	Self_Below },
	{ "analyzearmy_button",			"generic_mouseover",	"",	3009, 0,	0,	Self_Left },
	{ "creatureinfo_button",		"generic_mouseover",	"",	3010, 0,	0,	Self_Right },
	{ "back",				"statusbar_helptext",	"statusbar_helptext", 0, 3011, 0, Point_Below },
	{ "tutorial_button",			"generic_mouseover",	"",	3014, 0,	0,	Self_Left },
	{ "armybuilder_tutorial_back",			"blank_tooltip",	"",	0, 0,	0,	Self_Left },
	
}
