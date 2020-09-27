/////////////////////////////////////////////////////////////////////
// File    : ModifierExt.cpp
// Desc    : 
// Created : Wednesday, June 27, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "ModifierExt.h"

#include <Util/Iff.h>
#include <Util/BIff.h>

///////////////////////////////////////////////////////////////////// 
// 

static MemPoolSmall s_pool( "Modifiers" );

namespace
{
	Modifier* CreateModifier( long ModID )
	{
		UNREF_P( ModID );
/***
		switch ( ModID )
		{
			case Modifier::MID_Laughing:
				return new LaughingModifier;
			break;

			default:
				dbBreak();
		}
***/

		return NULL;
	}

	unsigned long GetModifierVersion( long ModID )
	{
		UNREF_P( ModID );
/***
		switch ( ModID )
		{
			case Modifier::MID_Laughing:
				return LaughingModifier::ModifierFileVer;
			break;

			default:
				dbBreak();
		}
***/

		return 1;
	}
	
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
ModifierExt::ModifierExt()
	: m_modifiers( ModifierList::allocator_type( s_pool ) )
{
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
ModifierExt::~ModifierExt()
{
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::Execute()
{
	ModController* pMC = GetSelf();
	
	m_execiter = m_modifiers.begin();

	while ( m_execiter != m_modifiers.end() )
	{
		const bool r = ( *m_execiter )->Execute( pMC );
		
		// This might look strange but this is to fix the case where
		// the above execute call calls flushmodifiers
		if ( m_execiter == m_modifiers.end() )
			break;

		if ( r )
		{
			ModifierList::iterator tmp = m_execiter;

			m_execiter++;
			// remove it
			RmvModifier( *tmp );
		}
		else
		{
			m_execiter++;
		}
	} // next

	return;
}


///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::AddModifier( Modifier* pModifier )
{
	// validate parm
	dbAssert( pModifier != 0 );
	dbAssert( std::find( m_modifiers.begin(), m_modifiers.end(), pModifier ) == m_modifiers.end() );

	// add modifier to list
	m_modifiers.push_back( pModifier );

	// apply modifier
	pModifier->Init( GetSelf() );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::FlushModifiers()
{
	while( !m_modifiers.empty() )
	{
		// remove modifier
		RmvModifier( m_modifiers.front() );
	}

	// Signal the Execute function that the list is empty now.
	// this happens if one of the modifiers indirectly causes the entity to die
	m_execiter = m_modifiers.end();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
Modifier* ModifierExt::GetModifier( Modifier::ModifierType modifierID ) const
{
	ModifierList::const_iterator iter = m_modifiers.begin();
	ModifierList::const_iterator eiter = m_modifiers.end();

	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetModifierType() == modifierID )
			break;
	}

	if ( iter != eiter )
		return *iter;

	return NULL;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::RmvModifier( Modifier* m )
{
	// validate parm
	dbAssert( m != 0 );

	// locate modifier
	ModifierList::iterator found = std::find( m_modifiers.begin(), m_modifiers.end(), m );
	dbAssert( found != m_modifiers.end() );

	// remove it
	m_modifiers.erase( found );
		
	// stop it
	m->Shut( GetSelf() );

	// release it
	delete m;
}


///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::SaveExt( BiFF& biff ) const
{
	IFF& iff = *biff.GetIFF();

	iff.PushChunk( Type_Form, 'EMDE', 1 );

		ModifierList::const_iterator iter = m_modifiers.begin();
		ModifierList::const_iterator eiter = m_modifiers.end();

		for ( ; iter != eiter; ++iter )
		{
			Modifier* pModifier = *iter;

			long ModID = pModifier->GetModifierType();

			iff.PushChunk( Type_NormalVers, 'MDFR', GetModifierVersion( ModID ) );

				IFFWrite( iff, ModID );
			
				pModifier->Save( iff );
			
			iff.PopChunk();
		}

	iff.PopChunk();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void ModifierExt::LoadExt( IFF& iff )
{
	iff.AddParseHandler( HandleEMDE, Type_Form, 'EMDE', this, NULL );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
unsigned long ModifierExt::HandleEMDE( IFF& iff, ChunkNode*, void* pContext1, void* )
{
	dbAssert( pContext1 );

	iff.AddParseHandler( HandleMDFR, Type_NormalVers, 'MDFR', pContext1, NULL );

	return iff.Parse();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
unsigned long ModifierExt::HandleMDFR( IFF& iff, ChunkNode*, void* pContext1, void* )
{
	ModifierExt* pModifierExt = static_cast< ModifierExt* >( pContext1 );
	dbAssert( pModifierExt );

	//
	long ModID;
	IFFRead( iff, ModID );

	//
	Modifier* pModifier = CreateModifier( ModID );
	if ( pModifier )
	{
		dbAssert( pModifier->GetModifierType() == ModID );
		// Load the modifier
		pModifier->Load( iff, pModifierExt->GetSelf() );

		// Add it to our list
		pModifierExt->m_modifiers.push_back( pModifier );
	}

	return 0;
}
