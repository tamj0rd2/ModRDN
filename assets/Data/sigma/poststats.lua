-- poststats.lua - March 11th/ 2002

-- All that is needed to edit the postgame screen stats

-- list of all localized string name values from sigmatext.h
SCOREBOARD_COMPARE =			39000
SCOREBOARD_COMBAT =			39001
SCOREBOARD_RESOURCES =			39002
SCOREBOARD_ARMIES =			39003
SCOREBOARD_RESEARCH =			39004
SCOREBOARD_BESTINSHOW = 		39005
SCOREBOARD_ADVANCED = 			39006
								     
SCOREBOARD_COMBATSCORE =		39010
SCOREBOARD_RESOURCESCORE =		39011
SCOREBOARD_RESEARCHSCORE =		39012
SCOREBOARD_OVERALLSCORE =		39013
								     
SCOREBOARD_CREATURESPRODUCED =	39020	
SCOREBOARD_TOTALKILLED =		39021	
SCOREBOARD_TOTALLOST =			39022	
SCOREBOARD_STRUCTURESBUILT =	39023	
SCOREBOARD_STRUCTURESLOST =		39024	
SCOREBOARD_HENCHMENBUILT =		39025
SCOREBOARD_HENCHMENMAX =		39080
SCOREBOARD_STRUCTURESKILLED =	39026
								     
SCOREBOARD_SCRAPGATHERED =		39030	
SCOREBOARD_SCRAPSPENT =		39031
SCOREBOARD_SCRAPCONVERTED =		39038
SCOREBOARD_ELECTRICITYGATHERED =	39032	
SCOREBOARD_ELECTRICITYSPENT =	39033	
SCOREBOARD_ELECTRICITYCONVERTED =	39039
SCOREBOARD_RESEARCHED =			39034
SCOREBOARD_RESEARCHCOUNT =		39036
SCOREBOARD_MAPEXPLORED =		39035	
								     
SCOREBOARD_BESTUNIT =			39040
SCOREBOARD_AVERAGECREATURECOST =	39042
SCOREBOARD_AVERAGECREATURERANK =	39043
SCOREBOARD_ARMYRATING =		39045

SCOREBOARD_TIMETORANK2 = 		39050
SCOREBOARD_TIMETORANK3 = 		39051
SCOREBOARD_TIMETORANK4 = 		39052
SCOREBOARD_TIMETORANK5 = 		39053
SCOREBOARD_FINALRANK = 			39054

SCOREBOARD_MOSTLOSSES = 		39060
SCOREBOARD_MOSTEFFECTIVE =		39061
SCOREBOARD_MOSTDMG2STRUCTURES =	39062
SCOREBOARD_MOSTDMG2CREATURES =	39063
SCOREBOARD_MOSTBUILT =			39064

SCOREBOARD_CREAUTREPRODUCED =	39070
SCOREBOARD_CREATURELOSSED = 		39071
SCOREBOARD_CREATUREMAX = 		39072
SCOREBOARD_DMG2CREATURES = 		39073
SCOREBOARD_DMG2STRUCTURES = 	39074
SCOREBOARD_EFFICIENCY = 			39075

SCOREBOARD_STINKCOUNT           = 39090
SCOREBOARD_ELECTRICBURSTCOUNT   = 39091
SCOREBOARD_QUILLBURSTCOUNT      = 39092
SCOREBOARD_FRENZYCOUNT          = 39093
SCOREBOARD_DIGGINGCOUNT         = 39094
SCOREBOARD_SONARPULSECOUNT      = 39095
SCOREBOARD_PLAGUECOUNT          = 39096
SCOREBOARD_COALDONATED          = 39097
SCOREBOARD_ELECDONATED          = 39098
SCOREBOARD_DMGBYSBTOWER         = 39099
SCOREBOARD_DMGBYAATOWER         = 39100
SCOREBOARD_POPMAX               = 39101
SCOREBOARD_NUMCREATUREUPGRADES  = 39102
SCOREBOARD_HITPOINTSLOST        = 39103
SCOREBOARD_EFFAGAINSTSTRUCTURES = 39104
SCOREBOARD_EFFAGAINSTCREATURES  = 39105
SCOREBOARD_HITPOINTLOSTVALUE    = 39106
SCOREBOARD_WEBTHROWCOUNT        = 39107
SCOREBOARD_ASSASSINATECOUNT		= 39108
SCOREBOARD_FLASHCOUNT			= 39109
SCOREBOARD_INFESTATIONCOUNT		= 39110
SCOREBOARD_SOILEDLANDCOUNT      = 39111
SCOREBOARD_JUMPINGCOUNT			= 39112

