/////////////////////////////////////////////////////////////////////
// File    : RDNEBPs.h
// Desc    :
// Created : Thursday, March 08, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

// forward declaration
class ControllerBlueprint;

/////////////////////////////////////////////////////////////////////
//

class RDNEBP
{
public:
	struct EBPName
	{
		const char *folder;
		const char *file;
	};

	static const EBPName Lab;
	static const EBPName Henchmen;

	static void Preload();

	static const ControllerBlueprint *Get(const EBPName &);
};
