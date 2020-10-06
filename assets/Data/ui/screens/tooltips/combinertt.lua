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
	{ "setinput0",				"generic_mouseover",	"",	 3500, 0, 0,	 Self_Below },
	{ "setinput1",				"generic_mouseover",	"",	 3501, 0, 0,	 Self_Below },
	{ "loadfromzoo_button",			"statusbar_helptext",	"statusbar_helptext", 0, 3502, 0, Point_Below },
	{ "addtoarmy_button",			"statusbar_helptext",	"statusbar_helptext", 0, 3503, 0, Point_Below },
	{ "cancel_button",			"statusbar_helptext",	"statusbar_helptext", 0, 3504, 0, Point_Below },
	{ "creaturename_edittext",		"statusbar_helptext",	"statusbar_helptext", 0, 3506, 0, Point_Below },
	{ "picture",				"generic_mouseover",	"",	 3508, 0, 0,	Self_Right },
	{ "inputcombobox0",			"generic_mouseover",	"",	 3509, 0, 0,	 Self_Above },
	{ "inputcombobox1",			"generic_mouseover",	"",	 3509, 0, 0,	 Self_Above },
	{ "result",				"statusbar_helptext",	"statusbar_helptext", 0, 3510, 0, Point_Below },
	
	{ "swaphead_input0",			"generic_mouseover",	"",	3550, 0, 0, Self_Left },
	{ "swaphead_input1",			"generic_mouseover",	"",	3550, 0, 0, Self_Right },
	{ "swapfrontlegs_input0",		"generic_mouseover",	"",	3551, 0, 0, Self_Left },
	{ "swapfrontlegs_input1",		"generic_mouseover",	"",	3551, 0, 0, Self_Right },
	{ "swaptorso_input0",			"generic_mouseover",	"",	3552, 0, 0, Self_Left },
	{ "swaptorso_input1",			"generic_mouseover",	"",	3552, 0, 0, Self_Right },
	{ "swapbacklegs_input0",		"generic_mouseover",	"",	3553, 0, 0, Self_Left },
	{ "swapbacklegs_input1",		"generic_mouseover",	"",	3553, 0, 0, Self_Right },
	{ "swaptail_input0",			"generic_mouseover",	"",	3554, 0, 0, Self_Left },
	{ "swaptail_input1",			"generic_mouseover",	"",	3554, 0, 0, Self_Right },
	{ "swapwings_input0",			"generic_mouseover",	"",	3555, 0, 0, Self_Left },
	{ "swapwings_input1",			"generic_mouseover",	"",	3555, 0, 0, Self_Right },
	{ "swapclaws_input0",			"generic_mouseover",	"",	3556, 0, 0, Self_Left },
	{ "swapclaws_input1",			"generic_mouseover",	"",	3556, 0, 0, Self_Right },

	{ "bottom_tutorial_back",		"blank_tooltip",	"",	0, 0,	0,	Self_Left },
	{ "top_tutorial_back",			"blank_tooltip",	"",	0, 0,	0,	Self_Left },

	{ "picturelock_checkbox",		"statusbar_helptext",	"statusbar_helptext", 0, 3557, 0, Point_Below },

}
