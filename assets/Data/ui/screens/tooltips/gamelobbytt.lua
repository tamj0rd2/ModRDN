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
	{ "hostgame",		"statusbar_helptext",	"statusbar_helptext", 0, 26201, 0, Point_Below },
	{ "joingame",		"statusbar_helptext",	"statusbar_helptext", 0, 26202, 0, Point_Below },
	{ "refresh",		"statusbar_helptext",	"statusbar_helptext", 0, 26203, 0, Point_Below },
	{ "filtersettings",	"statusbar_helptext",	"statusbar_helptext", 0, 26204, 0, Point_Below },
	{ "gamename_sort",	"statusbar_helptext",	"statusbar_helptext", 0, 26205, 0, Point_Below },
	{ "gametype_sort",	"statusbar_helptext",	"statusbar_helptext", 0, 26206, 0, Point_Below },
	{ "back",		"statusbar_helptext",	"statusbar_helptext", 0, 26207, 0, Point_Below },
	{ "ping_sort",		"statusbar_helptext",	"statusbar_helptext", 0, 26208, 0, Point_Below },
	{ "players_sort",	"statusbar_helptext",	"statusbar_helptext", 0, 26209, 0, Point_Below },
	{ "password_sort",	"statusbar_helptext",	"statusbar_helptext", 0, 26210, 0, Point_Below },


	
}
