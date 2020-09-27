/////////////////////////////////////////////////////////////////////
// File    : BlipFactory.cpp
// Desc    : 
// Created : Monday, September 24, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "BlipFactory.h"

#include "../ModObj.h"

#include <Assist/StlExVector.h>

#include <Util/IFFmath.h>
#include <Util/IFF.h>

#include <Platform/Platform.h>

//------------------------------------------------------------------------------------------------
// static routines
//------------------------------------------------------------------------------------------------

static ChunkHandlerFunc BF_HandleBLPL;
static ChunkHandlerFunc BF_HandleBLIP;

//------------------------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------------------------

namespace
{
	const long IFFVERSION = 1001L;

	const int BLIPTEMPID = 65535;

	struct EqualBlipId :
		public std::binary_function< const Blip*, int, bool >
	{
		bool operator()( const Blip* l, int r ) const
		{
			return l->GetID() == r;
		}
	};

	const float BLIPPERIOD = 4.0f;		// total cycle duration in seconds

	// compute blip radius
	// ... the blip radius varies periodically as follows:
	//
	//
	//
	//
	//        /~~~~~|
	//      /       |
	//    /         |
	//  /           |_________
	//
	//  |     |     |        |
	//  t0    t1    t2       t3

	// the following time constants are normalized to the duration of the period
	const float t0 = 0.0f;
	const float t1 = 0.6f;
	const float t2 = 0.7f;
	const float t3 = 1.0f;

