---------------------------------------------------------------------
-- File    : taskbar.lua
-- Desc    :
-- Created :
-- Author  :
--
-- (c) 2003 Relic Entertainment Inc.
--

-- * in-game taskbar script

--
-- hotkeys


	-- System
	HK_System_Escape		= "keygroups.systemcommands.keys.escape"
	HK_System_Accept		= "keygroups.systemcommands.keys.accept"
	HK_System_CommandQueue	= "keygroups.systemcommands.keys.commandqueue"
	HK_System_Menu			= "keygroups.systemcommands.keys.showpausemenu"
	HK_System_Chat			= "keygroups.systemcommands.keys.chat"

	-- Camera
	HK_Camera_Focus		= "keygroups.cameracontrol.keys.focus"
	HK_Camera_ZoomCameraIn	= "keygroups.cameracontrolfree.keys.keyzoomin"
	HK_Camera_ZoomCameraOut	= "keygroups.cameracontrolfree.keys.keyzoomout"

	-- HENCHMEN
	HK_Henchman_Build			= "keygroups.basicunitcommands.keys.buildmenu"
	HK_Henchman_BuildAdvanced		= "keygroups.basicunitcommands.keys.buildmenuadvanced"
	HK_Henchman_Repair			= "keygroups.basicunitcommands.keys.repair"
	HK_Henchman_Gyrocopter		= "keygroups.henchmancommands.keys.gyrocopter"
	HK_Henchman_Heal			= "keygroups.henchmancommands.keys.heal"
	HK_Henchman_Gather			= "keygroups.basicunitcommands.keys.gather"
	HK_Henchman_Tag			= "keygroups.henchmancommands.keys.tag"
	HK_Henchman_BuildToggle		= "keygroups.henchmanbuildcommands.keys.buildtoggle"
	HK_Henchman_Unload			= "keygroups.henchmancommands.keys.unload"
	HK_Henchman_Airlift			= "keygroups.henchmancommands.keys.airlift"
	HK_Henchman_Untag			= "keygroups.henchmancommands.keys.untag"

	-- Lab

	HK_Lab_SpawnHenchmen		= "keygroups.hqcommands.keys.spawnhenchmen"

	-- basic unit
	HK_Generic_Stop	= "keygroups.basicunitcommands.keys.stop"
	HK_Generic_Move	= "keygroups.basicunitcommands.keys.move"
	HK_Generic_Kill = "keygroups.basicunitcommands.keys.kill"
	HK_Generic_Attack		= "keygroups.basicunitcommands.keys.attack"
	HK_Generic_Guard		= "keygroups.creaturecommands.keys.guard"
	HK_Generic_Patrol		= "keygroups.creaturecommands.keys.patrol"

	-- selection
	HK_Select_UnitsOnScreen	= "keygroups.select.keys.unitsonscreen"
	HK_Select_UnitsInWorld	= "keygroups.select.keys.unitsinworld"
	HK_Select_Lab			= "keygroups.select.keys.hq"

	-- buttons
	menu_commands =
	{
		{ 40950, HK_System_Menu,			42380, },
		{ 40950, HK_System_Chat,			42380, },
	}

	-- 2: hotkey. 4: tga
	henchman_commands =
	{
		{ 40820,	HK_Generic_Move,			42320,	"ui/ingame/henchmen_move.tga" },
		{ 40821,	HK_Henchman_Gather,			42322,	"ui/ingame/gather.tga" },
		{ 40822,	HK_Generic_Guard,			42323,	"ui/ingame/henchmen_guard.tga" },
		{ 40823,	HK_Henchman_Repair,			42324,	"ui/ingame/repair.tga" },
		{ 40824,	HK_Generic_Stop,			42326,	"ui/ingame/henchmen_stop.tga" },

		{ 40825,	HK_Henchman_Build,			42327,	"ui/ingame/build_structure.tga" },
		{ 40834,	HK_Henchman_BuildAdvanced,	42345,	"ui/ingame/build_structure_advanced.tga" },

		{ 40826,	HK_Henchman_Heal,			42325,	"ui/ingame/heal.tga" },
		{ 40827,	HK_Henchman_Tag,			42332,	"ui/ingame/tag.tga" },
		{ 40829,	HK_Generic_Kill,			42329,	"ui/ingame/kill.tga" },
		{ 40835,	HK_Henchman_Unload,			42346,	"ui/ingame/unload.tga" },
		{ 40856,	HK_Generic_Attack,			42330,	"ui/ingame/henchmen_attack.tga" },
		{ 40836,	HK_Henchman_Airlift,		42348,	"ui/ingame/airlift.tga" },
		{ 40837,	HK_Henchman_Untag,		42349,  "ui/ingame/untag.tga" },
	}

	hq_commands =
	{
		{ 40930,	HK_Lab_SpawnHenchmen,		42370,	"ui/ingame/spawn_rock.tga" },
	}

	henchman_modalmodes =
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_Gather },
		{ MM_Cursor,	MC_Guard },
		{ MM_Cursor,	MC_Repair },
		{ MM_None,		MC_None },				-- stop
		{ MM_Cursor,	MC_BuildStructure },	-- build basic
		{ MM_Cursor,	MC_BuildStructure },	-- build advanced
		{ MM_Cursor,	MC_Heal },
		{ MM_Cursor,	MC_Tag },
		{ MM_None,		MC_None },				-- kill
		{ MM_Cursor,	MC_Unload },			-- unload
		{ MM_Cursor,	MC_AttackMove },
		{ MM_Cursor,	MC_Airlift },
		{ MM_Cursor,	MC_UnTag },
	}

	singleselectinfotable =
	{
		{38410, 38411},		-- healthbar/healthlabel
		{38412, 38413},		-- entity icon
		{38414, 38415},		-- entity damage
		{38416, 38417},		-- entity endurance
		{38418, 38419},		-- entity name
	}

	-- Record the taskbar menu context so that we can refresh the menu without losing
	-- the context.  The menu is made up of 3 parts: the Lua expression to call to
	-- recover the context, and the Lua function to call to ensure that at least one
	-- of the selected entities qualifies for this context, and the command to end
	-- any modal UI state.  The 1st part is the context refresher
	-- and the 2nd part is the context qualifier.
	--
	menucontext = { "", "", "" }

