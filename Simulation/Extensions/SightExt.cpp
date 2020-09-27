/////////////////////////////////////////////////////////////////////
// File    : SightExt.cpp
// Desc    : 
// Created : Thursday, June 28, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "SightExt.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../RDNPlayer.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/SightExtInfo.h"

#include <Util/Iff.h>
#include <Util/Biff.H>

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//

SightExt::SightExt( const SightExtInfo* pSightExtInfo ) :
	m_sightRadius( 0.0f ),
	m_FOWPosX( size_t(-1) ),
	m_FOWPosZ( size_t(-1) ),
	m_FOWRad( FLT_MAX ),
	m_bInFOW( false )
{
	if ( pSightExtInfo )
	{
		m_sightRadius		= pSightExtInfo->sightRadius;
	}
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
float SightExt::GetSightRadius() const
{
	return m_sightRadius;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void SightExt::SaveExt( BiFF& biff ) const
{
	IFF& iff = *biff.GetIFF( );

	biff.StartChunk (Type_NormalVers, 'ESGT', "Extension: Sight", 0);

		IFFWrite( iff, m_sightRadius );

    biff.StopChunk( );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void SightExt::LoadExt( IFF& iff )
{
	iff.AddParseHandler( HandleESGT, Type_NormalVers, 'ESGT', this, NULL );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
unsigned long SightExt::HandleESGT( IFF& iff, ChunkNode*, void* pContext1, void* )
{
	SightExt* pSightExt = static_cast< SightExt* >( pContext1 );
	dbAssert( pSightExt );

	IFFRead( iff, pSightExt->m_sightRadius );

	return 0;
}
