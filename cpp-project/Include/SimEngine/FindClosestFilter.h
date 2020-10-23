/////////////////////////////////////////////////////////////////////
// File    : FindClosestFilter.h
// Desc    : 
// Created : Thursday, April 26, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FindClosest Entity Filter predicates, used to filter out unwanted entities.
// Used when calling the World::FindClosest function.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FindClosestFilter
{
public:
	virtual bool Check( const Entity* ) = 0;
};

class NullFilter : public FindClosestFilter
{
public:
	bool Check( const Entity* ) {return true;}
};