--
cleartaskbar = function()

	Clear()

	--show background labels
	ShowHud( "picturelabel" )

	BindHotkey( HK_System_Escape, "deselectall", 0 )

	BindHotkey( HK_Camera_Focus, "focusonselection", 0 )
	BindHotkey( HK_Camera_ZoomCameraIn, "zoomin", 0 )
	BindHotkey( HK_Camera_ZoomCameraOut, "zoomout", 0 )

	BindLabelToGameTime( "gametime_label" )

	-- if playing the game
	if not( LocalPlayer() == 0 ) then

		-- quick keyboard selection
		BindHotkey       ( HK_Select_UnitsOnScreen, "selectallunitsonscreen", 0 )
		BindHotkey       ( HK_Select_UnitsInWorld, "selectallunitsinworld", 0 )
		BindHotkey       ( HK_Select_Lab, "selecthq", 0 )

		-- player resources

		ShowHud("background1")

		BindLabelToPlayerCash	( "resource_cash_label", "resourceindicatorstooltip", 1, LocalPlayer() )
		BindHudToTooltip		( "resource_cash_icon", "resourceindicatorstooltip", 1, 1 )
		BindLabelToPlayerPop	( "resource_unitcap_label", "resourceindicatorstooltip", 1, LocalPlayer() )
		BindHudToTooltip		( "resource_unitcap_icon", "resourceindicatorstooltip", 1, 1 )

	end

	-- menu button
	BindButton( "menu_button", menu_commands[1][2], "escapemenu",  "menutooltip", "", 1 )

	-- chat
	local chat = ChatAllowed()
	if (chat == CHATALLOW_Ok) or (chat == CHATALLOW_Dead) or (chat == CHATALLOW_COPPA) then
		BindButtonToChat( "chat_button", menu_commands[2][2], "chat", "chattooltip" )
	end

	-- minimap
	BindLabelToTooltip( "minimaptooltip", "minimap_tooltipcb" )

	-- infocenter
	infocenter()

end

--
chat = function( dummy )

	ChatShow()

end

--
chattooltip = function( enabled, index )

	HelpTextTitle( 40952 )
	HelpTextChat ()

end

--
tooltip_command = function( enabled, index, table )

	if index >= 1 and index <= getn( table ) then

	HelpTextTitle		( table[ index ][1] )
	HelpTextShortcut	( table[ index ][2] )
	HelpTextTextWithoutRequirements( table[index][3] )

	end

end

--
resourceindicatorstooltip = function( enabled, index )

	HelpTextTitle( 40750 )
	HelpTextTextWithoutRequirements( 42400 )

end