SCOREBOARD_SBTOWERBURSTCOUNT	= 39113
SCOREBOARD_AATOWERBURSTCOUNT	= 39114

SCOREBOARD_LABDEFENSECOUNT		= 39115
SCOREBOARD_RADARPULSECOUNT		= 39116

-- NumTabs
--  set the number of tabs we have (this must be set and not erase)

NumTabs = 7

function ScoresTabType( tabIndex )

	-- should validate arguments first

	return ColumnHeadings[ tabIndex+1 ].tabtype;
	
end

-- TabName
--  return the name of each tab

function TabName( tabIndex )

	-- should validate arguments first

	return ColumnHeadings[ tabIndex+1 ].tabname;
	
end

-- ScoresTabColumnDecFunc
--  return the function style that will be used to decorate and highlight a column element

function ScoresTabColumnDecFunc( tabIndex, columnIndex )
	
	return ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ].highlight;
	
end

-- ColumnHeadingInfo
--  return the name of each column heading within each tab

function ColumnHeadingInfo( tabIndex, columnIndex )

	-- should validate arguments first
	
	return ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ].name;

end

-- ColumnTooltipInfo
--  return the name of each column heading within each tab

function ColumnTooltipInfo( tabIndex, columnIndex )

	-- should validate arguments first
	
	return ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ].tooltip;

end

-----------------------------------------
-- Scoring Functions

-- TotalResourcesKills

function TotalResourcesKills( idplayer )

	local totalkilled = 0.0;

	local i = 0;
	local e = Stats_CreaturesTypeCount( idplayer );

	while ( i < e ) do
	
		local ebpid = Stats_CreaturesTypeAt ( idplayer, i );
		local killv = Stats_CreatureCreatureDmgValue( idplayer, ebpid )+Stats_CreatureStructureDmgValue( idplayer, ebpid );

		totalkilled = totalkilled + killv;
		
		i = i + 1
	end

	return totalkilled;
end

-- TotalResourcesLosses

function TotalResourcesLosses( idplayer )

	local totallosses = 0.0;

	local i = 0;
	local e = Stats_CreaturesTypeCount( idplayer );

	while(  i < e ) do
		local ebpid  = Stats_CreaturesTypeAt  ( idplayer, i );
		local cost   = Stats_CreatureCostValue( idplayer, ebpid );
		local losses = Stats_CreatureLosses( idplayer, ebpid );

		totallosses = totallosses + (cost * losses);
		
		i = i + 1
	end

	return totallosses;
	
end

-- ScoreArmy

function ScoreArmy( idplayer )

	local sumKills  = 0.0;
	local sumLosses = 0.0;

	local i = 0;
	local e = Stats_CreaturesTypeCount( idplayer );

	while(  i < e ) do
	
		local ebpid = Stats_CreaturesTypeAt( idplayer, i );
		
		local k = Stats_CreatureDmgValue( idplayer, ebpid );
		local c = Stats_CreatureCostValue( idplayer, ebpid );
		local l = Stats_CreatureLosses( idplayer, ebpid );

		sumKills  = sumKills + k;
		sumLosses = sumLosses + (c * l);
		
		i = i + 1
	end

	return sumKills / max( 1.0, sumLosses );
end

-- ScoreCombat

function ScoreCombat( idplayer )
	
	return TotalResourcesKills( idplayer ) * 1.2;
	
end

-- ScoreResource

function ScoreResource( idplayer )

	local totalsurviving = 0.0;

	local i = 0;
	local e = Stats_CreaturesTypeCount( idplayer );

	while (i < e) do
	
		local ebpid = Stats_CreaturesTypeAt  ( idplayer, i );
		local cost  = Stats_CreatureCostValue( idplayer, ebpid );
		local count = Stats_CreatureActive   ( idplayer, ebpid );
		
		totalsurviving = totalsurviving + (cost * count);
		
		i = i + 1
	end

	local mapbonus = Stats_MapExplored( idplayer )* 10.0;
	
	local total = ( Stats_ScrapGathered( idplayer ) *.3 ) + ( Stats_ElectricityGathered( idplayer ) *.3 ) + ( mapbonus ) --( totalsurviving * 0.3 ) +;

	return total;
	
