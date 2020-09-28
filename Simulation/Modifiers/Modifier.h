/////////////////////////////////////////////////////////////////////
// File    : Modifier.h
// Desc    :
// Created : Wednesday, June 27, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

/////////////////////////////////////////////////////////////////////
// Forward Declarations

class ModController;
class IFF;

/////////////////////////////////////////////////////////////////////
// Modifier base class, pure virtual

class Modifier
{
	// types
public:
	/////////////////////////////////////////////////////////////////////
	// The order of this list is VERY important for the savegame.
	// Make sure to not

	enum ModifierType
	{
		MID_Begin = 1,

		MID_Frenzy = 1,
		MID_Leap,
		MID_Charge,
		MID_Plague,
		MID_Poison,
		MID_Sabotage,
		MID_SonicDmg,
		MID_Stealth,
		MID_Stink,
		MID_StinkProtect,
		MID_VenomSpray,
		MID_Loner,
		MID_AutoDefense,
		MID_Webbed,
		MID_Flash,
		MID_Infested,
		MID_Swamp,
		MID_SoiledLand,
		MID_SoiledDmg,

		MID_Count,
	};

	// overides for new and delete, all modifiers are allocated using a small pool
public:
	static void *operator new(size_t size);
	static void operator delete(void *ptr, size_t size);

	// interface
public:
	virtual ~Modifier();

	virtual ModifierType GetModifierType() const = 0;

	// returns true when the modifier is finished it's stuff
	virtual bool Execute(ModController *) = 0;

	// these functions are called each time a Modifier is activated or deactivated
	virtual void Init(ModController *) = 0;
	virtual void Shut(ModController *) = 0;

	// these functions are called each time a Modifier is activated or deactivated
	virtual void Save(IFF &) const = 0;
	virtual void Load(IFF &, ModController *) = 0;
};