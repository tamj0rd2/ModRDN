/////////////////////////////////////////////////////////////////////
// File    : RDNUIState.cpp
// Desc    : 
// Created : Friday, July 12, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNUIState.h"

#include "../ModObj.h"
#include "ObjectiveFactory.h"
#include "BlipFactory.h"
#include "RDNHUD.h"

#include <EngineAPI/CameraInterface.h>
#include <EngineAPI/SelectionInterface.h>
#include <EngineAPI/GhostInterface.h>

#include <Util/Iff.h>
#include <Util/IffMath.h>

///////////////////////////////////////////////////////////////////// 
// 

namespace
{
	RDNUIState* s_pSingleton = NULL;
}

///////////////////////////////////////////////////////////////////// 
// Singleton interface

void RDNUIState::Startup()
{
	dbAssert( s_pSingleton == NULL );
	s_pSingleton = new RDNUIState;
}

void RDNUIState::Shutdown()
{
	dbAssert( s_pSingleton );
	DELETEZERO( s_pSingleton );
}

RDNUIState* RDNUIState::i()
{
	dbAssert( s_pSingleton );
	return s_pSingleton;
}

///////////////////////////////////////////////////////////////////// 
// Interface

RDNUIState::RDNUIState() :
	m_bIsLoaded	( false ),
	m_camDecl	( PI/4 ),
	m_camRotate	( 0.0f ),
	m_camZoom	( 10.0f ),
	m_gameSpeed	( 2 )
{
	
}

RDNUIState::~RDNUIState()
{
	
}


///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIState::Save( IFF& iff ) const
{
	// save the objectives
	ObjectiveFactory* pObjFac = ModObj::i()->GetObjectiveFactory();
	pObjFac->Save( iff );

	// save the blips
	BlipFactory* pBlipFac = ModObj::i()->GetBlipFactory();
	pBlipFac->Save( iff );

	// save the ghosts
	ModObj::i()->GetGhostInterface()->Save( iff );

	// save the camera
	CameraInterface*	pCamera		= ModObj::i()->GetCameraInterface();
	SelectionInterface* pSelection	= ModObj::i()->GetSelectionInterface();
	
	iff.PushChunk( Type_NormalVers, 'SUIS', 3 );

		Vec3f target;
		pCamera->GetTarget( target );

		IFFWrite( iff, target );

		IFFWrite( iff, pCamera->GetDeclination() );
		IFFWrite( iff, pCamera->GetRotation() );
		IFFWrite( iff, pCamera->GetZoom() );

		const size_t count = nHOTKEYGROUPS;
		IFFWrite( iff, count );

		size_t i = 0;
		for ( ; i != count; ++i )
		{
			pSelection->GetHotkeyGroup( i ).SaveEmbedded( iff );
		}
		
		pSelection->GetSelection().SaveEmbedded( iff );

	iff.PopChunk();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNUIState::Load( IFF& iff )
{
	// load the objectives
	ObjectiveFactory* pObjFac = ModObj::i()->GetObjectiveFactory();
	pObjFac->Load( iff );

	// load the blips
	BlipFactory* pBlipFac = ModObj::i()->GetBlipFactory();
	pBlipFac->Load( iff );

	// load the ghosts
	ModObj::i()->GetGhostInterface()->Load( iff );

	iff.AddParseHandler( HandleSUIS, Type_NormalVers, 'SUIS', this, NULL );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
unsigned long RDNUIState::HandleSUIS( IFF& iff, ChunkNode* , void* pContext1, void* )
{
	RDNUIState* pUIState = static_cast<RDNUIState*>( pContext1 );
	dbAssert( pUIState );

	IFFRead( iff, pUIState->m_camTarget );

	if ( iff.GetNormalVersion() >= 2 )
	{
		IFFRead( iff, pUIState->m_camDecl );
		IFFRead( iff, pUIState->m_camRotate );
		IFFRead( iff, pUIState->m_camZoom );
	}

	size_t HotkeyCount;
	IFFRead( iff, HotkeyCount );

	size_t i=0;
	for ( ; i != HotkeyCount; ++i )
	{
		if ( i < nHOTKEYGROUPS )
		{
			pUIState->m_HotkeyGroups[i].LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );
		}
		else
		{
			EntityGroup temp;
			temp.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );
		}
	}

	pUIState->m_Selection.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );

	pUIState->m_bIsLoaded = true;

	return 0;
}
