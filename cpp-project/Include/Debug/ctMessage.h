/////////////////////////////////////////////////////////////////////
// File    : ctMessage.h
// Desc    : 
// Created : Tuesday, February 13, 2001
// Author  : 
// 
// FlipCode - Tip Of The Day
//	by Alberto García-Baquero Vega, posted on 11 January 2001 

#pragma once

///////////////////////////////////////////////////////////////////// 
// notes as warnings in compiler output

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") :"

#define FIXME( x )	message( __FILE__LINE__" fixme : " #x "\n" ) 
#define TBD( x )	message( __FILE__LINE__" tbd : " #x "\n" ) 
