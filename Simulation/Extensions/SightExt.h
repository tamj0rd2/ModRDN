/////////////////////////////////////////////////////////////////////
// File    : SightExt.h
// Desc    : 
// Created : Thursday, June 28, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

class SightExtInfo;

///////////////////////////////////////////////////////////////////////////////
// SightExt

class SightExt : public Extension
{
// types
public:
	enum
	{
		ExtensionID = EXTID_Sight,
	};

// interface
public:
	//
	virtual float	GetSightRadius() const;

	bool			SameAsLastFOWPos( size_t cx, size_t cz ) const;
	size_t			GetLastFOWXPos( ) const;
	size_t			GetLastFOWZPos( ) const;
	void			SetLastFOWPos( size_t cx, size_t cz );

	float			GetLastFOWRad() const;
	void			SetLastFOWRad( const float );

	bool			GetInFOW() const;
	void			SetInFOW( bool inFOW );

// inherited interface: Extension
private:

	virtual void SaveExt( BiFF& ) const;
	virtual void LoadExt( IFF& );

	// Chunk Handlers
	static unsigned long HandleESGT( IFF&, ChunkNode*, void*, void* );

// construction
protected:
	SightExt( const SightExtInfo* );

// fields
private:
	float			m_sightRadius;

	size_t			m_FOWPosX, m_FOWPosZ;
	float			m_FOWRad;
	bool			m_bInFOW;
};

///////////////////////////////////////////////////////////////////// 
// Inline functions

inline bool SightExt::SameAsLastFOWPos( size_t cx, size_t cz ) const
{
	return ( cx == m_FOWPosX && cz == m_FOWPosZ );
}

inline size_t SightExt::GetLastFOWXPos( ) const
{
	return m_FOWPosX;
}

inline size_t SightExt::GetLastFOWZPos( ) const
{
	return m_FOWPosZ;
}

inline void SightExt::SetLastFOWPos( size_t cx, size_t cz )
{
	m_FOWPosX = cx;
	m_FOWPosZ = cz;
}

inline float SightExt::GetLastFOWRad() const
{
	return m_FOWRad;
}

inline void SightExt::SetLastFOWRad( const float newRad )
{
	m_FOWRad = newRad;
}

inline bool SightExt::GetInFOW() const
{
	return m_bInFOW;
}

inline void SightExt::SetInFOW( bool inFOW )
{
	m_bInFOW = inFOW;
}