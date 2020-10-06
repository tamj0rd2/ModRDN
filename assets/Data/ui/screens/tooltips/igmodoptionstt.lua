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
	{ "gamespeed_label",				"ig_generic_mouseover",	"", 25327, 0,	0,	 Self_Above },
	{ "mousescrollrate_label",			"ig_generic_mouseover",	"", 25328, 0,	0,	 Self_Above },
	{ "keyscrollrate_label",			"ig_generic_mouseover",	"", 25329, 0,	0,	 Self_Above },
	{ "gamespeed_statbar",				"ig_generic_mouseover",	"", 25327, 0,	0,	 Self_Above },
	{ "mousescroll_statbar",			"ig_generic_mouseover",	"", 25328, 0,	0,	 Self_Above },
	{ "keyscroll_statbar",				"ig_generic_mouseover",	"", 25329, 0,	0,	 Self_Above },

	{ "buildingsounds_toggle",			"ig_generic_mouseover",	"", 25330, 0,	0,	 Self_Above },
	{ "eventcuesounds_toggle",			"ig_generic_mouseover",	"", 25331, 0,	0,	 Self_Above },
	{ "unitresponses_toggle",			"ig_generic_mouseover",	"", 25332, 0,	0,	 Self_Above },
	{ "unitconfirmations_toggle",			"ig_generic_mouseover",	"", 25333, 0,	0,	 Self_Above },
	{ "invertdec_toggle",				"ig_generic_mouseover",	"", 25334, 0,	0,	 Self_Above },
	
	{ "maprotation_toggle",				"ig_generic_mouseover",	"", 25335, 0,	0,	 Self_Above },
	{ "mapzoom_toggle",				"ig_generic_mouseover",	"", 25336, 0,	0,	 Self_Above },
	{ "mapscroll_toggle",				"ig_generic_mouseover",	"", 25337, 0,	0,	 Self_Above },
	{ "camerarotation_toggle",			"ig_generic_mouseover",	"", 25338, 0,	0,	 Self_Above },
	{ "cameradeclination_toggle",			"ig_generic_mouseover",	"", 25339, 0,	0,	 Self_Above },

	{ "placeholder_hotkeygroups",			"ig_generic_mouseover",	"", 25340, 0,	0,	 Self_Above },
	{ "placeholder_keybinding",			"ig_generic_mouseover",	"", 25341, 0,	0,	 Self_Above },
	{ "resetall_button",				"ig_generic_mouseover",	"", 25342, 0,	0,	 Self_CenteredAbove },
	{ "assignkey_button",				"ig_generic_mouseover",	"", 25343, 0,	0,	 Self_CenteredAbove },
	{ "resetdefaults_button0",			"ig_generic_mouseover",	"", 25344, 0,	0,	 Self_CenteredAbove },
	{ "done_button",				"ig_generic_mouseover",	"", 25345, 0,	0,	 Self_CenteredAbove },

	{ "invertpan_toggle",				"ig_generic_mouseover",	"", 25346, 0,	0,	 Self_Above },
	
}
