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
	{ "starttutorial_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27401, 0, Point_Below },
	{ "cameracontrol",			"statusbar_helptext",	"statusbar_helptext", 0, 27402, 0, Point_Below },
	{ "unitselection",			"statusbar_helptext",	"statusbar_helptext", 0, 27403, 0, Point_Below },
	{ "resourcegathering",			"statusbar_helptext",	"statusbar_helptext", 0, 27404, 0, Point_Below },
	{ "structurebuilding",			"statusbar_helptext",	"statusbar_helptext", 0, 27405, 0, Point_Below },
	{ "armybuilder",			"statusbar_helptext",	"statusbar_helptext", 0, 27406, 0, Point_Below },
	{ "unitcreation",			"statusbar_helptext",	"statusbar_helptext", 0, 27407, 0, Point_Below },
	{ "puttingittogether",			"statusbar_helptext",	"statusbar_helptext", 0, 27408, 0, Point_Below },
	{ "back",				"statusbar_helptext",	"statusbar_helptext", 0, 27409, 0, Point_Below },

	
}