--
menutooltip = function( enabled, index )

	tooltip_command( enabled, index, menu_commands )

end

--
infocentersinglebasicstats = function( id )

	local ebpid = EntityEBP( id )
	local owner = EntityOwner( id )

	-- basic stats
	BindLabelToEntityName  ( "singlselect_name_label",    id, "singleselectinfotooltip", 5 )
	BindLabelToEntityHealth( "singlselect_statbar_label", id, "singleselectinfotooltip", 1 )
	BindImageToEntityIcon  ( "singlselect_icon", id, "singleselectinfotooltip", 2 )
	BindBarToEntityHealth  ( "singlselect_statbar", id, "singleselectinfotooltip", 1 )

	-- owner
	if EntityBelongsToPlayer( id ) == 0 then

		-- owner
		BindLabelToPlayerName  ( "textlabel_playerinfo1", EntityOwner( id ) )
		BindLabelToPlayerColour( "color_label",           EntityOwner( id ) )

		-- ally/enemy
		if not( LocalPlayer() == 0) then
			if SelectionIsEnemy() == 1 then
				BindLabelToText( "textlabel_infoline02", 40971 )
			elseif SelectionIsAlly() == 1 then
				BindLabelToText( "textlabel_infoline02", 40970 )

			end
		end

	end

end

--
infocenterbuildqueue = function( id, enabled )

	--
	local bqbuttons =
	{
		"buildque_icon00",
		"buildque_icon01",
		"buildque_icon02",
		"buildque_icon03",
		"buildque_icon04",
		"buildque_icon05",
		"buildque_icon06",
		"buildque_icon07"
	}

	local n = getn( bqbuttons )
	local bqlength = BuildQueueLength( id )

	local count = 0

	if( n < bqlength ) then
		count = n
	else
		count = bqlength
	end

	for i = 0, count - 1
	do

		BindButtonToBuildQueue( bqbuttons[ i + 1 ], "", "docancelbuildunit", "", id, i, enabled )

	end

	BindLabelToBuildQueue   ( "buildque_name_label", id, 0 )
	BindLabelToBuildProgress( "buildque_progress_label", id )
	BindBarToBuildQueue     ( "buildque_progress_statbar", id )

end

--
infocenterguy = function( id )
end

--
infocenterhq = function( id )
	if EntityInSpawning( id ) == 1 then
		infocenterbuildqueue( id, 1 )
	end
end

--
infocenterenemy = function()

	if SelectionCount() == 1 then

		local id = SelectionId( 0 )

		local type = EntityType( id )

		if	type == Lab_EC or
			type == Henchmen_EC
		then

			infocentersinglebasicstats( id )

		end

	end

end

--
infocenterfriendly = function()

	if SelectionCount() == 1 then

		-- single
		infocentersingle( SelectionId( 0 ) )

	else

		-- multi
		infocentermulti()

	end

end

--
infocenterworld = function()

	if SelectionCount() == 1 then

		local id = SelectionId( 0 )

		local type = EntityType( id )

		-- basic stats
		infocentersinglebasicstats( id )

	end --

end

--
infocentersingle = function( id )

	-- basic stats
	infocentersinglebasicstats( id )

	-- special states
	if EntityType( id ) == Henchmen_EC then

		infocenterguy( id )

	elseif EntityType( id ) == Lab_EC then

		infocenterhq( id )

	end

end

--
infocenter = function()

	if SelectionCount() == 0 then

		-- display nothing

	else

		-- if playback
		if LocalPlayer() == 0 then

			if SelectionCount() == 1 then

				-- single
				infocentersingle( SelectionId( 0 ) )

			end

		elseif SelectionBelongsToPlayer() == 1 then

			infocenterfriendly()

		elseif SelectionIsEnemy() == 1 or SelectionIsAlly() == 1 then

			infocenterenemy()

		else

			infocenterworld()

		end

	end

end

