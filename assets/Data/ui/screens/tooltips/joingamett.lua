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
	{ "host_startgame",			"statusbar_helptext",	"statusbar_helptext", 0, 26801, 0, Point_Below },
	{ "recordgame_checkbox",	"statusbar_helptext",	"statusbar_helptext", 0, 26802, 0, Point_Below },
	{ "playername_header",	"statusbar_helptext",	"statusbar_helptext", 0, 26803, 0, Point_Below },
	{ "team_header",	"statusbar_helptext",	"statusbar_helptext", 0, 26805, 0, Point_Below },
	{ "color_header",	"statusbar_helptext",	"statusbar_helptext", 0, 26806, 0, Point_Below },
	{ "ping_header",	"statusbar_helptext",	"statusbar_helptext", 0, 26807, 0, Point_Below },
	{ "status_header",	"statusbar_helptext",	"statusbar_helptext", 0, 26808, 0, Point_Below },
	{ "armyname__header",	"statusbar_helptext",	"statusbar_helptext", 0, 26810, 0, Point_Below },
	{ "back",			"statusbar_helptext",	"statusbar_helptext", 0, 26809, 0, Point_Below },
	{ "gamename_header",			"statusbar_helptext",	"statusbar_helptext", 0, 26811, 0, Point_Below },
	{ "game_banner",			"statusbar_helptext",	"statusbar_helptext", 0, 26811, 0, Point_Below },
	{ "roomname_header",			"statusbar_helptext",	"statusbar_helptext", 0, 26812, 0, Point_Below },
	{ "room_banner",			"statusbar_helptext",	"statusbar_helptext", 0, 26812, 0, Point_Below },
	

	
}