	const float BlipMinRadius = 0.001f;
	const float BlipMaxRadius = 0.015f;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

static float UpdateBlipRadius( float timeBlip )
{
	// calculate t
	float t = timeBlip / BLIPPERIOD;
	t -= ((int)t);
	dbAssert( t >= 0.0f && t <= 1.0f );

	float radius = 0;

	// calculate radius
	if( t >= t0 && t <= t1 )
	{
		const float tRamp = ( t - 0.0f ) / ( t1 - t0 );
		radius = BlipMinRadius + ( tRamp * ( BlipMaxRadius - BlipMinRadius ) );
	}
	else
	if( t >= t1 && t <= t2 )
	{
		radius = BlipMaxRadius;
	}
	if( t >= t2 && t <= t3 )
	{
		radius = 0.0f;
	}

	return radius;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

static unsigned long BF_HandleBLPL(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	iff.AddParseHandler( BF_HandleBLIP, Type_NormalVers, 'BLIP', pContext1, NULL);
	return iff.Parse();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

static unsigned long BF_HandleBLIP(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	BlipFactory* pBlipFac = static_cast<BlipFactory*>(pContext1);

	int id;
	IFFRead( iff, id );

	Blip* pBlip = pBlipFac->CreateBlip(id);
	if (pBlip)
	{
		pBlip->Load( iff );
	}

	return 0;
}

//------------------------------------------------------------------------------------------------
// BlipFactory
//------------------------------------------------------------------------------------------------

BlipFactory::BlipFactory()
	: m_blipTemp( BLIPTEMPID )
{
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

BlipFactory::~BlipFactory()
{
	std::for_each( m_blips.begin(), m_blips.end(), DELETEITEM() );
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

Blip* BlipFactory::CreateBlip( int id )
{
	// validate id
	if( id >= BLIPTEMPID )
	{
		dbPrintf("Invalid Blip number (%d)", id);
		return NULL;
	}

	// check for duplicates
	if( GetBlipFromId( id ) != 0 )
	{
		dbPrintf("Duplicated Blip number (%d)", id);
		return NULL;
	}

	//
	Blip* p = new Blip( id, false );
	m_blips.push_back(p);

	return p;
}

Blip* BlipFactory::CreateBlipTemp()
{
	//
	Blip* p = new Blip( m_blipTemp++, true );
	m_blips.push_back(p);

	return p;
}

Blip* BlipFactory::GetBlipFromId( int id )
{
	// linear search through list
	BlipArray::iterator found = 
		std::find_if( m_blips.begin(), m_blips.end(), std::bind2nd(EqualBlipId(), id) );

	// not found?
	if( found == m_blips.end() )
		return 0;

	return *found;	
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void BlipFactory::DeleteBlip( int id )
{
	// linear search through list
	BlipArray::iterator found = 
		std::find_if( m_blips.begin(), m_blips.end(), std::bind2nd(EqualBlipId(), id) );

	if( found == m_blips.end() )
		return;

	// keep pointer
	Blip* p = *found;

	// remove from list
	std::vector_eraseback( m_blips, found );

	//
	DELETEZERO( p );

	return;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void BlipFactory::DeleteBlipDead()
{
	// kill all dead blips
	BlipArray::iterator i = m_blips.begin();

	for( ; i != m_blips.end(); )
	{
		if( (*i)->IsDead() )
		{
			i = std::vector_eraseback( m_blips, i );
		}
		else
		{
			i++;
		}
	}

	return;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void BlipFactory::Load( IFF& iff )
{
	iff.AddParseHandler( BF_HandleBLPL, Type_Form, 'BLPL', (void*)this, NULL);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void BlipFactory::Save( IFF& iff )
{
	iff.PushChunk( Type_Form, 'BLPL', 1000L);

		size_t i = 0;
		size_t e = GetBlipCount();

		for( ; i != e; ++i )
		{
			Blip* p = GetBlipAt( i );

			if( !p->IsTemp() )
			{
				iff.PushChunk( Type_NormalVers, 'BLIP', IFFVERSION );

					IFFWrite( iff, p->GetID() );
					p->Save( iff );

				iff.PopChunk();
			}
		}		

	iff.PopChunk();
}

///////////////////////////////////////////////////////////////////// 
// Blip

Blip::Blip( const int id, bool temp )
	: m_id( id ),
	  m_flags( temp? BF_TEMP : 0 ),
	  m_pos( 0, 0, 0 ),
	  m_timeBlip( 0),
	  m_lifetime( BLIPPERIOD )
{
	m_entity.SetFlag( EF_IsSpawned );

	return;
}

void Blip::Load( IFF& iff )
{
	if( iff.GetNormalVersion() < 1001L )
	{
		m_flags = BF_ENTITY;
		m_entity.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );
	}
	else
	{
		// read flags
		IFFRead( iff, m_flags );

		//
		if( m_flags & BF_ENTITY )
		{
			m_entity.LoadEmbedded( iff, ModObj::i()->GetEntityFactory() );
			if (!m_entity.empty())
			{
				m_pos = m_entity.front()->GetPosition();
			}
		}
		else
		if( m_flags & BF_POS )
		{
			IFFRead( iff, m_pos );
		}
		else
		{
			// wtf??
			m_flags = 0;
		}
	}

	return;
}

void Blip::Save( IFF& iff )
{
	// validate object state
	dbAssert( !IsTemp() );

	// save flags
	IFFWrite( iff, m_flags );

	//
	if( m_flags & BF_ENTITY )
	{
		m_entity.SaveEmbedded( iff );
	}
	else
	if( m_flags & BF_POS )
	{
		IFFWrite( iff, m_pos );
	}

	return;
}

void Blip::Reset()
{
	m_entity.clear();
	m_pos = Vec3f( 0, 0, 0 );

	m_flags &= ~BF_POS;
	m_flags &= ~BF_ENTITY;

	return;
}

void Blip::SetEntity( const Entity* e )
{
	//
	Reset();

	//
	if( e )
	{
		m_entity.push_back( const_cast<Entity*>(e) );
		m_pos = e->GetPosition();
		m_flags |= BF_ENTITY;
	}

	return;
}

void Blip::SetPosition( const Vec3f& pos )
{
	//
	Reset();
	
	//
	m_pos = pos;
	m_flags |= BF_POS;

	return;	
}

void Blip::SetLifeTime( float lifetime )
{
	// lifetime only applies to temp blips
	if ( m_flags & BF_TEMP )
	{
		m_lifetime = lifetime;
	}
}


bool Blip::Update( float elapsedSeconds )
{
	m_timeBlip += elapsedSeconds;

	// update entity position
	if( m_flags & BF_ENTITY )
	{
		if (m_entity.empty())
		{
			if( m_flags & BF_TEMP )
			{
				m_flags = BF_POS | BF_TEMP;
			}
			else
			{
				m_flags = 0;
			}
		}
		else
		{
			m_pos = m_entity.front()->GetPosition();
		}
	}

	if( m_flags & BF_TEMP )
	{
		if( m_timeBlip >= m_lifetime )
			m_flags = 0;
	}
	
	// validate
	if( IsDead() )
		return false;

	// update radius
	m_radius = UpdateBlipRadius( m_timeBlip );

	return true;
}
