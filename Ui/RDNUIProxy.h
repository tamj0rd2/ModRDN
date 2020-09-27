/////////////////////////////////////////////////////////////////////
// File    : RDNUIProxy.h
// Desc    : 
// Created : Wednesday, January 30, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include "../Simulation/GameEventSys.h" 

// forward declaration
class RDNSimProxy;

class RTSHud;
class SelectionInterface;
class CameraInterface;
class UIInterface;
class MiniMap;
class FXInterface;
class SoundInterface;
class MessageInterface;
struct RDNUIMsg;

class RDNInputBinder;
class RDNUIOptions;

class LuaConfig;

///////////////////////////////////////////////////////////////////// 
// RDNUIProxy

class RDNUIProxy : private GameEventSys::Listener
{
// types
public:
	enum ChatAllowedResult
	{
		CHATALLOW_Ok,
		CHATALLOW_NotMP,
		CHATALLOW_Dead,
		CHATALLOW_NoLocal,
		CHATALLOW_COPPA
	};

// construction
public:
	 RDNUIProxy
		(
		LuaConfig*			lua,
		RTSHud*				rts,
		SelectionInterface*	selection,
		CameraInterface*	camera,
		UIInterface*		ui,
		FXInterface*		fx,
		SoundInterface*		sound,
		MessageInterface*	message,
		RDNSimProxy*		sim,
		RDNInputBinder*		inputBinder,
		RDNUIOptions*		uiOptions
		);
	~RDNUIProxy();

// interface
public:
	// call this every frame (not simstep)
	void		Update();

	//
	void		OnCinematicMode( bool bCinematic );
	void		OnCharacterTalk( unsigned long entityID, bool bTalk );

// interface -- exported to lua
public:
	// sound
	void		PlaySound( const char* sound );

	// selection 
	int			SelectionCount() const;

	bool		SelectionBelongsToPlayer() const;
	bool		SelectionIsEnemy() const;
	bool		SelectionIsAlly() const;

	bool		SelectionAllSameType() const;

	bool		SelectionHasSpecialCommand	( int triggeredability ) const;
	bool		SelectionHasAttackType      ( int attacktype ) const;

	int			SelectionId( int selndx ) const;

	int			GetRangeAttackCount( int ebpid );

	void		SelectEntity( int entityId, int actOnSimilar );
	void		DeSelectEntity( int entityId, int actOnSimilar );
	void		DeSelectAll();

	void		SelectHotkeyGroup( int groupNb );
	void		AssignHotkeyGroup( int groupNb );
	void		UnassignFromAllHotkeyGroups();

	void		SelectAllUnitsOnScreen();
	void		SelectAllUnitsInWorld();
	void		SelectHQ();
	void		SelectNextSubSelect();

	// ui functions
	void		PauseMenuShow();

	// return type is an enum of type ChatAllowedResult
	int			ChatAllowed() const;
	void		ChatShow   ();

	bool		FastSpeedAllowed() const;
	bool		IsFastSpeed() const;
	void		SetFastSpeed( bool );

	void		ModOptionsShow();

	void		BuildButtonPressed();
	void		BuildEBPButtonPressed( long ebpid );

	// camera
	void		FocusOnEntity   ( int entityId, bool focusOnEntity, bool jump );
	void		FocusOnSelection();
	void		ZoomCameraMouse	( float ammount );

	void		LoadUIOptions( );
	
// inherited -- GameEventSys::Listener
private:
	virtual void OnEvent( const GameEventSys::Event& );
						
// fields
private:
	class Data;
	Data* m_pimpl;

// implementation
private:
	void	Preload();

	void	LuaSetup();
	void	LuaReset();

	void	DlgModOptionsClose();

	void	SendMessageToAll( const unsigned char* msg, unsigned int msgLen ) const;

	void	DispatchMessage( const std::vector<unsigned char>& msg, unsigned long sender );


// copy -- do not define
private:
	RDNUIProxy( const RDNUIProxy& );
	RDNUIProxy& operator= ( const RDNUIProxy& );
};