--
infocentermulti = function()

	-- info center
	local multibuttons =
	{
		{ "multiselect_icon01", "multiselect_statbar01" },
		{ "multiselect_icon02", "multiselect_statbar02" },
		{ "multiselect_icon03", "multiselect_statbar03" },
		{ "multiselect_icon04", "multiselect_statbar04" },
		{ "multiselect_icon05", "multiselect_statbar05" },
		{ "multiselect_icon06", "multiselect_statbar06" },
		{ "multiselect_icon07", "multiselect_statbar07" },
		{ "multiselect_icon08", "multiselect_statbar08" },
		{ "multiselect_icon09", "multiselect_statbar09" },
		{ "multiselect_icon10", "multiselect_statbar10" },
		{ "multiselect_icon11", "multiselect_statbar11" },
		{ "multiselect_icon12", "multiselect_statbar12" },
		{ "multiselect_icon13", "multiselect_statbar13" },
		{ "multiselect_icon14", "multiselect_statbar14" },
		{ "multiselect_icon15", "multiselect_statbar15" },
		{ "multiselect_icon16", "multiselect_statbar16" },
		{ "multiselect_icon17", "multiselect_statbar17" },
		{ "multiselect_icon18", "multiselect_statbar18" },
		{ "multiselect_icon19", "multiselect_statbar19" },
		{ "multiselect_icon20", "multiselect_statbar20" },
		{ "multiselect_icon21", "multiselect_statbar21" },
		{ "multiselect_icon22", "multiselect_statbar22" },
		{ "multiselect_icon23", "multiselect_statbar23" },
		{ "multiselect_icon24", "multiselect_statbar24" },
		{ "multiselect_icon25", "multiselect_statbar25" },
		{ "multiselect_icon26", "multiselect_statbar26" },
		{ "multiselect_icon27", "multiselect_statbar27" },
		{ "multiselect_icon28", "multiselect_statbar28" },
		{ "multiselect_icon29", "multiselect_statbar29" },
		{ "multiselect_icon30", "multiselect_statbar30" },
		{ "multiselect_icon31", "multiselect_statbar31" },
		{ "multiselect_icon32", "multiselect_statbar32" },
		{ "multiselect_icon33", "multiselect_statbar33" },
		{ "multiselect_icon34", "multiselect_statbar34" },
		{ "multiselect_icon35", "multiselect_statbar35" },
		{ "multiselect_icon36", "multiselect_statbar36" },
	}

	local count = SelectionCount()

	if  count > getn( multibuttons ) then

		count = getn( multibuttons )

	end

	for i = 0, count - 1
	do

		local id = SelectionId( i )

		BindButtonToEntity   ( multibuttons[ i + 1 ][ 1 ], "", "selectentity", "", id )
		BindBarToEntityHealth( multibuttons[ i + 1 ][ 2 ], id, "", 0 );

	end

end

--
dummy = function( tmp )
end

minimap_tooltipcb = function( enabled, index )
	HelpTextTitle(39537)
	HelpTextTextWithoutRequirements(39538)
end

--
commandstooltip = function( enabled, index )
end

singleselectinfotooltip = function( index )

	if index >= 1 and index <= getn( singleselectinfotable ) then

		HelpTextTitle( singleselectinfotable[ index ][1] )
		HelpTextTextWithoutRequirements( singleselectinfotable[index][2] )

	end

end


--
tooltip_ebp = function( enabled, ebpid )

	if enabled == 1 then

		HelpTextEBPName(ebpid)
		HelpTextEBPCost(ebpid)

	else

		HelpTextEBPName(ebpid)
		HelpTextEBPCost(ebpid)
		HelpTextEBPPrerequisite(ebpid)

	end

end

--
commandqueuecancel = function()

	domodalcancel()

end

--
commandqueuecancelignore = function()

	-- Empty on purpose

end

-- check to see if an entity can self-destruct
mcqualifier_killconfirm = function( id )

	local type = EntityType( id )

	-- Lab cannot self destruct
	if type == Lab_EC then
		return 0
	end

	return 1

end

--
dokillconfirm = function( index )

	-- register function for refresh calls
	menucontext = { "dokillconfirm( " .. index .. " )", "mcqualifier_killconfirm", "" }

	--
	cleartaskbar()

	ShowHud( "textlabel_kill_unit" )

	-- command area
	BindButton( "command_formation_icon06", HK_Generic_Kill,   "dodestroy",     "", "UI/InGame/accept.tga", 0 )
	BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

end

--
dostop = function( dummy )

	DoCommandStop()

end

--
dodestroy = function( dummy )

	DoDestroy()

end

--
creaturetooltip = function( enabled, index )
end

--
creaturemodalselection = function( id )
end

--
henchmantooltip = function( enabled, index )

	tooltip_command( enabled, index, henchman_commands )

end

