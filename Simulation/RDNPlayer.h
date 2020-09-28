/////////////////////////////////////////////////////////////////////
// File    : RDNPlayer.h
// Desc    :
// Created : Wednesday, February 21, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Controllers/ControllerTypes.h"

#include <SimEngine/Player.h>
#include <SimEngine/EntityGroup.h>

#include <EngineAPI/EntityFactory.h>

// forward declaration
class RDNWorld;
class PlayerFOW;
class WorldFOW;
class ResRenewExtInfo;
class DefenseExt;

/////////////////////////////////////////////////////////////////////
// RDNPlayer

class RDNPlayer : public Player
{
	// types
public:
	enum BuildResult
	{
		BR_AllowBuild,
		BR_Never,
		BR_Restricted,
		BR_NeedResourceCash,
		BR_NeedPrerequisite
	};

	enum ResourceIncreased
	{
		RES_Other,
		RES_Resourcing,
		RES_Refund
	};

	enum KillPlayerReason
	{
		KPR_UnusedPlayer,
		KPR_Trigger,
		KPR_NetworkOutOfSync,
		KPR_NetworkAbort,
		KPR_NetworkDisconnected,
		KPR_NetworkKickedOut,

		KPR_COUNT
	};

	enum BonusOp
	{
		BonusOp_Remove = 0,
		BonusOp_Add
	};

	class Observer;

	// construction
public:
	RDNPlayer(WorldFOW *pWorldFOW);
	~RDNPlayer();

	// interface
public:
	// race
	void SetRace(size_t race);
	size_t GetRace() const;

	// race-based info
	float GetRaceBonusCost(const ControllerBlueprint *pCBP) const;
	float GetRaceBonusHealthMax(const ControllerBlueprint *pCBP) const;
	float GetRaceBonusSpeed(const ControllerBlueprint *pCBP) const;

	// hq
	const Entity *GetHQEntity() const;
	Entity *GetHQEntity();
	const Vec3f &GetStartingPosition() const;

	PlayerFOW *GetFogOfWar();
	inline const PlayerFOW *
	GetFogOfWar() const;

	// resource
	float GetResourceCash() const;
	float IncResourceCash(float amount, ResourceIncreased reason = RES_Other);
	float DecResourceCash(float amount);
	float GetResourceCashRatePerTick() const;

	// population
	int PopulationCurrent() const;
	int PopulationMax() const;

	// current pop + all units in construction
	int PopulationTotal() const;

	// check resources, tech, prerequisites - pass in the resource amounts to compare with (AI uses budgets)
	BuildResult BlueprintCanBuild(const ControllerBlueprint *pCBP) const;

	// use this function to disallow a specific ebp
	void BlueprintOverride(const ControllerType &ct, bool bAdd);

	//
	bool GetBlueprintOverride(const ControllerType ct) const;

	// fog of war
	// call to see if this player sees this entity
	bool FoWIsVisible(const Entity *entity) const;
	bool FoWIsVisible(const Player *player, const ControllerBlueprint *pCBP, const Matrix43f &transform) const;

	// call this every sim steps
	void FoWUpdate(const RDNWorld *w);

	// call this to update player every sim step
	void Update();

	// call this when a player has successfully tagged an enemy creature
	void AddTaggedEntity(Entity *e);
	// call this when a tag has been removed from a creature
	void RemoveTaggedEntity(Entity *e);

	int GetStructureBudget(const ControllerBlueprint *cbp) const;

	//---------------------------------------
	// Access to specific sub-groups

	const EntityGroup &
	GetEntityGroup(int type) const;

	// get the number of entities of this type that are active at this time
	size_t GetNumEntity(int type) const;

	// get the number of entities built of this type in total
	// if this is a building type, only account for buildings that are completely built
	size_t GetNumEntityTotal(int type) const;

	// get the number of entities that are alive (have non-zero health)
	size_t GetNumLiveEntity(int type) const;

	void SpawnEntity(Entity *);

	void DeSpawnEntity(Entity *);

	// inherited -- Player
public:
	virtual void SetName(const wchar_t *name);

	virtual void AddEntity(Entity *e);
	virtual void RemoveEntity(Entity *e);

	virtual bool CanControlEntity(const Entity *e) const;

	virtual void PreFirstSimulate();

	// the 'reason' should be one of enum KillPlayerReason
	virtual void KillPlayer(int reason);

	virtual void CommandDoProcess(
			const unsigned int cmd,
			const unsigned long param,
			const unsigned int flags,
			Player *sender);

	virtual void CommandDoProcess(
			const unsigned int cmd,
			const unsigned long param,
			const unsigned int flags,
			Player *sender,
			const EntityGroup &entities,
			const Vec3f *pos,
			const size_t posCount);

	// Save and Load functions.  You MUST call the base class version of this function first
public:
	virtual void Save(IFF &iff) const;
	virtual void Load(IFF &iff);

	// Chunk Handlers for the loading code
	static unsigned long HandleSPFC(IFF &, ChunkNode *, void *, void *);
	static unsigned long HandleSPDT(IFF &, ChunkNode *, void *, void *);

	// Observer system
public:
	void AddObserver(Observer *p) const;
	void RemoveObserver(Observer *p) const;

	// types
private:
	typedef std::set<ControllerType> BPOverrideList;

	// fields
private:
	size_t m_race;

	float m_resourceCash;

	Vec3f m_hqPosition;

	PlayerFOW *m_pFOW;

	// population
	// do not save that value
	int m_population;

	float m_cashPerTick;

	// a group for each type of controller - for faster and easier ways to find objects
	// do not save that value
	EntityGroup m_groupController[MAX_EC];

	typedef std::set<Observer *>
			PlayerObserverList;

	mutable PlayerObserverList
			m_observers;
	// implementation
private:
	void CmdCheatCash(Player *sender, unsigned long);
	void CmdCheatKillSelf(Player *sender);

	float CalcRateForAllCashPilePerTick() const;
};

inline const PlayerFOW *RDNPlayer::GetFogOfWar() const
{
	return const_cast<RDNPlayer *>(this)->GetFogOfWar();
}

////////////////////////////////////////////////////////////////////
// RDNPlayer::Observer

class RDNPlayer::Observer
{
public:
	virtual void OnIncResourceCash(const RDNPlayer *, float amount, RDNPlayer::ResourceIncreased reason) = 0;
	virtual void OnDecResourceCash(const RDNPlayer *, float amount) = 0;

	virtual void OnAddEntity(const RDNPlayer *, const Entity *entity) = 0;
	virtual void OnRemoveEntity(const RDNPlayer *, const Entity *entity) = 0;

	// optional i/f
	virtual void OnReSpawnEntity(const RDNPlayer *, const Entity *) {}
	virtual void OnDeSpawnEntity(const RDNPlayer *, const Entity *) {}
};
