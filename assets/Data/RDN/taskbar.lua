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

	-- Guy
	HK_Guy_Stop			= "keygroups.guycommands.keys.stop"
	HK_Guy_Move			= "keygroups.guycommands.keys.move"
	HK_Guy_Attack			= "keygroups.guycommands.keys.attack"
	HK_Guy_Kill				= "keygroups.guycommands.keys.kill"

	-- Lab
	HK_Lab_SpawnRock		= "keygroups.hqcommands.keys.spawnrock"
	HK_Lab_SpawnPaper		= "keygroups.hqcommands.keys.spawnpaper"
	HK_Lab_SpawnScissor		= "keygroups.hqcommands.keys.spawnscissor"

	-- selection
	HK_Select_UnitsOnScreen	= "keygroups.select.keys.unitsonscreen"
	HK_Select_UnitsInWorld	= "keygroups.select.keys.unitsinworld"
	HK_Select_Lab			= "keygroups.select.keys.hq"

	-- guy buttons
	guy_buttons =
	{
		"spawn_rock",
		"spawn_paper",
		"spawn_scissors",
	}

	-- buttons
	menu_commands =
	{
		{ 40950, HK_System_Menu,			42380, },
		{ 40950, HK_System_Chat,			42380, },
	}

	guy_commands =
	{
		{ 40800,	HK_Guy_Stop,			40800,	"ui/ingame/henchmen_stop.tga" },
		{ 40800,	HK_Guy_Move,			40800,	"ui/ingame/henchmen_move.tga" },
		{ 40800,	HK_Guy_Attack,			40800,	"ui/ingame/henchmen_attack.tga" },
		{ 40800,	HK_Guy_Kill,			40800,	"ui/ingame/kill.tga"	},
	}

	hq_commands =
	{
		{ 40930,	HK_Lab_SpawnRock,		42370,	"ui/ingame/spawn_rock.tga" },
		{ 40931,	HK_Lab_SpawnPaper,		42336,	"ui/ingame/spawn_paper.tga" },
		{ 40932,	HK_Lab_SpawnScissor,		42337,	"ui/ingame/spawn_scissor.tga" },
	}

	guy_modalmodes =
	{
		{ MM_Cursor,	MC_Move },
		{ MM_Cursor,	MC_AttackMove },
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
guyselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

end

--
hqselection = function()

	local id = SelectionId( 0 )

	--
	cleartaskbar()

	-- spawn guys
	BindButtonToUnitEBP( "command_big_icon01", HK_Lab_SpawnRock, "dobuildunit", "commandstooltip", id, RockEBP() )
	BindButtonToUnitEBP( "command_big_icon02", HK_Lab_SpawnPaper, "dobuildunit", "commandstooltip", id, PaperEBP() )
	BindButtonToUnitEBP( "command_big_icon03", HK_Lab_SpawnScissor, "dobuildunit", "commandstooltip", id, ScissorEBP() )

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

	--
	cleartaskbar()

	-- just need one id for each type
	guyId			= -1
	buildingId		= -1

	-- check what's in our selection
	for i = 1, SelectionCount()
	do

		local id = SelectionId( i - 1 )

		-- per type stuff
		local type = EntityType( id )

		if type == Henchmen_EC then

			-- guy
			guyId = id

		elseif type == Lab_EC then

			-- building
			buildingId = id

		end

	end

	if not (buildingId == -1) then

		hqselection()

	else

		guyselection( creatureId )

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
dospawnmodal = function( index )

	-- register function for refresh calls
	menucontext = { "dospawnmodal(" .. index .. ")", "", "ModalUIEnd()" }

	-- translate mode in game usable mode (only one here)
	local mode		= MM_LockCursor
	local command	= MC_SetRallyPoint

	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )

	end

end

--
docreaturemodal = function( index )

	-- register function for refresh calls
	menucontext = { "docreaturemodal(" .. index .. ")", "mcqualifier_creaturemodal", "ModalUIEnd()" }

	-- translate mode in game usable mode
	local mode		= creature_modalmodes[ index ][1];
	local command	= creature_modalmodes[ index ][2];

	-- let creature modal commands be queued
	CommandQueueEnable( HK_System_CommandQueue, "commandqueuecancel" )

	--
	local result = ModalUIBegin( "domodalclick", "domodalcancel", mode, command )

	if result == 0 then

		--
		cleartaskbar()

		-- command area
			-- cancel button
		BindButton( "command_formation_icon07", HK_System_Escape, "domodalcancel", "", "UI/InGame/Cancel.tga", 0 )

	else

		-- failed
		failedcommand( result )

	end

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

--
commandarearesearch = function( id, research, tooltipcb )

	--
	local buttons =
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10",
	}

	local countbuttons  = getn( buttons  )
	local countresearch = getn( research )

	local count

	if countbuttons < countresearch then

		count = countbuttons

	else

		count = countresearch

	end

	for i = 1, count
	do

		if ResearchIsOpen( research[ i ][1] ) == 1 then

			BindButtonToResearch( buttons[ i ], research[ i ][2], "doresearch", tooltipcb, id, research[ i ][1] )

		end

	end

end

--
commandareaupgrade = function( id, ebpnetid, upgrade, tooltipcb )

	--
	local buttons =
	{
		"command_big_icon01",
		"command_big_icon02",
		"command_big_icon03",
		"command_big_icon04",
		"command_big_icon05",
		"command_big_icon06",
		"command_big_icon07",
		"command_big_icon08",
		"command_big_icon09",
		"command_big_icon10",
	}

	local countbuttons  = getn( buttons  )
	local countupgrade = getn( upgrade )

	local count

	if countbuttons < countupgrade then

		count = countbuttons

	else

		count = countupgrade

	end

	for i = 1, count
	do

		if CreatureUpgradeIsOpen( ebpnetid, upgrade[ i ][1] ) == 1 then

			BindButtonToCreatureUpgrade( buttons[ i ], upgrade[ i ][2], "docreatureupgrade", tooltipcb, id, ebpnetid, upgrade[ i ][1] )

		end

	end

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
