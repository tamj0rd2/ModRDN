/////////////////////////////////////////////////////////////////////
// File    : Modifier.cpp
// Desc    : 
// Created : Tuesday, March 26, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"

#include "Modifier.h"

#include <Memory/MemorySmall.h>

///////////////////////////////////////////////////////////////////// 
// 

static MemPoolSmall  s_poolsmall( "MOD-Modifiers" );

///////////////////////////////////////////////////////////////////// 
// 

void* Modifier::operator new( size_t size )
{
	return s_poolsmall.Alloc( size );
}

void Modifier::operator delete( void* ptr, size_t size )
{
	s_poolsmall.Free( ptr, size );
}

///////////////////////////////////////////////////////////////////// 
// 

Modifier::~Modifier()
{
}
