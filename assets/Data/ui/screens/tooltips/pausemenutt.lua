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
	{ "exittomenu",			"ig_generic_mouseover",	"",	28001, 0,	0,	Self_Above },
	{ "exittowindows",		"ig_generic_mouseover",	"",	28002, 0,	0,	Self_Above },
	{ "continue",			"ig_generic_mouseover",	"",	28003, 0,	0,	Self_Above },
	{ "savemission",		"ig_generic_mouseover",	"",	28004, 0,	0,	Self_Above },
	{ "loadmission",		"ig_generic_mouseover",	"",	28005, 0,	0,	Self_Above },
	{ "restartmission",		"ig_generic_mouseover",	"",	28006, 0,	0,	Self_Above },
	{ "options",			"ig_generic_mouseover",	"",	28007, 0,	0,	Self_Above },
	{ "modoptions_button",		"ig_generic_mouseover",	"",	28008, 0,	0,	Self_Above },

	
}