end

-- ScoreResearch

function ScoreResearch( idplayer )

	local totalGather = 
		Stats_BuildingsTotalScrap( idplayer ) +
		Stats_AddOnsTotalScrap( idplayer ) +
		Stats_TechTotalScrap( idplayer );

	local totalRenew = 
		Stats_BuildingsTotalElectricity( idplayer ) + 
		Stats_AddOnsTotaElectricity( idplayer ) +
		Stats_TechTotalElectricity( idplayer );

	return (totalGather + totalRenew)*.75;
	
end

-- ScoreOverall

function ScoreOverall( idplayer )

	local killV = TotalResourcesKills ( idplayer );
	local lostV = TotalResourcesLosses( idplayer );
	
	local scores = ScoreCombat  ( idplayer ) + ScoreResource( idplayer ) + ScoreResearch( idplayer );
	
	--local avgcost = max( 1.0, Stats_ArmyAvgCost( idplayer ) );
	
	--local combatmod = 1.0 + ( killV / avgcost ) / max( 1.0, ( lostV / avgcost ) );

	return scores;-- * combatmod  / 3 ;

end

function TotalResearchPlusCreatureUpgrades( idplayer )

	return Stats_TechResearchedCount(idplayer)+Stats_CreatureUpgradeCount(idplayer)

end

function CreatureEffectivenessVersusCreatures( idplayer, ebpid )
	
	local HPExtracted = Stats_CreatureCreatureDmgValue( idplayer, ebpid )
	local HPLost = Stats_CreatureSelfDmgValue( idplayer, ebpid )
	
	if (HPLost==0) then
		HPLost = 1
	end
	
	local ratio = 10 * HPExtracted/HPLost;
	
	return ratio;
	
end

function CreatureEffectivenessVersusStructures( idplayer, ebpid )
	
	local HPExtracted = Stats_CreatureStructureDmgValue( idplayer, ebpid )
	local HPLost = Stats_CreatureSelfDmgValue( idplayer, ebpid )
	
	if (HPLost==0) then
		HPLost = 1
	end
	
	local ratio =  HPExtracted/((HPLost)*0.01);
	
	return ratio
	
end

-- ArmyFindMax
-- this goes through a players army and runs a function 'functor' on each creature
-- and then calculates the best of these creatures based on the function. Example function
-- might be MostKills, this would return the ebp with the most kills. If you want to find
-- out how many kills this was set 'retValue' to 1

function ArmyFindMax( idplayer, functor, retValue, startVal )

	-- go through each creature an determine the one with the most losses

	local i = 0;
	local e = Stats_CreaturesTypeCount( idplayer );

	-- what does the value have to be great than to return anything at all
	local bestValue = startVal;
	-- return zero if there is no Max creature
	local bestEbp = 0;

	while( i < e ) do
		local ebp    = Stats_CreaturesTypeAt( idplayer, i );
		local ebpval = functor( idplayer, ebp );

		if( bestValue < ebpval ) then
		
			bestValue = ebpval;
			bestEbp = ebp;
		end
		
		i = i + 1
	end

	if (retValue==1) then
		return bestValue;
	else
		return bestEbp;
	end
end

function ArmyMostEffective( idplayer, retValue )
	
	return ArmyFindMax( idplayer, Stats_CreatureEffectiveness, retValue, 0)
	
end

function ArmyMostLosses( idplayer, retValue )

	return ArmyFindMax( idplayer, Stats_CreatureLosses, retValue, 0 )
	
end

function ArmyMostDmgToCreatures( idplayer, retValue )

	return ArmyFindMax( idplayer, Stats_CreatureCreatureDmg, retValue, 0 )
	
end

function ArmyMostDmgToStructures( idplayer, retValue )

	return ArmyFindMax( idplayer, Stats_CreatureStructureDmg, retValue, 0 )
	
end

function ArmyMostBuilt( idplayer, retValue)

	return ArmyFindMax( idplayer, Stats_CreatureTotal, retValue, 0)
	
end

-- ScoresTabColumnValue
--  given a tab and column return the value that should be displayed

