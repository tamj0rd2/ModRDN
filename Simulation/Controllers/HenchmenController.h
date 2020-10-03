/////////////////////////////////////////////////////////////////////
// File    : HenchmenController.h
// Desc    :
// Created :
// Author  :
//
// (c) 2003 Relic Entertainment Inc.
//

#pragma once

#include "ModController.h"

#include "../CommandProcessor.h"

#include "../States/StateIdle.h"
#include "../States/StateDead.h"
#include "../States/StateMove.h"
#include "../States/StateAttack.h"
#include "../States/StateAttackMove.h"
#include "../States/StateGroupMove.h"
#include "../States/StatePause.h"

#include "../Extensions/HealthExt.h"
#include "../Extensions/ModifierExt.h"
#include "../Extensions/MovingExt.h"
#include "../Extensions/SightExt.h"
#include "../Extensions/AttackExt.h"

#include "../ExtInfo/HealthExtInfo.h"
#include "../ExtInfo/MovingExtInfo.h"
#include "../ExtInfo/AttackExtInfo.h"
#include "../ExtInfo/SightExtInfo.h"
#include "../ExtInfo/CostExtInfo.h"
#include "../ExtInfo/UIExtInfo.h"

#include "../UnitConversion.h"
#include "../RDNWorld.h" // for k_SimStepsPerSecond

// forward declarations
class Entity;
class SimEntity;
class EntityGroup;
class ChunkNode;

class Vec3f;

///////////////////////////////////////////////////////////////////////////////
// HenchmenController

class HenchmenController : private HealthExt,
													 private ModifierExt,
													 private MovingExt,
													 private SightExt,
													 private AttackExt,
													 public ModController
{
	// types
public:
	class StaticInfo : private MovingExtInfo,
										 private AttackExtInfo,
										 private HealthExtInfo,
										 private SightExtInfo,
										 private CostExtInfo,
										 private UIExtInfo,
										 public ModStaticInfo
	{
	public:
		StaticInfo(const ControllerBlueprint *);

	public:
		virtual const ModStaticInfo::ExtInfo *QInfo(unsigned char id) const;
	};

	// construction
public:
	HenchmenController(Entity *pEntity, const ECStaticInfo *);
	virtual ~HenchmenController();

	// inherited -- EntityController
public:
	virtual bool Update(const EntityCommand *currentCommand);

	virtual void Execute();

	virtual Extension *QI(unsigned char InterfaceID);

	virtual State *QIActiveState(unsigned char StateID);

	// inherited -- ModController
private:
	virtual Extension *QIAll(unsigned char);

	virtual State *QIStateAll(unsigned char StateID);

	virtual void SetActiveState(unsigned char StateID);

	// Save and Load functions
public:
	virtual void Save(BiFF &) const;
	virtual void Load(IFF &);

	// Data
private:
	// State stuff for Guy.
	State *m_CurrentState;

	StateIdle m_stateidle;
	StateMove m_statemove;
	StateAttack m_stateattack;
	StateDead m_statedead;
	StateAttackMove m_stateattackmove;
	StateGroupMove m_stategroupmove;
	StatePause m_statepause;

	CommandProcessor m_commandproc;

	bool m_bFirstUpdate;

	// Chunk Handlers
private:
	static unsigned long HandleGYEC(IFF &, ChunkNode *, void *, void *);

	// inherited -- Extension
private:
	virtual ModController *GetSelf();

	// inherited -- AttackExt
private:
	virtual void OnDoDamageTo(float damagePerHit, float damageBonus, const AttackPackage &attack, Entity *pTarget);
	virtual void OnDoTriggeredAttack(const AttackPackage &attack, Entity *pTarget);

	// inherited -- HealthExt
private:
	virtual void NotifyHealthGone();
	virtual void OnApplyDamage(const float amountdone, const DamageType type);
};
