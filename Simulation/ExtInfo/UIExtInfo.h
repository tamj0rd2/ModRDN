/////////////////////////////////////////////////////////////////////
// File    : UIExtInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "ModStaticInfo.h" 
//#include "SigmaTuning.h"

#include <Util/Colour.h>


///////////////////////////////////////////////////////////////////// 
// UIExtInfo

class UIExtInfo : public ModStaticInfo::ExtInfo
{
// types
public:
	enum { ExtensionID = ModStaticInfo::EXTINFOID_UI };

// fields
public:
	bool	bMiniMapable;
	bool	bMiniMapTeamColour;
	Colour	miniMapColour;

	bool	bGhostable;

// construction
public:
	UIExtInfo( const ControllerBlueprint* cbp )
	{
		bMiniMapable		= GetVal( cbp, "minimap_enable", true );
		if ( bMiniMapable )
		{
			bMiniMapTeamColour	= GetVal( cbp, "minimap_teamcolour", true );
			if ( !bMiniMapTeamColour )
			{
				miniMapColour.r		= unsigned char(GetVal( cbp, "minimap_colour_r", 0xff ));
				miniMapColour.g		= unsigned char(GetVal( cbp, "minimap_colour_g", 0xff ));
				miniMapColour.b		= unsigned char(GetVal( cbp, "minimap_colour_b", 0xff ));
				miniMapColour.a		= 0xff;
			}
			else
			{
				//	Hot pink, the error colour
				miniMapColour = Colour(0xff,0,0xff);
			}
		}

		bGhostable			= GetVal( cbp, "ghost_enable", false );
	}
};