function ScoresTabColumnValue( tabIndex, columnIndex, idplayer )
	
	-- make reference to this table
	local cdata = ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ];
	
	-- does this column and function exist
	if (cdata.name ~= 0 and cdata.func) then
		
		-- call func from table for this given tab
		--return ColumnHeadings[ tabIndex+1 ].tabfunc( columnIndex, idplayer );
		return cdata.func( idplayer );
		
	end
	
	return -1;

end

-- ScoresTabColumnValue
--  given a tab and column return the value that should be displayed

function ScoresTabColumnValue2( tabIndex, columnIndex, idplayer, ebpnetid )
	
	-- make reference to this table
	local cdata = ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ];
	
	-- does this column and function exist
	if (cdata.name ~= 0 and cdata.func) then
		
		-- call func from table for this given tab
		return cdata.func( idplayer,  ebpnetid);
		
	end
	
	return -1;

end


-- ScoresTabColumnType

function ScoresTabColumnType( tabIndex, columnIndex )

	-- make reference to this table
	local cdata = ColumnHeadings[ tabIndex+1 ].columndata[ columnIndex+1 ];
	
	-- does this column and data exist
	if (cdata.name ~= 0 and cdata.type) then
		
		return cdata.type;
		
	end
	
	-- error value
	return VT_COUNT;
end

