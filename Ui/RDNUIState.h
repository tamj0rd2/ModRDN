/////////////////////////////////////////////////////////////////////
// File    : RDNUIState.h
// Desc    : 
// Created : Friday, July 12, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include <SimEngine/EntityGroup.h>

///////////////////////////////////////////////////////////////////// 
// Forward Declarations

class IFF;
class ChunkNode;

///////////////////////////////////////////////////////////////////// 
// 

class RDNUIState
{
// Singleton
public:

	static void				Startup();
	static void				Shutdown();

	static RDNUIState*	i();

// Ctor Dtor
public:
	RDNUIState();
	~RDNUIState();

// types
public:

	enum 
	{
		nHOTKEYGROUPS = 10
	};

// Interface
public:

	inline bool					IsLoaded( ) const;

	inline const Vec3f&			GetCameraTarget( ) const;
	inline float				GetCameraDeclination( ) const;
	inline float				GetCameraRotation( ) const;
	inline float				GetCameraZoom( ) const;

	inline size_t				GetGameSpeed( ) const;

	inline const EntityGroup&	GetSelection( ) const;
	inline const EntityGroup&	GetHotkeyGroup( size_t i ) const;

	void Save( IFF& iff ) const;
	void Load( IFF& iff );

private:

	static unsigned long HandleSUIS( IFF& iff, ChunkNode* , void* pContext1, void* );

// data
private:

	bool	m_bIsLoaded;

	Vec3f	m_camTarget;
	float	m_camDecl;
	float	m_camRotate;
	float	m_camZoom;

	size_t	m_gameSpeed;

	EntityGroup	m_HotkeyGroups[nHOTKEYGROUPS];
	EntityGroup	m_Selection;

};

///////////////////////////////////////////////////////////////////// 
// Inline Access to restored data

inline bool RDNUIState::IsLoaded( ) const
{
	return m_bIsLoaded;
}

inline const Vec3f& RDNUIState::GetCameraTarget( ) const
{
	return m_camTarget;
}

inline float RDNUIState::GetCameraDeclination( ) const
{
	return m_camDecl;
}

inline float RDNUIState::GetCameraRotation( ) const
{
	return m_camRotate;
}

inline float RDNUIState::GetCameraZoom( ) const
{
	return m_camZoom;
}

inline size_t RDNUIState::GetGameSpeed( ) const
{
	return m_gameSpeed;
}

inline const EntityGroup& RDNUIState::GetSelection( ) const
{
	return m_Selection;
}

inline const EntityGroup& RDNUIState::GetHotkeyGroup( size_t i ) const
{
	dbAssert( i < nHOTKEYGROUPS );
	return m_HotkeyGroups[ i ];
}
