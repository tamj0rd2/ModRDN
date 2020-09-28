/////////////////////////////////////////////////////////////////////
// File    : AttackMemory.h
// Desc    :
// Created : Thursday, February 14, 2002
// Author  : dswinerd
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <SimEngine/EntityGroup.h>
#include <SimEngine/EntityMemory.h>

/////////////////////////////////////////////////////////////////////
//	Forward Declarations:
//
class Entity;
class Player;

/////////////////////////////////////////////////////////////////////
//	Class	: AttackMemory
//	Desc.	: Responsible for remembering who attacked an Entity and when
//
class AttackMemory
{
	// construction
public:
	AttackMemory();
	~AttackMemory();

	// interface
public:
	void SetAttackedBy(Entity *pEntity, long time);

	bool WasAttackedBy(Entity *pEntity, long time) const;

	bool WasAttackedBy(Player *pPlayer, long time) const;

	bool HasBeenAttackedSince(long time) const;

	void GetAttackers(EntityGroup &attackers, long time) const;

	// fields
private:
	EntityMemory m_memory;

	// copy
private:
	AttackMemory(const AttackMemory &);
	AttackMemory &operator=(const AttackMemory &);
};
