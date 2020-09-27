/////////////////////////////////////////////////////////////////////
// File    : RDNUIOptions.cpp
// Desc    : 
// Created : Saturday, March 23, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNUIOptions.h"

#include "../RDNDLLSetup.h"

#include <EngineAPI/UIInterface.h>
#include <EngineAPI/CameraInterface.h>

namespace
{
	const char*	k_UIOptionsFilename = "Player:ICUIOptions.lua";
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNUIOptions::RDNUIOptions( UIInterface* pUIInterface, CameraInterface* pCameraInterface )
:	m_pUIInterface		( pUIInterface ),
	m_pCameraInterface	( pCameraInterface )
{
	dbAssert( m_pUIInterface );
	dbAssert( m_pCameraInterface );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNUIOptions::~RDNUIOptions( )
{
	m_pUIInterface = NULL;
	m_pCameraInterface = NULL;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIOptions::Load( )
{
	m_LuaConfig.LoadFile( k_UIOptionsFilename );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIOptions::Save( )
{
	m_LuaConfig.StartSave( k_UIOptionsFilename );
	m_LuaConfig.SaveAll( true );
	m_LuaConfig.EndSave();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIOptions::ApplyOptions( )
{
	///////////////////////////////////////////////////////////////////// 
	// Boolean Options

	// Apply Mini Map Options
	m_pUIInterface->SetMMZoom						( GetBoolOption( MMO_Zoom ) );
	m_pUIInterface->SetMMRotateCamera				( GetBoolOption( MMO_Rotate ) );
	m_pUIInterface->SetMMPanCamera					( GetBoolOption( MMO_Pan ) );
	
	// Apply Camera Options
	m_pCameraInterface->SetCanRotate				( GetBoolOption( CO_Rotate ) );
	m_pCameraInterface->SetCanDeclinate				( GetBoolOption( CO_Declinate ) );
	m_pCameraInterface->SetInvertDec				( GetBoolOption( CO_InvertDec ) );
	m_pCameraInterface->SetInvertPan				( GetBoolOption( CO_InvertPan ) );
	
	///////////////////////////////////////////////////////////////////// 
	// Float Options

	// UI Options
	
	// Must convert the range 0.0 - 1.0 into 0.20 to 5
	float mousescroll = GetFloatOption( UIO_MouseScroll );
	mousescroll = expf( ( mousescroll * 2.0f * logf( 5.0f )  - logf( 5.0f ) ) );
	m_pCameraInterface->SetMouseScrollMult( mousescroll );

	float keyscroll = GetFloatOption( UIO_KeyScroll );
	keyscroll = expf( ( keyscroll * 2.0f * logf( 5.0f )  - logf( 5.0f ) ) );
	m_pCameraInterface->SetKeyScrollMult( keyscroll );
}

///////////////////////////////////////////////////////////////////// 
// 
namespace
{
	const char* k_BooleanOptionNames[ RDNUIOptions::BO_Last ] = 
	{
		// minimap Options
		"MMRotate",
		"MMZoom",
		"MMPan",

		// Camera Options
		"CAMRotate",
		"CAMDeclinate",
		"CAMInvertDec",
		"CAMInvertPan",
	};
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIOptions::SetBoolOption( BooleanOptions Option, bool bValue )
{
	dbAssert( Option >= BO_First && Option < BO_Last );

	LCSetBool( m_LuaConfig, k_BooleanOptionNames[ Option ], bValue );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNUIOptions::GetBoolOption( BooleanOptions Option ) const
{
	dbAssert( Option >= BO_First && Option < BO_Last );

	bool bValue;
	// try and retrieve the option
	if ( !LCGetBool( m_LuaConfig, k_BooleanOptionNames[ Option ], bValue ) )
	{
		bValue = false;
		
		// If you want the option to be true then add a condition here
		if  ( 
				Option == CO_Rotate			||
				Option == CO_Declinate		||
				Option == CO_InvertPan
			)
		{
			bValue = true;
		}
	}

	return bValue;
}

///////////////////////////////////////////////////////////////////// 
// 
namespace
{
	const char* k_FloatOptionNames[ RDNUIOptions::FO_Last ] = 
	{
		// UI Options
		"MouseScroll",
		"KeyScroll"
	};
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIOptions::SetFloatOption( FloatOptions Option, float value )
{
	dbAssert( Option >= FO_First && Option < FO_Last );

	LCSetFloat( m_LuaConfig, k_FloatOptionNames[ Option ], value );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
float RDNUIOptions::GetFloatOption( FloatOptions Option ) const
{
	dbAssert( Option >= FO_First && Option < FO_Last );

	float value;
	// try and retrieve the option
	if ( !LCGetFloat( m_LuaConfig, k_FloatOptionNames[ Option ], value ) )
	{
		value = 1.0f;
		
		if ( 
			Option == UIO_MouseScroll ||
			Option == UIO_KeyScroll
			)
		{
			value = 0.5f;
		}

		
	}

	return value;
}