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
	{ "roomname_header",				"statusbar_helptext",	"statusbar_helptext", 0, 31851, 0, Point_Below },
	{ "players_header",				"statusbar_helptext",	"statusbar_helptext", 0, 31852, 0, Point_Below },
	{ "rooms_header",				"statusbar_helptext",	"statusbar_helptext", 0, 31855, 0, Point_Below },
	{ "playerstats_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31853, 0, Point_Below },
	{ "joinroom_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31856, 0, Point_Below },
	{ "createroom_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31857, 0, Point_Below },
	{ "addfriend_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31858, 0, Point_Below },

	{ "hostgame_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31859, 0, Point_Below },
	{ "mainlobby_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31860, 0, Point_Below },
	{ "friends_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31861, 0, Point_Below },
	{ "news_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31862, 0, Point_Below },

	{ "playersearch_header",			"statusbar_helptext",	"statusbar_helptext", 0, 31863, 0, Point_Below },
	{ "username_edittext",				"statusbar_helptext",	"statusbar_helptext", 0, 31863, 0, Point_Below },
	{ "searchresults_header",			"statusbar_helptext",	"statusbar_helptext", 0, 31864, 0, Point_Below },
	{ "removefriend_button",			"statusbar_helptext",	"statusbar_helptext", 0, 31865, 0, Point_Below },
	{ "playerstats_button1",			"statusbar_helptext",	"statusbar_helptext", 0, 31853, 0, Point_Below },
	{ "playerstats_button2",			"statusbar_helptext",	"statusbar_helptext", 0, 31853, 0, Point_Below },
	{ "playername_header",				"statusbar_helptext",	"statusbar_helptext", 0, 31866, 0, Point_Below },
	{ "friends_header",				"statusbar_helptext",	"statusbar_helptext", 0, 31867, 0, Point_Below },

	{ "squelch_button",				"statusbar_helptext",	"statusbar_helptext", 0, 31868, 0, Point_Below },


}