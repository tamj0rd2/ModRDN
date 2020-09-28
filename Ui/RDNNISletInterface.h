/////////////////////////////////////////////////////////////////////
// File    : RDNNISletInterface.h
// Desc    :
// Created : Thursday, Jan 03, 2002
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/NISletInterface.h>

class RDNNISletInterface : public NISletInterface
{
public:
	// singleton interface

	static bool Initialize(void);
	static void Shutdown(void);

	static RDNNISletInterface *Instance(void);

public:
	// inherited from NISletInterface
	virtual void Teleport(const EntityGroup &eg, const Vec2f &destination);

private:
	RDNNISletInterface();
};