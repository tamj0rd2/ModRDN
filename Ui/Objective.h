/////////////////////////////////////////////////////////////////////
// File    : Objective.h
// Desc    : 
// Created : Friday, September 21, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <SimEngine/EntityGroup.h>

//------------------------------------------------------------------
//------------------------------------------------------------------

class IFF;

class Objective
{
private:
	// hide constructor so it must be created by friends
	Objective();
	~Objective();

public:
	// enumrated types
	enum Type
	{
		OT_Primary,
		OT_Secondary,
		OT_NumTypes
	};

	enum State
	{
		OS_Off,
		OS_Incomplete,
		OS_Complete,
		OS_Failed,

		OS_NumStates
	};

public:
	// accessors
	int				GetID() { return m_ID; }
	int				GetShortDescID() { return m_shortDescID; }
	int				GetTipID() { return m_tipID; }
	Type			GetType() { return m_type; }
	State			GetState() { return m_state; }
	Entity*			GetEntity() const;

	void			SetID( int id ) { m_ID = id; }
	void			SetShortDescID( int id ) { m_shortDescID = id; }
	void			SetTipID( int id ) { m_tipID = id; }
	void			SetType( Type type ) { m_type = type; }
	void			SetState( State state );
	void			SetEntity( Entity* pEntity );

	// load and save objective states
	void			Load( IFF& iff );
	void			Save( IFF& iff );

	// friend
	friend class ObjectiveFactory;

private:
	// instance variables
	int				m_ID;
	int				m_shortDescID;
	int				m_tipID;
	Type			m_type;
	State			m_state;
	EntityGroup		m_eGroup;
};
