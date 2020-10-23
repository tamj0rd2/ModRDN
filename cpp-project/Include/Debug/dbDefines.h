/////////////////////////////////////////////////////////////////////
// File    : dbDefines.h
// Desc    : This file is a global file that contains defines that
//			 will conditionally compile in debug code.
//			 By default this code is compiled out in Release builds,
//			 but may be included in release if needed.
// Created : Wednesday, June 13, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//


///////////////////////////////////////////////////////////////////////////////
//
//	Define Naming convention:
//
//	If adding a define for a new DLL or Lib or EXE put a new comment block
//	that contains the name of the DLL or lib and a quick description.
//
//	Use a common naming prefix for all defines in that lib or DLL.  All
//	defines should be in CAPITAL letters.  Also include a brief comment
//	that describes what the define will compile in/out.
//
//	e.g.	Debug DLL name prefix DB_
//
//	to enable the debug Address LRU define DB_DEBUGADDRLRU
//

///////////////////////////////////////////////////////////////////// 
// When RELIC_RTM is defined all code that should not ship with the
// game should be compiled out.  So you should use something like:
//
//	#if !defined( RELIC_RTM )
//		... code that should NOT be included in an RTM Build
//	#endif
//

// #define RELIC_RTM

//	Usage of the defines:
//
//	To use the defines in code, you just need to surround the code with
//	#if defined( <definename> )
//		line of code;
//		line of code;
//	#endif
//

///////////////////////////////////////////////////////////////////// 
// This define is used when you want to build the Demo on you system
// #define ICPC_DEMO

#if !defined( NDEBUG )
///////////////////////////////////////////////////////////////////////////////
// DEBUG BUILD, or more accuratly NOT RELEASE
///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// Debug DLL defines

	#define DB_DEBUGADDRLRU				// This will enable the DEBUG LRU address cacheing scheme, uses lots o ram

	///////////////////////////////////////////////////////////////////////////////
	// Blueprint Lib defines

	///////////////////////////////////////////////////////////////////////////////
	// Game Exe defines

	///////////////////////////////////////////////////////////////////////////////
	// GameEngine defines

#else
///////////////////////////////////////////////////////////////////////////////
// RELEASE BUILD
///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// Debug DLL defines

	// #define DB_DEBUGADDRLRU			// This will enable the DEBUG LRU address cacheing scheme, uses lots o ram

#endif
