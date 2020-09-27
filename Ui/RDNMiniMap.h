/////////////////////////////////////////////////////////////////////
// File    : RDNMiniMap.h
// Desc    : 
// Created : Monday, April 6, 2001
// Author  : Shelby Hubick
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <EngineAPI/MiniMap.h>

// forward declaration
class RDNPlayer;
class RDNGhost;
class CameraInterface;
class SelectionInterface;

class Entity;
class EntityController;

namespace Plat
{
	struct InputEvent;
};

///////////////////////////////////////////////////////////////////// 
// RDNHUD

class RDNMiniMap
{
// construction
public:				
	 RDNMiniMap( MiniMap*, const RDNPlayer*, CameraInterface*, SelectionInterface* selection, const RDNGhost* );
	~RDNMiniMap();

public:

	void	Update( float elapsedSeconds);

	void	SetModalClickCapture( bool bCapture );

	void	UpdateEntityFow( const Entity* e );

	bool	IsEntityVisible( const Entity* e );

	void	SetRevealAll( bool b );
	bool	GetRevealAll() const;
							
// fields
private:
	class Data;
	Data* m_pimpl;

private:
		
	// ---- BUTTON CALLBACKS

	void				OnRightClick( );
	void				OnLeftClick( );

	//---- INIT HELPER FUNCTIONS -----

	void				RegisterCallbacksToButtons();

	//---- UPDATE HELPERS -----

	void				UpdatePoints( float elapsedSeconds );
	void				UpdateBlips( float elapsedSeconds );
	void				UpdateFog( float elapsedSeconds );
	
	void				AddWorldEntities( );
	void				AddLocalEntities( );

	void				AddEntityPoint( const EntityController* pController, bool bSelected );
};
