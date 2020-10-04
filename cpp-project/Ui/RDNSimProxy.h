/////////////////////////////////////////////////////////////////////
// File    : RDNSimProxy.h
// Desc    :
// Created : Monday, April 23, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

// * this class acts as a remote proxy between the taskbar
// * and the simulation
// * it is necessary to mask the delay between requesting a command
// * -such as building a unit- and the execution of that command

// * NOTE: the taskbar should NEVER access the simulation outside
// * of this class

// * NOTE: the taskbar should send ALL commands through this class

#pragma once

#include "../Simulation/GameEventSys.h"

#include <EngineAPI/RTSHud.h>

#include <SimEngine/EntityGroup.h>

// forward declarations
class RDNPlayer;
class RDNWorld;

class SelectionInterface;
class CommandInterface;
class CameraInterface;
class UIInterface;
class FXInterface;

class LuaConfig;

namespace Plat
{
	struct InputEvent;
}

/////////////////////////////////////////////////////////////////////
// RDNSimProxy

class RDNSimProxy : private GameEventSys::Listener,
										private EntityGroup::Observer
{
	// rypes
public:
	enum FailedCommand
	{
		FC_Ok = 0,
		FC_NeedCash = 1,
		FC_BuildQueueFull,
		FC_TooManyUnit,
		FC_Other,
	};

	// construction
public:
	RDNSimProxy(
			LuaConfig *lua,
			SelectionInterface *selection,
			CommandInterface *command,
			CameraInterface *camera,
			UIInterface *ui,
			FXInterface *fx,
			RTSHud *hud,
			const RDNWorld *world,
			const RDNPlayer *player);

	~RDNSimProxy();

	// interface
public:
	// call this every frame (not simstep)
	void Update();

	// called in response to a keyboard/mouse event
	bool Input(const Plat::InputEvent &ie);

	//
	void GetCursorInfo(char *cursor, size_t len, int &ttStrId, const Entity *mouseOverEntity);

	// apply default command on selection
	void DoCommand(const EntityGroup &, bool bQueueCommand);
	void DoCommand(const Vec3f *, unsigned long num, bool bQueueCommand);

	//
	CommandInterface *GetCommand();
	SelectionInterface *GetSelectionInterface();
	CameraInterface *GetCameraInterface();

	// interface -- only for taskbar
public:
	//
	const RDNPlayer *GetPlayer() const;
	const RDNWorld *GetWorld() const;

	const EntityGroup &GetSelection() const;

	// proxy stuff
	const ControllerBlueprint *
	BuildQueueAt(int building, int index) const;
	float BuildQueueBar(int building) const;

	int ValidateBuildUI(const ControllerBlueprint *) const;

	bool GetCachedRally(const Vec3f **const position, const EntityGroup **const target) const;

	// refresh the taskbar based on the current selection
	void SetDirtyFlag();

	// refresh the taskbar without losing the current taskbar context
	void SetRefreshFlag();

	// interface -- exported to lua
public:
	// player
	int LocalPlayer() const;
	int LocalPlayerLabId() const;

	// entity queries
	// returns a *_EC value
	int EntityType(int entityId);

	// returns an ebpid
	int EntityEBP(int entityId);

	// return id of owning player (or zero)
	// NOTE: do NOT use this function for testing ownership
	int EntityOwner(int entityid);

	bool EntityBelongsToPlayer(int entityid) const;

	// NOTE: not all of these functions are valid for all selections
	bool EntityInSpawning(int entityId);

	// unit spawning
	int RockEBP() const;
	int PaperEBP() const;
	int ScissorEBP() const;

	//
	bool UnitCanBeBuiltHere(int building, int ebpid);

	// build queue
	int BuildQueueLength(int id);

	// building engineering
	// return ebp id of specified building type
	int BuildingEBPFromType(int type);

	// building engineering
	// return Type of EBP from network ID
	int TypeFromEBP(int ebpid);

	// commands
	// all these commands use the current selection
	int DoBuildUnit(int ebpid);
	void DoCancelBuildUnit(int unitIndex);
	void DoCommandStop();
	void DoDestroy();
	void DoModalCommand(int mode, float x, float y, float z, int ebpid, bool bAddCommand);

	// inherited -- ModController::Observer
private:
	virtual void OnEvent(const GameEventSys::Event &);

	// inherited -- EntityGroup::Observer
private:
	virtual void Notify_Insertion(Entity *e);
	virtual void Notify_Removal(Entity *e);

	// fields
private:
	class Data;
	Data *m_pimpl;

	// implementation
private:
	void Preload();

	void LuaSetup();
	void LuaReset();

	void CheatSetup();
	void CheatReset();

	void CheatCash(int);
	void CheatKillSelf();

	bool SelectionCanReceiveCommand(int max);

	void Refresh();

	void OnEntityPointCmd(const Vec3f &target, int commandType);
	void OnEntityEntityCmd(const Entity *target);

	// copy -- do not define
private:
	RDNSimProxy(const RDNSimProxy &);
	RDNSimProxy &operator=(const RDNSimProxy &);
};
