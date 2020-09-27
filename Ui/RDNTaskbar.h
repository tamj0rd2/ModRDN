/////////////////////////////////////////////////////////////////////
// File    : RDNTaskbar.h
// Desc    : 
// Created : Monday, April 23, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "../Simulation/GameEventSys.h" 

// forward declaration
class LuaConfig;

class RTSHud;
class CameraInterface;
class SelectionInterface;
class UIInterface;
class SoundInterface;
class FXInterface;

class RDNPlayer;
class RDNWorld;

class Entity;

class RDNSimProxy;
class RDNGhost;
class ControllerBlueprint;
class RDNInputBinder;
class RDNUIProxy;

namespace Plat { enum InputKey; struct InputEvent; };

///////////////////////////////////////////////////////////////////// 
// RDNTaskbar

class RDNTaskbar : private GameEventSys::Listener
{
// types
public:

	// NOTE: if this changes, likely RDNSimProxy::DoModalCommand() also has to change
	enum ModalCommands
	{
		MC_None,
		MC_Move,
		MC_Attack,
		MC_AttackMove,
		MC_SetRallyPoint,
	};

	enum EnableType
	{
		ENABLE_HenchmanKill,
		ENABLE_HenchmanBuild,
		ENABLE_HenchmanAdvancedBuild,

		ENABLE_COUNT
	};

// construction
public:	
	 RDNTaskbar
		(
		LuaConfig*			lua,
		RTSHud*				hud,
		CameraInterface*	camera,
		SelectionInterface*	selection,
		UIInterface*		ui,
		FXInterface*		fx,
		RDNSimProxy*		proxy,
		RDNInputBinder*	inputBinder,
		RDNUIProxy*		uiproxy,
		const RDNGhost*	pGhost
		);

	~RDNTaskbar();

// interface
public:
	// call this every frame (not simstep)
	void			Update( float elapsedSeconds);

	// called in response to a keyboard/mouse event
	bool			Input( const Plat::InputEvent &ie );

	// sets the fow visibility of each entity
	void			UpdateEntityFow( const Entity* e );

	// check fow visibility
	bool			IsEntityVisible( const Entity* e );

	// should only return a string if in modal mode
	void			GetCursorInfoOverride( char* cursor, size_t len, int& ttStrId, const Entity* mouseOverEntity );

	// call this when a tooltip for context cursors need to be displayed
	void			OnIngameTooltip( int ttStrId );

	// returns true if the command queue key (shift) is pressed
	bool			IsCommandQueueKeyPressed();

	void			EnableHud( EnableType type, bool enable );
	bool			IsHudEnabled( unsigned long type );

	void			OnCinematicMode( bool bCinematic );

	// reset game timer
	void			ResetGameTimer( );
	void			SetGameStartTime( float startTime );
	float			GetGameStartTime() const;

	//
	void			SetRevealAll( bool b );
	bool			GetRevealAll() const;

	// reset modal UI 
	void			ModalUIReset();

// interface -- exported to lua
public:
	void	Clear();

	void	PreloadTexture( const char* texture );
	
	void	CreateMinimap( const char* label );
	
	void	BindLabelToText					( const char* label, int textId );
	void	BindLabelToTooltip				( const char* label, const char* tooltipcb );
	void	BindLabelToHotkey				( const char* label, const char* hotkeyLuaName );
	void	BindLabelToTextTimer			( const char* label, int textId, float seconds );
	void	BindLabelToGameTime				( const char* label );
	void	BindLabelToPlayerName           ( const char* label, int playerId );
	void	BindLabelToPlayerCash			( const char* label, const char* tooltipcb, int index, int playerId );
	void	BindLabelToPlayerPop			( const char* label, const char* tooltipcb, int index, int playerId );
	void	BindLabelToPlayerColour			( const char* label, int playerId );
	void	BindLabelToEntityName			( const char* label, int entityId, const char* tooltipcb, int parm );
	void	BindLabelToEntityHealth			( const char* label, int entityId, const char* tooltipcb, int parm );
	void	BindLabelToBuildQueue			( const char* label, int building, int index );
	void	BindLabelToBuildProgress		( const char* label, int building );
	void	BindLabelToResource             ( const char* label, int resource );
	void	BindLabelToEBPName				( const char* label, int ebpid );
	void	BindLabelToEBPCostCash			( const char* label, int ebpid );

	void	BindImageToEntityIcon			( const char* label, int entityId, const char* tooltipcb, int parm );
	void	BindImageToTexture				( const char* label, const char* texture );
											
	void	BindBarToEntityHealth			( const char* bar, int entityId, const char* tooltipcb, int parm );

	void	BindBarToBuildQueue				( const char* bar, int buildingId );

	void	BindHotkey						( const char* hotkeyLuaName, const char* callback, int parm );
											
