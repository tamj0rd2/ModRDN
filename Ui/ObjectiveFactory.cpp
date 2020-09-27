/////////////////////////////////////////////////////////////////////
// File    : ObjectiveFactory.cpp
// Desc    : 
// Created : Friday, September 21, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "ObjectiveFactory.h"

#include "Objective.h"

#include <Util/iff.h>

//------------------------------------------------------------------------------------------------
// pimple data
//------------------------------------------------------------------------------------------------

class ObjectiveFactory::Data
{
public:
	std::list<Objective*>	m_objectives;
};

//------------------------------------------------------------------------------------------------
// static routines
//------------------------------------------------------------------------------------------------

static ChunkHandlerFunc OF_HandleOBJL;
static ChunkHandlerFunc OF_HandleOBJT;

//------------------------------------------------------------------------------------------------
// ObjectiveFactory::ObjectiveFactory
//------------------------------------------------------------------------------------------------

ObjectiveFactory::ObjectiveFactory()
	: m_pimpl(new Data)
{
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

ObjectiveFactory::~ObjectiveFactory()
{
	std::list<Objective*>::iterator oi = m_pimpl->m_objectives.begin();
	std::list<Objective*>::iterator oe = m_pimpl->m_objectives.end();

	for (; oi != oe; oi++)
	{
		Objective* pObj = *oi;
		DELETEZERO( pObj );
	}

	DELETEZERO( m_pimpl );
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

Objective*	ObjectiveFactory::CreateObjective( int id )
{
	// check for duplicates
	if ( GetObjective( id ) != 0 )
	{
		dbPrintf("Duplicated objective number (%d)", id);
		return NULL;
	}

	Objective* pObj = new Objective;
	pObj->SetID( id );

	m_pimpl->m_objectives.push_back(pObj);

	return pObj;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

Objective*	ObjectiveFactory::GetObjective( int id )
{
	// linear search through list of folders
	std::list<Objective*>::iterator oi = m_pimpl->m_objectives.begin();
	std::list<Objective*>::iterator oe = m_pimpl->m_objectives.end();

	for (; oi != oe; oi++)
	{
		Objective* pObj = *oi;

		if (pObj->GetID() == id)
		{
			return pObj;
		}
	}

	return NULL;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void ObjectiveFactory::DeleteObjective( Objective* pObj )
{
	m_pimpl->m_objectives.remove(pObj);
	DELETEZERO( pObj );
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void ObjectiveFactory::GetAllObjectives( std::list<Objective*>& objectives )
{
	objectives = m_pimpl->m_objectives;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void ObjectiveFactory::Load( IFF& iff )
{
	iff.AddParseHandler( OF_HandleOBJL, Type_Form, 'OBJL', (void*)this, NULL);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void ObjectiveFactory::Save( IFF& iff )
{
	iff.PushChunk( Type_Form, 'OBJL', 1000L);

		std::list<Objective*>::iterator oi = m_pimpl->m_objectives.begin();
		std::list<Objective*>::iterator oe = m_pimpl->m_objectives.end();

		for (; oi != oe; oi++)
		{
			iff.PushChunk( Type_NormalVers, 'OBJT', 1000L);

				Objective* pObj = *oi;
				IFFWrite( iff, pObj->GetID() );
				pObj->Save( iff );

			iff.PopChunk();
		}		

	iff.PopChunk();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

static unsigned long OF_HandleOBJL(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	iff.AddParseHandler( OF_HandleOBJT, Type_NormalVers, 'OBJT', pContext1, NULL);
	return iff.Parse();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

static unsigned long OF_HandleOBJT(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	ObjectiveFactory* pObjFac = (ObjectiveFactory*)(pContext1);

	int id;
	IFFRead( iff, id );

	Objective* pObj = pObjFac->CreateObjective(id);
	if (pObj)
	{
		pObj->Load( iff );
	}

	return 0;
}
