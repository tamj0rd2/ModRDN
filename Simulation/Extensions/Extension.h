/////////////////////////////////////////////////////////////////////
// File    : Extension.h
// Desc    : 
// Created : Tuesday, February 13, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//
#pragma once

// forward declaration
class ModController;
class BiFF;
class IFF;
class ChunkNode;

///////////////////////////////////////////////////////////////////////////////
//  Extension 

// NOTE: this class MUST remain a pure virtual base class, otherwise we'll have
// serious problems

// NOTE: derived class must NEVER inherit from Extension more than once

class Extension
{
// construction
protected:
	~Extension()
	{
	}

public:
	// Must be implemented by the extension, even if the implementation does nothing.
	virtual void SaveExt( BiFF& ) const = 0;
	virtual void LoadExt( IFF& ) = 0;

protected:
	virtual ModController* GetSelf() = 0;
	
	inline const ModController* GetSelf() const;

};


///////////////////////////////////////////////////////////////////// 
// Inline Functions

inline const ModController* Extension::GetSelf() const
{
	return (const_cast<Extension*>(this))->GetSelf();
}