-- tab column placement and naming (could place other entry data in here too)
ColumnHeadings = 
	{
		-- Total Score Tab
		{ 
			tabname=SCOREBOARD_COMPARE,
			tabtype=TT_ByPlayer,
						
			columndata={
				{
					name=SCOREBOARD_COMBATSCORE,
					tooltip=53500,
					type=VT_Number,
					func=ScoreCombat,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_RESOURCESCORE,
					tooltip=53501,
					type=VT_Number,
					func=ScoreResource,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_RESEARCHSCORE,
					tooltip=53502,
					type=VT_Number,
					func=ScoreResearch,
					highlight=DF_MaxEqual,
				},
				{name=0,type=0,func=0},
				{name=0,type=0,func=0},
				{
					name=SCOREBOARD_OVERALLSCORE,
					tooltip=53503,
					type=VT_Number,
					func=ScoreOverall,
					highlight=DF_MaxEqual,
				},
			},
		},
		-- Combat Tab 
		{ 
			tabname=SCOREBOARD_COMBAT,
			tabtype=TT_ByPlayer,
						
			columndata={
				{
					name=SCOREBOARD_CREATURESPRODUCED,
					tooltip=53504,
					type=VT_Number,
					func=Stats_CreaturesBuilt,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_TOTALKILLED,
					tooltip=53505,
					type=VT_Number,
					func=Stats_KillsTotal,
					highlight=DF_MaxEqual,
				},
				{	
					name=SCOREBOARD_TOTALLOST,
					tooltip=53506,
					type=VT_Number,
					func=Stats_LossesTotal,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_STRUCTURESKILLED,
					tooltip=53507,
					type=VT_Number,
					func=Stats_BuildingsKilled,
					highlight=DF_MaxEqual,
				},
				{	
					name=SCOREBOARD_STRUCTURESLOST,	
					tooltip=53508,	
					type=VT_Number,
					func=Stats_BuildingsLost,
					highlight=DF_MaxEqual,
				},
				{name=0,type=0,func=0,highlight=0},
			},
		},
		-- Resource Tab
		{ 
			tabname=SCOREBOARD_RESOURCES,
			tabtype=TT_ByPlayer,
						
			columndata={
				{
					name=SCOREBOARD_SCRAPGATHERED,
					tooltip=53509,
					type=VT_Number,
					func=Stats_ScrapGathered,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_SCRAPSPENT,
					tooltip=53510,		
					type=VT_Number,
					func=Stats_ScrapSpent,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_ELECTRICITYGATHERED,
					tooltip=53511,
					type=VT_Number,
					func=Stats_ElectricityGathered,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_ELECTRICITYSPENT,
					tooltip=53512,
					type=VT_Number,
					func=Stats_ElectricitySpent,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_MAPEXPLORED,
					tooltip=53513,
					type=VT_Number,
					func=Stats_MapExplored,
					highlight=DF_MaxEqual,
				},
				{	
					name=SCOREBOARD_HENCHMENBUILT,
					tooltip=53514,
					type=VT_Number,
					func=Stats_HenchmenBuilt,
					highlight=DF_MaxEqual,
				},
			},
		},
		-- Research Tab
		{ 
			tabname=SCOREBOARD_RESEARCH,
			tabtype=TT_ByPlayer,
						
			columndata={
				{
					name=SCOREBOARD_TIMETORANK2,
					tooltip=53515,
					type=VT_Time,
					func=Stats_Rank2Time,
					highlight=DF_MinEqual,
				},
				{
					name=SCOREBOARD_TIMETORANK3,
					tooltip=53516,
					type=VT_Time,
					func=Stats_Rank3Time,
					highlight=DF_MinEqual,
				},
				{
					name=SCOREBOARD_TIMETORANK4,
					tooltip=53517,
					type=VT_Time,
					func=Stats_Rank4Time,
					highlight=DF_MinEqual,
				},
				{
					name=SCOREBOARD_TIMETORANK5,
					tooltip=53518,
					type=VT_Time,
					func=Stats_Rank5Time,
					highlight=DF_MinEqual,
				},
				{
					name=SCOREBOARD_FINALRANK,
					tooltip=53519,
					type=VT_Number,
					func=Stats_RankFinal,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_RESEARCHCOUNT,
					tooltip=53520,
					type=VT_Number,
					func=TotalResearchPlusCreatureUpgrades,
					highlight=DF_MaxEqual,
				},
			},
		},
		-- Player Army Tab
		{ 
			tabname=SCOREBOARD_ARMIES,
			tabtype=TT_ByCreature,
						
			columndata={
				{
					name=SCOREBOARD_CREAUTREPRODUCED,
					tooltip=53521,
					type=VT_Number,
					func=Stats_CreatureTotal,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_CREATURELOSSED,
					tooltip=53522,
					type=VT_Number,
					func=Stats_CreatureLosses,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_CREATUREMAX,
					tooltip=53523,
					type=VT_Number,
					func=Stats_CreatureMax,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_DMG2CREATURES,
					tooltip=53524,
					type=VT_Number,
					func=Stats_CreatureCreatureDmg,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_DMG2STRUCTURES,
					tooltip=53525,
					type=VT_Number,
					func=Stats_CreatureStructureDmg,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_EFFICIENCY,
					tooltip=53526,
					type=VT_Number,
					func=Stats_CreatureEffectiveness,
					highlight=DF_MaxEqual,
				},
			},
		},
		-- Army Tab
		{ 
			tabname=SCOREBOARD_BESTINSHOW,
			tabtype=TT_ByPlayer,
						
			columndata={
				{
					name=SCOREBOARD_MOSTEFFECTIVE,
					tooltip=53527,
					type=VT_ControllerBlueprint,
					func=ArmyMostEffective,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_MOSTLOSSES,
					tooltip=53528,
					type=VT_ControllerBlueprint,
					func=ArmyMostLosses,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_MOSTDMG2CREATURES,
					tooltip=53529,
					type=VT_ControllerBlueprint,
					func=ArmyMostDmgToCreatures,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_MOSTDMG2STRUCTURES,
					tooltip=53530,
					type=VT_ControllerBlueprint,
					func=ArmyMostDmgToStructures,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_MOSTBUILT,
					tooltip=53531,
					type=VT_ControllerBlueprint,
					func=ArmyMostBuilt,
					highlight=DF_MaxEqual,
				},
				{
					name=SCOREBOARD_BESTUNIT,
					tooltip=53532,
					type=VT_ControllerBlueprint,
					func=ArmyMostEffective,
					highlight=DF_MaxEqual,
				},
			},
		},
		-- Advanced tab
		{ 
			tabname=SCOREBOARD_ADVANCED,
			tabtype=TT_ByPlayer,
						
			columndata={
				{name=0,type=0,func=0,highlight=0},
				{name=0,type=0,func=0,highlight=0},
				{name=0,type=0,func=0,highlight=0},
				{name=0,type=0,func=0,highlight=0},
				{name=0,type=0,func=0,highlight=0},
				{name=0,type=0,func=0,highlight=0},
			},
		},
		
		
		
	};

