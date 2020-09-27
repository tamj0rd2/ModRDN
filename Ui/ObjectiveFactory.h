/////////////////////////////////////////////////////////////////////
// File    : ObjectiveFactory.h
// Desc    : 
// Created : Friday, September 21, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

//----------------------------------------------------------------------------
// forward class declarations
//----------------------------------------------------------------------------

class Objective;
class IFF;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class ObjectiveFactory
{
public:
	// construction
	ObjectiveFactory();
	virtual ~ObjectiveFactory();

public:
	// interface
	Objective*					CreateObjective( int id );
	Objective*					GetObjective( int id );
	void						DeleteObjective( Objective* pObj );
	void						GetAllObjectives( std::list<Objective*>& objectives );

	// load and save objectives
	void						Load( IFF& iff );
	void						Save( IFF& iff );

// fields
private:
	// pimple idiom
	class Data;
	Data* m_pimpl;
};
