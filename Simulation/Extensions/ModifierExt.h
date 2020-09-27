/////////////////////////////////////////////////////////////////////
// File    : ModifierExt.h
// Desc    : 
// Created : Wednesday, June 27, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

#include "../Modifiers/Modifier.h"

#include <Memory/MemoryFSAlloc.h>

class Entity;
class Modifier;
class EntityController;

///////////////////////////////////////////////////////////////////////////////
// ModifierExt

class ModifierExt : public Extension
{
// types
public:
	enum
	{
		ExtensionID = EXTID_Modifier,
	};

// interface
public:
	// called by the controller each execute update
	void		Execute();

	// a Modifier is deleted after Modifier::Execute() returns false
	void		AddModifier( Modifier* );

	void		FlushModifiers();

	Modifier*	GetModifier( Modifier::ModifierType modifierID ) const;

// inherited interface: Extension
private:

	virtual void SaveExt( BiFF& ) const;
	virtual void LoadExt( IFF& );

// Chunk Handlers
private:
	
	static unsigned long HandleEMDE( IFF&, ChunkNode*, void*, void* );
	static unsigned long HandleMDFR( IFF&, ChunkNode*, void*, void* );

// construction
protected:
	 ModifierExt();
	~ModifierExt();

// fields
private:
	typedef std::list< Modifier*, mempool_fs_alloc< Modifier* > > 
		ModifierList;

	ModifierList			m_modifiers;
	// this is the iterator for the execute loop.  This needs
	// to be a member so that the flush call can fix it up if it gets called.
	ModifierList::iterator	m_execiter;

// implementation
private:
	void RmvModifier( Modifier* );
};

///////////////////////////////////////////////////////////////////// 
// Modifier Helpers

template < class T >
	inline T* QIMod( ModifierExt *pModExt )
	{
		return ( pModExt == 0 )? 0 : static_cast< T* >( pModExt->GetModifier( Modifier::ModifierType( T::ModifierID ) ) );
	}

template < class T >
		inline const T* QIMod( const ModifierExt *pModExt )
	{
		return QIMod< T >( const_cast< ModifierExt* >( pModExt ) );
	}

template < class T >
	inline T* QIMod( ModController *pMC )
	{
		return ( pMC == 0 )? 0 : QIMod< T >( QIExt<ModifierExt>( pMC ) );
	}

template < class T >
	inline T* QIMod( EntityController *pEC )
		{
		return QIMod< T >( static_cast< ModController* >( pEC ) );
	}

template < class T >
	inline const T* QIMod( const EntityController *pEC )
	{
		return QIMod< T >( const_cast< EntityController* >( pEC ) );
	}

template < class T >
	inline T* QIMod( Entity* pE )
	{
		return ( pE == 0 )? 0 : QIMod< T >( pE->GetController() );
	}

template < class T >
	inline const T* QIMod( const Entity* pE )
	{
		return QIMod< T >( const_cast< Entity* >( pE ) );
	}