/////////////////////////////////////////////////////////////////////
// File    : RDNHUD.h
// Desc    :
// Created : Friday, February 16, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/ModUIEvent.h>
#include <ModInterface/ModSimVis.h>

// forward declaration
class RTSHud;

class CommandInterface;
class SelectionInterface;
class CameraInterface;
class UIInterface;
class SoundInterface;
class FXInterface;
class MessageInterface;

class RDNPlayer;
class Entity;
class EntityGroup;
class RDNTaskbar;
class RDNEventCue;
class RDNGhost;

class IFF;

class LuaConfig;

namespace Plat
{
	struct InputEvent;
};

/////////////////////////////////////////////////////////////////////
// RDNHUD

class RDNHUD : public ModUIEvent, public ModSimVis
{
	// construction
private:
	RDNHUD();
	~RDNHUD();

public:
	// singleton
	static RDNHUD *instance();
	static bool IsInitialized();

	static void Initialize(
			const RDNPlayer *localplayer,
			RTSHud *hud,
			CommandInterface *command,
			SelectionInterface *selection,
			CameraInterface *camera,
			UIInterface *ui,
			SoundInterface *sound,
			FXInterface *fx,
			MessageInterface *message);
	static void Shutdown();

	// interface
public:
	//
	void OnEntityCreate(const Entity *);

	// call this every frame
	void Update(float elapsedSeconds);

	// call this to process input
	// returns false if the event should be passed on
	bool Input(const Plat::InputEvent &ie);

	// name of cursor to display
	const char *GetCursor(const Entity *);

	// default actions
	void DoCommand(const EntityGroup &);
	void DoCommand(const Vec3f *, unsigned long num);

	// retrieve the RDN taskbar
	RDNTaskbar *GetTaskbar();

	// retrieve the event cue system
	RDNEventCue *GetEventCue();

	// retrieve the ghost system
	RDNGhost *GetGhost();

	void UIPause(bool bPause);

	void ShowModOptions();

	// inherited -- ModUIEvent
public:
	virtual void OnHostMigrated(unsigned long idplayer);
	virtual void OnPlayerDrops(unsigned long idplayer);
	virtual void OnCinematicMode(bool bCinematic);
	virtual void OnShowTeamColour(bool bShow);
	virtual void OnResetSM();
	virtual void OnCharacterTalk(unsigned long entityID, bool bTalk);

	// inherited -- ModSimVis
public:
	virtual void EntityVisUpdate(const Entity *, const Vec3f &interpPos, bool bSelected);
	virtual bool EntityVisible(const Entity *e) const;
	virtual void Draw();
	virtual const Array2D<unsigned long> *GetFOWInfo(unsigned long &visiblemask, unsigned long &exploredmask);

	// fields
private:
	class Data;
	Data *m_pimpl;

	// copy -- do not define
private:
	RDNHUD(const RDNHUD &);
	RDNHUD &operator=(const RDNHUD &);
};