--
henchmanselection = function()
	cleartaskbar()

	BindButton( "command_modal_icon01",		henchman_commands[ 1][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 1][4],  1 )		-- move
	BindButton( "command_modal_icon02",		henchman_commands[ 2][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 2][4],  2 )		-- gather
	BindButton( "command_modal_icon03",		henchman_commands[ 3][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 3][4],  3 )		-- guard
	BindButton( "command_modal_icon04",		henchman_commands[ 4][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[ 4][4],  4 )		-- repair
	BindButton( "command_modal_icon05",		henchman_commands[12][2],  "dohenchmanmodal",	"henchmantooltip", henchman_commands[12][4], 12 )		-- attack
	BindButton( "command_modal_icon07",		henchman_commands[ 5][2],  "dostop",			"henchmantooltip", henchman_commands[ 5][4],  5 )		-- stop

end

--
labSelection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- spawn guys
	BindButtonToUnitEBP( "command_modal_icon01", HK_Lab_SpawnHenchmen, "dobuildunit", "commandstooltip", id, HenchmenEBP() )

	-- command area
		-- background
	ShowBitmapLabel( "command_bigicon_back" )

end


--
selectentity = function( id )

	-- check to see if the select similar entity button is pressed
	-- currently this button is 'Shift'
	local actOnSimilar = IsSelectSimilarPressed()

	-- check to see if the select single entity button is being pressed
	-- currently this button is 'Ctrl'
	if (IsSelectSinglePressed() == 1) then
		DeSelectEntity( id, actOnSimilar )
	else
		SelectEntity( id, actOnSimilar )
	end


end

--
friendlyselection = function()

	-- just need one id for each type
	henchmanId			= -1
	buildingId		= -1

	-- check what's in our selection
	for i = 1, SelectionCount()
	do

		local id = SelectionId( i - 1 )

		-- per type stuff
		local type = EntityType( id )

		if type == Henchmen_EC then

			henchmanId = id

		elseif type == Lab_EC then

			-- building
			buildingId = id

		end

	end

	if not (buildingId == -1) then

		labSelection()

	else

		if not (henchmanId == -1) then
			henchmanselection( henchmanId )
			showStance = 0
		end

	end

end

--
enemyselection = function()

	--
	cleartaskbar()

	-- command area
		-- empty

end

--
worldselection = function()

	--
	cleartaskbar()

	-- command area
		-- empty

end

--
emptyselection = function()

	cleartaskbar()

end

--
failedcommand = function( reason )

	local errmsg = {}
		errmsg[ FC_NeedCash			] = { 40800,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_BuildQueueFull	] = { 40802,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_TooManyUnit		] = { 40803,	"audio/ui/AlertShort.pat" }
		errmsg[ FC_Other			] = { 0,		"" }

	BindLabelToTextTimer( "contextarea_line01", errmsg[ reason ][ 1 ], 5 )

	PlaySound( errmsg[ reason ][ 2 ] )

end

--
dobuildunit = function( ebpid )

	local result = DoBuildUnit( ebpid )

	if result == 0 then

		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
docancelconstruction = function( dummy )

	DoCancelConstruction()

end

--
docancelbuildunit = function( unitindex )

	DoCancelBuildUnit( unitindex )

end

--
docancelresearch = function( building )

	DoCancelResearch()

end


--
docancelcreatureupgrade = function( building )

	DoCancelCreatureUpgrade()

end

--
doresearch = function( research )

	local result = DoResearch( research )

	if result == 0 then

		-- success

	else

		-- failed
		failedcommand( result )

	end

end

--
dobuildbuildingcancel = function( dummy )

	-- stop ui
	BuildUIEnd()

	--
	on_selection()

end

--
buildbuilding_updateui = function()

	--
	cleartaskbar()

	-- command area
		-- cancel button
	BindButton( "command_formation_icon07", HK_System_Escape, "dobuildbuildingcancel", "", "UI/InGame/Cancel.tga", 0 )

end


--
domodalclick = function( mode, x, y, z, ebpid )

	-- are we in command queue mode?
	local queue = ModalCommandQueueRequest()

	if queue == 0 then

		-- stop ui (taskbar)
		ModalUIEnd()

	end

	-- send command based on modal mode	(proxy)
	DoModalCommand( mode, x, y, z, ebpid, queue )
end

--
domodalcancel = function( dummy )

	-- stop ui
	ModalUIEnd()

	--
	on_selection()

end

--
menucontext_valid = function()

	-- context is invalid if nothing is selected
	if ( SelectionCount() == 0 ) then
		return 0
	end

	-- check to see if there is a context to recover
	if ( menucontext[1] == "" ) then
		return 0
	end

	-- no qualifier means the context is always valid
	if menucontext[2] == "" then
		return 1
	end

	-- loop through all the selected entities and return true as soon as one of the
	-- entities is valid
	local count = SelectionCount()
	for i = 0, count - 1
	do

		local id = SelectionId( i )
		dostring( "menucontext_valid_var = " .. menucontext[2] .. "( " .. id .. " )" )
		if ( menucontext_valid_var == 1 ) then
			return 1
		end

	end

	return 0

end

--
menucontext_refresh = function()

	dostring( menucontext[1] )

end

--
menucontext_clear = function()

	menucontext = {"", "", ""}

end

--
menucontext_cancelmodal = function()

	if menucontext[3] ~= "" then

		dostring( menucontext[3] )

	end

end

--
focusonselection = function()
	FocusOnSelection()
end

--
zoomin = function()
	ZoomCameraMouse( -0.70 )
end

--
zoomout = function()
	ZoomCameraMouse( 1.45 )
end


--
deselectall = function()
	DeSelectAll()
end

--
escapemenu = function()
	PauseMenuShow()
end

--
unhotkeygroup = function()
	UnassignFromAllHotkeyGroups()
end

--
preloadall = function()

	PreloadTexture( "ui/ingame/resource_cash.tga" )
	PreloadTexture( "ui/ingame/back.tga" )

end

--
on_initial = function()

	-- preload texture
	preloadall()

	-- create minimap (should match the name of UI hud element in igscreen)
	CreateMinimap( "minimap" )

	-- focus on player's lab
	FocusOnEntity( LocalPlayerLabId(), 0, 1 )

	-- Initialize UI Prefs
	LoadUIOptions()

end

--
on_selection = function()

	-- selection has changed, clear visible rally point
	RallyPointHide();

	WayPointPathHide()

	-- clear menu context
	menucontext_clear()

	--
	if SelectionCount() == 0 then

		emptyselection()

	else

		if SelectionBelongsToPlayer() == 1 then

			friendlyselection()

		elseif SelectionIsEnemy() == 1 then

			enemyselection()

		else

			worldselection()

		end

	end


end

--
on_refresh = function()

	if ( menucontext_valid() == 1 ) then

		-- refresh the context
		menucontext_refresh()

	else

		-- clear all modal UI context
		menucontext_cancelmodal()

		-- otherwise, just treat this as a fresh selection
		on_selection()

	end

end

--
on_gamestart = function()
end

--
on_playerwin = function()
end

--
on_playerlose = function()
end

selectallunitsonscreen = function()
	SelectAllUnitsOnScreen()
end

selectallunitsinworld = function()
	SelectAllUnitsInWorld()
end

selecthq = function()
	SelectLab()
end

--
mcqualifier_henchman = function( id )

	local type = EntityType( id )
	if 	(type == Henchman_EC) then
		return 1
	end

	return 0

end

--
dohenchmanmodal = function( index )
	-- register function for refresh calls
	menucontext = { "dohenchmanmodal(" .. index .. ")", "mcqualifier_henchman", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= henchman_modalmodes[ index ][1];
	local command	= henchman_modalmodes[ index ][2];

	-- let henchman modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	-- inplace commands
	local result = 0

	if (command == MC_Unload) then

		result = ModalUIBegin( "domodalclick", "dounloadmodalcancel", mode, command )
	else

		result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )
	end

	if result == 0 then

		--
		cleartaskbar()

		-- command area

		-- TODO: all of this stuff needs implementing
		-- inplace commands
		if (command == MC_Unload) then

			-- Need to refresh passenger icons on the left-hand-side hud
			-- Get selection id's (there should only be one entity selected)
			local count = SelectionCount()
			local id = SelectionId( 0 )

			-- Make sure it is a gyrocopter and only one selection
			local type = EntityType( id )
			if (type == Gyrocopter_EC) and (count == 1) then
				gyrocopterpassengermulti( id );
			end

			local t = henchman_commands[ index ];
			BindButton( "command_normal_icon03", t[2], "dohenchmanunloadnow", "henchmantooltip", t[4], index )

			-- cancel button
			BindButton( "command_formation_icon07", HK_System_Escape, "dounloadmodalcancel", "", "UI/InGame/Cancel.tga", 0 )

		else
			-- cancel button
			BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )
		end

	else

		-- failed
		failedcommand( result )

	end

end

--
dounloadmodalcancel = function( dummy )

	-- clear out pending unload list
	DoCancelPendingUnload()

	-- stop ui
	ModalUIEnd()

	--
	on_selection()

end
