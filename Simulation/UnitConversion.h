/////////////////////////////////////////////////////////////////////
// File    : UnitConversion.h
// Desc    : 
// Created : Thursday, June 21, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

///////////////////////////////////////////////////////////////////// 
// This file contains useful conversion macros


///////////////////////////////////////////////////////////////////// 
//	
//	This function will convert from a totalTime, and totalValue over time
//	To the number of ticks the time results in (truncating the decimal)
//	and the value per tick, inlcuding the fractional ammount that was lost
//	when the decimal was truncated.
//
//	i.e.	go from 1.1 secs, and 10 damage
//			to	8 ticks and 2.500 damage per tick
//	
inline
void TimeValue_To_TicksValuePerTick
		(
		const float		ticksPerSecond,
		const float		totalTime,
		const float		totalValue,
		long&			outTickCount,
		float&			outValuePerTick
		)
{
	float numTicks		= totalTime*ticksPerSecond;
	float valuePerTick	= totalValue / numTicks;

	float flrTicks = floorf( numTicks );

	outValuePerTick = valuePerTick + ( valuePerTick*(numTicks-flrTicks) / flrTicks );
	outTickCount = long( flrTicks );
}

inline
void ValuePerSec_To_ValuePerTick
		(
		const float		ticksPerSecond,
		const float		valuePerSec,
		float&			outValuePerTick
		)
{
	outValuePerTick = valuePerSec / ticksPerSecond;
}

inline
void TimeSec_To_NumTicks
		(
		const float		ticksPerSecond,
		const float		timeSeconds,
		long&			outValuePerTick
		)
{
	outValuePerTick = long( timeSeconds * ticksPerSecond );
}

inline
void NumTicks_To_TimeSec
		(
		const float		ticksPerSecond,
		const long		numTicks,
		float&			timeSeconds
		)
{
	timeSeconds = float(numTicks) / ticksPerSecond;
}
