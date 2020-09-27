/////////////////////////////////////////////////////////////////////
// File    : FOWTypes.h
// Desc    : 
// Created : Tuesday, January 08, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

enum FOWChannelType
{
	FOWC_Explored		= 0x01,
	FOWC_Visible		= 0x02,
};

typedef unsigned long PlayerFOWID;

typedef unsigned char FOWChannelMask;

inline FOWChannelMask CreateFOWMask( FOWChannelType c1 )
{
	return FOWChannelMask( c1 );
}

inline FOWChannelMask CreateFOWMask( FOWChannelType c1, FOWChannelType c2 )
{
	return FOWChannelMask( c1 | c2 );
}

inline FOWChannelMask CreateFOWMask( FOWChannelType c1, FOWChannelType c2, FOWChannelType c3 )
{
	return FOWChannelMask( c1 | c2 | c3 );
}

inline FOWChannelMask CombineFOWMasks( FOWChannelMask mask1, FOWChannelMask mask2 )
{
	return FOWChannelMask( mask1 | mask2 );
}

inline bool TestFOWMask( FOWChannelMask mask, FOWChannelType chan )
{
	return (mask & chan) != 0;
}