	void	ShowBitmapLabel					( const char* label );
	void	ShowHud							( const char* hud );
											
	void	BindButton						( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, const char* texture, int parm );
	void	BindButtonDisabled				( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, const char* texture, int parm );
	void	BindButtonToChat                ( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb );
	void	BindButtonToEntity				( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, int entityId );
	void	BindButtonToBuildingEBP			( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, int ebpId );
	void	BindButtonToUnitEBP				( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, int building, int ebpId );
	void	BindButtonToBuildQueue			( const char* button, const char* hotkeyLuaName, const char* callback, const char* tooltipcb, int building, int index, bool enabled );
	void	BindButtonToGroup				( const char* button, const char* callback_left, const char* callback_right, const char* tooltipcb, int groupNb );

	void	BindHudToTooltip				( const char* hud, const char* tooltipcb, int parm0, int parm1 );	

	int		ModalUIBegin( const char* callbackOk, const char* callbackAbort, int mode, int command );
	void	ModalUIEnd  ();

	// return a value from enum RDNSimProxy::FailedCommand
	int		BuildUIBegin( const char* callbackOk, const char* callbackAbort, int ebpid );
	void	BuildUIEnd  ();
	
	void	RallyPointShow( int entityId );
	void	RallyPointHide();
	void	RallyPointUpdate( const Entity *pEntity, const Vec3f& interpPos );

	void	WayPointPathShow( );
	void	WayPointPathHide( );

	bool	IsSelectSimilarPressed( );
	bool	IsSelectSinglePressed( );

	void	CommandQueueEnable( const char* hotkeyLuaName, const char* releaseCallback );

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

	void	ButtonCB( const std::string& str, Plat::InputKey mouseButton );
	void	ButtonLeftCB( const std::string& str );
	void	ButtonRightCB( const std::string& str );

	void	ButtonDispatch( const char* callback, int parm );

	void	ButtonInternal
				( 
				const char* button, 
				const char* hotkeyLuaName, 
				const char* callback_left, 
				const char* callback_right, 
				const char* tooltipcb, 
				const char* texture,
				bool enabled, 
				int parm1,
				int parm2,
				int type
				);

	// removes buttons that where marked to removed by a call
	// to Clear()
	void	RemoveClearedButtons( void );

	void	LabelUpdatePlayerCash	( const char* label, int parm );
	void	LabelUpdatePlayerPop	( const char* label, int parm );

	void	LabelUpdateEntityHealth	( const char* label, int parm );
	void	LabelUpdateEntityName	( const char* label, int entityId );
	
	void	LabelUpdateResource		( const char* label, int parm );

	void	LabelUpdateHenchmanState( const char* label, int parm );

	bool	LabelUpdateTextTimer    ( const char* label, float secondDie );

	void	LabelUpdateBuildQueuePrg( const char* label, int parm );

	void	BarUpdateEntityHealth	( const char* bar, int parm );
	void	BarUpdateBuildQueue		( const char* bar, int parm );

	void	LabelUpdateGameTime		( const char* label );

	void	ButtonUpdateBuilding			( const char* button, int parm0, int parm1 );
	void	ButtonUpdateUnit				( const char* button, int parm0, int parm1 );
	void	ButtonUpdateGroup				( const char* button, int parm0, int parm1 );
	void	ButtonUpdateChat                ( const char* button, int parm0, int parm1 );
	void	ButtonUpdateSelectEntity        ( const char* button );

	void	ModalUiCBAbort();
	void	ModalUiCBClick( Vec3f v, int ebpid );
	void	ModalUiCBTwoClick( Vec3f v1, Vec3f v2, int ebpid );			// callback used for fence building
	void	ModalUiCBPlaceEntity( Matrix43f& position, bool& bCanPlace, const ControllerBlueprint *cbp, bool ) const;
	void	ModalUiCBPlaceFence( const Vec3f& pos1, const Vec3f& pos2, std::vector<Matrix43f>& positionList, std::vector<bool>& canPlaceList, int& canAfford, const ControllerBlueprint* ) const;
	void	ModalUiCBCursorUpdate( unsigned long userData, Vec3f& position, bool& bValidIntersection, Entity* mouseOverEntity );

	void	RallyPointUpdateHelper( const Entity* pEntity, const Vec3f& interpPos );
	void	RallyPointSetPosition( const Vec3f& position, bool bVisible );

	bool	ModalCommandQueueRequest();

	void	OnChildToolTipCB( const std::string&, bool show );

	void	WayPointUpdate();

	void	OnSaveControlGroup	 ( const Entity* pEntity );
	void	OnRestoreControlGroup( const Entity* pEntity );

	void	UnselectHiddenEntities();

	int		GetModalCommandMode();

// copy -- do not define
private:
	RDNTaskbar( const RDNTaskbar& );
	RDNTaskbar& operator= ( const RDNTaskbar& );
};