-- example table of stats for the advanced screen
StatList = {
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_POPMAX,
		tooltip=53541,
		type=VT_Number,
		func=Stats_PopulationMax,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_HENCHMENMAX,
		tooltip=53533,
		type=VT_Number,
		func=Stats_HenchmenMax,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_STRUCTURESBUILT,
		tooltip=53534,
		type=VT_Number,
		func=Stats_BuildingsBuilt,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_NUMCREATUREUPGRADES,
		tooltip=53541,
		type=VT_Number,
		func=Stats_CreatureUpgradeCount,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_STINKCOUNT,
		tooltip=53535,
		type=VT_Number,
		func=Stats_AbilityStinkCloud,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_ELECTRICBURSTCOUNT,
		tooltip=53536,
		type=VT_Number,
		func=Stats_AbilityElectricBurst,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_QUILLBURSTCOUNT,
		tooltip=53537,
		type=VT_Number,
		func=Stats_AbilityQuillBurst,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_FRENZYCOUNT,
		tooltip=53538,
		type=VT_Number,
		func=Stats_AbilityFrenzy,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_DIGGINGCOUNT,
		tooltip=53539,
		type=VT_Number,
		func=Stats_AbilityDigging,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_SONARPULSECOUNT,
		tooltip=53540,
		type=VT_Number,
		func=Stats_AbilitySonarPulse,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_PLAGUECOUNT,
		tooltip=53541,
		type=VT_Number,
		func=Stats_AbilityPlague,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_SCRAPCONVERTED,
		tooltip=53548,
		type=VT_Number,
		func=Stats_ScrapConverted,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_ELECTRICITYCONVERTED,
		tooltip=53549,
		type=VT_Number,
		func=Stats_ElectricityConverted,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_COALDONATED,
		tooltip=53541,
		type=VT_Number,
		func=Stats_DonatedGather,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_ELECDONATED,
		tooltip=53541,
		type=VT_Number,
		func=Stats_DonatedRenew,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_DMGBYSBTOWER,
		tooltip=53541,
		type=VT_Number,
		func=Stats_DamageBySBTower,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_DMGBYAATOWER,
		tooltip=53541,
		type=VT_Number,
		func=Stats_DamageByAATower,
		highlight=DF_MaxEqual,
	},
		{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_SBTOWERBURSTCOUNT,
		tooltip=53541,
		type=VT_Number,
		func=Stats_SBTowerBurstCount,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_AATOWERBURSTCOUNT,
		tooltip=53541,
		type=VT_Number,
		func=Stats_AATowerBurstCount,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_LABDEFENSECOUNT,
		tooltip=53541,
		type=VT_Number,
		func=Stats_LabDefenseCount,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_RADARPULSECOUNT,
		tooltip=53541,
		type=VT_Number,
		func=Stats_RadarPulseCount,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_WEBTHROWCOUNT,
		tooltip=53542,
		type=VT_Number,
		func=Stats_AbilityWebThrow,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_ASSASSINATECOUNT,
		tooltip=53543,
		type=VT_Number,
		func=Stats_AbilityAssassinate,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_FLASHCOUNT,
		tooltip=53544,
		type=VT_Number,
		func=Stats_AbilityFlash,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_INFESTATIONCOUNT,
		tooltip=53545,
		type=VT_Number,
		func=Stats_AbilityInfestation,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_SOILEDLANDCOUNT,
		tooltip=53546,
		type=VT_Number,
		func=Stats_AbilitySoiledLand,
		highlight=DF_MaxEqual,
	},
	{
		tabtype=TT_ByPlayer,
		name=SCOREBOARD_JUMPINGCOUNT,
		tooltip=53547,
		type=VT_Number,
		func=Stats_AbilityJumping,
		highlight=DF_MaxEqual,
	}
}

function StatListAt( idx )

	return StatList[idx+1].name
	
end

function StatListTooltipAt( idx )

	return StatList[idx+1].tooltip
	
end

function StatListTabTypeAt( idx )

	return StatList[idx+1].tabtype

end

function StatListTypeAt( idx )

	return StatList[idx+1].type

end

function StatListDecoratorFuncAt( idx )
	
	return StatList[idx+1].highlight
	
end

function StatListValue( idx, idplayer )
	-- make reference to this table
	local cdata = StatList[ idx+1 ]
	
	-- does this column and function exist
	if (cdata and cdata.func) then
		
		-- call func
		return cdata.func( idplayer );
		
	end
	
	return -1;
end

function StatListValue2( idx, idplayer, ebpnetid )
	-- make reference to this table
	local cdata = StatList[ idx+1 ]
	
	-- does this column and function exist
	if (cdata and cdata.func) then
		
		-- call func
		return cdata.func( idplayer,  ebpnetid);
		
	end
	
	return -1;
end

