/////////////////////////////////////////////////////////////////////
// File    : RDNEBPs.cpp
// Desc    : 
// Created : Thursday, March 08, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h" 
#include "RDNEBPs.h" 

#include "../ModObj.h" 

#include <EngineAPI/EntityFactory.h>

///////////////////////////////////////////////////////////////////// 
// RDNEBP

const RDNEBP::EBPName RDNEBP::HQ		= { "structures",	"HQ" };
const RDNEBP::EBPName RDNEBP::CashPile	= { "structures",	"cashpile" };
const RDNEBP::EBPName RDNEBP::Rock		= { "units",		"rock" };
const RDNEBP::EBPName RDNEBP::Paper		= { "units",		"paper" };
const RDNEBP::EBPName RDNEBP::Scissor	= { "units",		"scissor" };

void RDNEBP::Preload()
{
#define LOAD(t) \
	ModObj::i()->GetEntityFactory()->GetControllerBP( t.folder, t.file );
	
	LOAD( HQ );
	LOAD( CashPile );
	LOAD( Rock );
	LOAD( Paper );
	LOAD( Scissor );

#undef LOAD

	return;
}

const ControllerBlueprint* RDNEBP::Get( const EBPName& t )
{
	return ModObj::i()->GetEntityFactory()->GetControllerBP( t.folder, t.file );
}
