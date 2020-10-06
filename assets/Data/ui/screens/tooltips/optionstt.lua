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
	{ "playersetuptab",		"statusbar_helptext",	"statusbar_helptext", 0, 27601, 0, Point_Below },
		{ "playersetuptab_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27601, 0, Point_Below },
		{ "usernames_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27603, 0, Point_Below },
		{ "deleteprofile",			"statusbar_helptext",	"statusbar_helptext", 0, 27604, 0, Point_Below },
		{ "createprofile",			"statusbar_helptext",	"statusbar_helptext", 0, 27602, 0, Point_Below },
		{ "back",				"statusbar_helptext",	"statusbar_helptext", 0, 27648, 0, Point_Below },
		{ "difficultylevel_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27651, 0, Point_Below },

	{ "controlstab",			"statusbar_helptext",	"statusbar_helptext", 0, 27606, 0, Point_Below },
		{ "keybindings_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27607, 0, Point_Below },
		{ "mousesensitivity_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27608, 0, Point_Below },
		{ "resetdefault_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27609, 0, Point_Below },
		{ "invertmouse_combo",				"statusbar_helptext",	"statusbar_helptext", 0, 27610, 0, Point_Below },

	{ "gameplaytab",		"statusbar_helptext",	"statusbar_helptext", 0, 27611, 0, Point_Below },
		{ "keyscrollrate_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27612, 0, Point_Below },
		{ "mousescrollrate_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27613, 0, Point_Below },
		{ "gamespeed_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27614, 0, Point_Below },
		{ "autosave_combo",			"statusbar_helptext",	"statusbar_helptext", 0, 27615, 0, Point_Below },
		{ "interfacetips_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27616, 0, Point_Below },

	{ "audiotab",			"statusbar_helptext",	"statusbar_helptext", 0, 27618, 0, Point_Below },
		{ "sound_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27619, 0, Point_Below },
		{ "voicechat_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27620, 0, Point_Below },
		{ "unitresponses_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27621, 0, Point_Below },
		{ "unitconfirmations_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27622, 0, Point_Below },
		{ "buildingsounds_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27623, 0, Point_Below },
		{ "subtitles_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27624, 0, Point_Below },
		{ "musicvol_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27625, 0, Point_Below },
		{ "speechvol_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27626, 0, Point_Below },
		{ "sfxvol_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27627, 0, Point_Below },
		{ "sfxchannels_label",			"statusbar_helptext",	"statusbar_helptext", 0, 27628, 0, Point_Below },
		{ "sfxquality_combo",			"statusbar_helptext",	"statusbar_helptext", 0, 27629, 0, Point_Below },
		{ "audiodriver_combo",			"statusbar_helptext",	"statusbar_helptext", 0, 27630, 0, Point_Below },
		{ "speakersettings_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27631, 0, Point_Below },
		{ "resetdefault_button3",		"statusbar_helptext",	"statusbar_helptext", 0, 27617, 0, Point_Below },
		{ "musicvol_statbar",			"statusbar_helptext",	"statusbar_helptext", 0, 27625, 0, Point_Below },
		{ "speechvol_statbar",			"statusbar_helptext",	"statusbar_helptext", 0, 27626, 0, Point_Below },
		{ "sfxvol_statbar",			"statusbar_helptext",	"statusbar_helptext", 0, 27627, 0, Point_Below },
		{ "sfxchannels_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27628, 0, Point_Below },
		{ "ambientsounds_toggle",		"statusbar_helptext",	"statusbar_helptext", 0, 27652, 0, Point_Below },
		{ "audiotab_button",			"statusbar_helptext",	"statusbar_helptext", 0, 27618, 0, Point_Below },

		{ "graphicstab",		"statusbar_helptext",	"statusbar_helptext", 0, 27632, 0, Point_Below },
		{ "lowend_checkbox",		"statusbar_helptext",	"statusbar_helptext", 0, 27633, 0, Point_Below },
		{ "medium_checkbox",		"statusbar_helptext",	"statusbar_helptext", 0, 27634, 0, Point_Below },
		{ "highend_checkbox",		"statusbar_helptext",	"statusbar_helptext", 0, 27635, 0, Point_Below },
		{ "custom_checkbox",		"statusbar_helptext",	"statusbar_helptext", 0, 27636, 0, Point_Below },
		{ "resolution_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27637, 0, Point_Below },
		{ "colordepth_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27638, 0, Point_Below },
		{ "renderingsystem_combo",	"statusbar_helptext",	"statusbar_helptext", 0, 27639, 0, Point_Below },
		{ "texturedetail_label",	"statusbar_helptext",	"statusbar_helptext", 0, 27640, 0, Point_Below },
			{ "texturedetail_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27640, 0, Point_Below },
		{ "texturedepth_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27641, 0, Point_Below },
		{ "antialiasing_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27642, 0, Point_Below },
		{ "gamma_label",		"statusbar_helptext",	"statusbar_helptext", 0, 27643, 0, Point_Below },
			{ "gamma_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27643, 0, Point_Below },
		{ "mindetaillevel_label",	"statusbar_helptext",	"statusbar_helptext", 0, 27644, 0, Point_Below },
			{ "mindetaillevel_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27644, 0, Point_Below },
		{ "maxdetaillevel_label",	"statusbar_helptext",	"statusbar_helptext", 0, 27645, 0, Point_Below },
			{ "maxdetaillevel_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27645, 0, Point_Below },
		{ "preferredframerate_label",	"statusbar_helptext",	"statusbar_helptext", 0, 27646, 0, Point_Below },
			{ "preferredframerate_statbar",		"statusbar_helptext",	"statusbar_helptext", 0, 27646, 0, Point_Below },
		{ "apply_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27647, 0, Point_Below },
		{ "shadowdetail_combo",		"statusbar_helptext",	"statusbar_helptext", 0, 27649, 0, Point_Below },
		{ "shinyobjects_toggle",	"statusbar_helptext",	"statusbar_helptext", 0, 27650, 0, Point_Below },
		{ "graphicstab_button",		"statusbar_helptext",	"statusbar_helptext", 0, 27632, 0, Point_Below },

	{ "resetdefault_button",	"statusbar_helptext",	"statusbar_helptext", 0, 27617, 0, Point_Below },
	{ "back",			"statusbar_helptext",	"statusbar_helptext", 0, 27800, 0, Point_Below },


	
}
