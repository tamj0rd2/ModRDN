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
	{ "musicvol_statbar",				"ig_generic_mouseover",	"", 25315, 0,	0,	 Self_Above },
	{ "speechvol_statbar",				"ig_generic_mouseover",	"", 25316, 0,	0,	 Self_Above },
	{ "sfxvol_statbar",				"ig_generic_mouseover",	"", 25317, 0,	0,	 Self_Above },
	{ "preferredframerate_statbar",			"ig_generic_mouseover",	"", 25319, 0,	0,	 Self_Above },
	{ "gamma_statbar",				"ig_generic_mouseover",	"", 25320, 0,	0,	 Self_Above },
	{ "mindetaillevel_statbar",			"ig_generic_mouseover",	"", 25321, 0,	0,	 Self_Above },
	{ "terraindetaillevel_statbar",			"ig_generic_mouseover",	"", 25322, 0,	0,	 Self_Above },
	{ "shadowdetail_combo",				"ig_generic_mouseover",	"", 25323, 0,	0,	 Self_Above },
	{ "done_button",				"ig_generic_mouseover",	"", 25324, 0,	0,	 Self_CenteredAbove },
	{ "musicvol_label",				"ig_generic_mouseover",	"", 25315, 0,	0,	 Self_Above },
	{ "speechvol_label",				"ig_generic_mouseover",	"", 25316, 0,	0,	 Self_Above },
	{ "sfxvol_label",				"ig_generic_mouseover",	"", 25317, 0,	0,	 Self_Above },
	{ "preferredframerate_label",			"ig_generic_mouseover",	"", 25319, 0,	0,	 Self_Above },
	{ "gamma_label",				"ig_generic_mouseover",	"", 25320, 0,	0,	 Self_Above },
	{ "mindetaillevel_label",			"ig_generic_mouseover",	"", 25321, 0,	0,	 Self_Above },
	{ "mindetaillevel_label0",			"ig_generic_mouseover",	"", 25322, 0,	0,	 Self_Above },

	
}
