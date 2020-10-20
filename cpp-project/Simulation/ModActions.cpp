/////////////////////////////////////////////////////////////////////
// File    : ModActions.cpp
// Desc    :
// Created : Thursday, June 14, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "RDNWorld.h"
#include "RDNPlayer.h"
#include "PlayerFOW.h"

#include "CommandTypes.h"
#include "ModTriggerTypes.h"

#include "ModObj.h"

#include "Controllers/ControllerTypes.h"
#include "Controllers/ModController.h"

#include "Extensions/HealthExt.h"
#include "Extensions/SightExt.h"

#include "ExtInfo/MovingExtInfo.h"
#include "ExtInfo/SiteExtInfo.h"
#include "ExtInfo/CostExtInfo.h"

#include "States/StateAttack.h"
#include "States/StateDead.h"

#include "RDNQuery.h"

#include "../UI/BlipFactory.h"
#include "../UI/Objective.h"
#include "../UI/ObjectiveFactory.h"
#include "../UI/RDNEntityFilter.h"

#include <SimEngine/TExpression.h>
#include <SimEngine/Entity.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/SpatialBucket.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <EngineAPI/TriggerFactory.h>
#include <EngineAPI/EntityFactory.h>
#include <EngineAPI/CommandInterface.h>
#include <EngineAPI/SelectionInterface.h>

#include <Util/Random.h>
#include <Util/iff.h>
#include <Util/biff.h>

//------------------------------------------------------------------
// OrderGroupToLocationAction
//------------------------------------------------------------------

class OrderGroupToLocationAction : public TExpression
{
public:
	enum
	{
		ARG_ENTITY
	};
	//------------------------------------------
	TEXPRESSION_CLONE(OrderGroupToLocationAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new OrderGroupToLocationAction; }
};

//------------------------------------------------------------------
// OrderGroupToStopAction
//------------------------------------------------------------------

class OrderGroupToStopAction : public TExpression
{
public:
	enum
	{
		ARG_GROUP
	};
	//------------------------------------------
	TEXPRESSION_CLONE(OrderGroupToStopAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new OrderGroupToStopAction; }
};

//------------------------------------------------------------------
// OrderGroupToMoveRandomlyAction
//------------------------------------------------------------------

class OrderGroupToMoveRandomlyAction : public TExpression
{
public:
	enum
	{
		ARG_PERCENT,
		ARG_GROUP,
		ARG_DIST
	};
	//------------------------------------------
	TEXPRESSION_CLONE(OrderGroupToMoveRandomlyAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new OrderGroupToMoveRandomlyAction; }
};

//------------------------------------------------------------------
// Group2GroupAction
//------------------------------------------------------------------

class Group2GroupAction : public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(Group2GroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new Group2GroupAction; }
};

//------------------------------------------------------------------
// OrderGroupToDoCommandAction
//------------------------------------------------------------------

class OrderGroupToDoCommandAction : public TExpression
{
public:
	enum
	{
		ARG_GROUP,
		ARG_ENTITYCMD,
		ARG_ENTITY
	};
	//------------------------------------------
	TEXPRESSION_CLONE(OrderGroupToDoCommandAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new OrderGroupToDoCommandAction; }
};

//------------------------------------------------------------------
// GiveCashAction
//------------------------------------------------------------------

class GiveCashAction : public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(GiveCashAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GiveCashAction; }
};

//------------------------------------------------------------------
// SetCashAction
//------------------------------------------------------------------

class SetCashAction : public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(SetCashAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SetCashAction; }

private:
	void setPlayerCash(RDNPlayer *player, float renewAmount);
};

//----------------------------------------------------------------------------------------------
// LoseAction Class
//----------------------------------------------------------------------------------------------

class LoseAction : public TExpression
{
public:
	enum
	{
		ARG_PLAYER
	};
	//------------------------------------------
	TEXPRESSION_CLONE(LoseAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new LoseAction; }
};

//----------------------------------------------------------------------------------------------
// WinAction Class
//----------------------------------------------------------------------------------------------

class WinAction : public TExpression
{
public:
	enum
	{
		ARG_PLAYER
	};
	//------------------------------------------
	TEXPRESSION_CLONE(WinAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new WinAction; }
};

//------------------------------------------------------------------
// SpawnEntityAction Class
//------------------------------------------------------------------

class SpawnEntityAction : public TExpression
{
public:
	TEXPRESSION_CLONE(SpawnEntityAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SpawnEntityAction; }
};

//----------------------------------------------------------------------------------------------
// GetEBPTypeGroupAction Class
//----------------------------------------------------------------------------------------------

class GetEBPTypeGroupAction : public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(GetEBPTypeGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return true; }
	//------------------------------------------
	static TExpression *Create() { return new GetEBPTypeGroupAction; }

private:
	void addEBPToGroup(Player *player, EntityGroup &group, long ebpNetID);
};

//----------------------------------------------------------------------------------------------
// GetAllGroupAction Class
//----------------------------------------------------------------------------------------------

class GetAllGroupAction : public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(GetAllGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return true; }
	//------------------------------------------
	static TExpression *Create() { return new GetAllGroupAction; }

private:
	void addPlayerEntitiesToGroup(Player *player, EntityGroup &group);
};

//------------------------------------------------------------------
// SetGroupOwner
//------------------------------------------------------------------

class SetGroupOwnerAction : public TExpression
{
public:
	TEXPRESSION_CLONE(SetGroupOwnerAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SetGroupOwnerAction; }

private:
	void Unselect(const Entity *pEntity);
	void StopFriendlyFire(Player *pPlayer);
};

//------------------------------------------------------------------
// CreatePrimObjectiveAction
//------------------------------------------------------------------

class CreatePrimObjectiveAction : public TExpression
{
public:
	TEXPRESSION_CLONE(CreatePrimObjectiveAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new CreatePrimObjectiveAction; }
};

//------------------------------------------------------------------
// CreateSecObjectiveAction
//------------------------------------------------------------------

class CreateSecObjectiveAction : public TExpression
{
public:
	TEXPRESSION_CLONE(CreateSecObjectiveAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new CreateSecObjectiveAction; }
};

//------------------------------------------------------------------
// BindObjectiveToLocAction
//------------------------------------------------------------------

class BindObjectiveToLocAction : public TExpression
{
public:
	TEXPRESSION_CLONE(BindObjectiveToLocAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new BindObjectiveToLocAction; }
};

//------------------------------------------------------------------
// RemoveObjectiveAction
//------------------------------------------------------------------

class RemoveObjectiveAction : public TExpression
{
public:
	TEXPRESSION_CLONE(RemoveObjectiveAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new RemoveObjectiveAction; }
};

//------------------------------------------------------------------
// SetObjectiveStateAction
//------------------------------------------------------------------

class SetObjectiveStateAction : public TExpression
{
public:
	TEXPRESSION_CLONE(SetObjectiveStateAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SetObjectiveStateAction; }
};

//------------------------------------------------------------------
// CreateBlipAction
//------------------------------------------------------------------

class CreateBlipAction : public TExpression
{
public:
	TEXPRESSION_CLONE(CreateBlipAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new CreateBlipAction; }
};

//------------------------------------------------------------------
// RemoveBlipAction
//------------------------------------------------------------------

class RemoveBlipAction : public TExpression
{
public:
	TEXPRESSION_CLONE(RemoveBlipAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new RemoveBlipAction; }
};

//----------------------------------------------------------------------------------------------
// GroupLookAtGroupAction Class
//----------------------------------------------------------------------------------------------

class GroupLookAtGroupAction : public TExpression
{
public:
	enum
	{
		ARG_SRC_GROUPID,
		ARG_DST_GROUPID
	};
	//------------------------------------------
	TEXPRESSION_CLONE(GroupLookAtGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GroupLookAtGroupAction; }
};

//----------------------------------------------------------------------------------------------
// GroupStopLookingAtGroupAction Class
//----------------------------------------------------------------------------------------------

class GroupStopLookingAtGroupAction : public TExpression
{
public:
	enum
	{
		ARG_SRC_GROUPID
	};
	//------------------------------------------
	TEXPRESSION_CLONE(GroupStopLookingAtGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GroupStopLookingAtGroupAction; }
};

//----------------------------------------------------------------------------------------------
// GroupFaceGroupAction Class
//----------------------------------------------------------------------------------------------

class GroupFaceGroupAction : public TExpression
{
public:
	TEXPRESSION_CLONE(GroupFaceGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GroupFaceGroupAction; }

protected:
	void DoGroupFaceGroup(EntityGroup &group1, EntityGroup &group2, float degPerSec);
};

//----------------------------------------------------------------------------------------------
// GroupFaceGroupSpeedControlAction Class
//----------------------------------------------------------------------------------------------

class GroupFaceGroupSpeedControlAction : public GroupFaceGroupAction
{
public:
	TEXPRESSION_CLONE(GroupFaceGroupSpeedControlAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	//------------------------------------------
	static TExpression *Create() { return new GroupFaceGroupSpeedControlAction; }
};

//----------------------------------------------------------------------------------------------
// GroupFaceDirectionAction Class
//----------------------------------------------------------------------------------------------

class GroupFaceDirectionAction : public TExpression
{
public:
	TEXPRESSION_CLONE(GroupFaceDirectionAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GroupFaceDirectionAction; }

protected:
	void DoGroupFaceDirection(EntityGroup &group, float angle, float degPerSec);
};

//----------------------------------------------------------------------------------------------
// GroupFaceDirectionSpeedControlAction Class
//----------------------------------------------------------------------------------------------

class GroupFaceDirectionSpeedControlAction : public GroupFaceDirectionAction
{
public:
	TEXPRESSION_CLONE(GroupFaceDirectionSpeedControlAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	//------------------------------------------
	static TExpression *Create() { return new GroupFaceDirectionSpeedControlAction; }
};

//----------------------------------------------------------------------------------------------
// UpdateProximityGroupAction Class
//----------------------------------------------------------------------------------------------

class UpdateProximityGroupAction : public TExpression
{
public:
	enum
	{
		ARG_GROUPID,
		ARG_PLAYER,
		ARG_INT,
		ARG_ENTITY
	};
	//------------------------------------------
	TEXPRESSION_CLONE(UpdateProximityGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new UpdateProximityGroupAction; }

private:
	void updateGroupForPlayer(
			EntityGroup &eGroup,
			const Player *player,
			const Entity *location,
			int distance,
			SimWorld *pSimWorld);
};

//------------------------------------------------------------------
// DeselectAllAction
//------------------------------------------------------------------

class DeselectAllAction : public TExpression
{
public:
	TEXPRESSION_CLONE(DeselectAllAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new DeselectAllAction; }
};

//------------------------------------------------------------------
// SetGroupDeathFadeDelayAction
//------------------------------------------------------------------

class SetGroupDeathFadeDelayAction : public TExpression
{
public:
	TEXPRESSION_CLONE(SetGroupDeathFadeDelayAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SetGroupDeathFadeDelayAction; }
};

//------------------------------------------------------------------
// MoveEntity
//------------------------------------------------------------------

class MoveEntityAction : public TExpression
{
public:
	TEXPRESSION_CLONE(MoveEntityAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new MoveEntityAction; }
};

//------------------------------------------------------------------
// MoveGroupAction
//------------------------------------------------------------------

class MoveGroupAction : public TExpression
{
public:
	TEXPRESSION_CLONE(MoveGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new MoveGroupAction; }
};

//------------------------------------------------------------------
// SetGroupSpeedAction
//------------------------------------------------------------------

class SetGroupSpeedAction : public TExpression
{
public:
	TEXPRESSION_CLONE(SetGroupSpeedAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new SetGroupSpeedAction; }
};

//------------------------------------------------------------------
// ResetGroupSpeedAction
//------------------------------------------------------------------

class ResetGroupSpeedAction : public TExpression
{
public:
	TEXPRESSION_CLONE(ResetGroupSpeedAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new ResetGroupSpeedAction; }
};

//------------------------------------------------------------------
// GroupVisibleInFOWAction
//------------------------------------------------------------------

class GroupVisibleInFOWAction : public TExpression
{
public:
	TEXPRESSION_CLONE(GroupVisibleInFOWAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new GroupVisibleInFOWAction; }
};

//------------------------------------------------------------------
// DespawnEntity
//------------------------------------------------------------------

class DespawnEntityAction : public TExpression
{
public:
	TEXPRESSION_CLONE(DespawnEntityAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new DespawnEntityAction; }
};

//------------------------------------------------------------------
// RespawnEntity
//------------------------------------------------------------------

class RespawnEntityAction : public TExpression
{
public:
	TEXPRESSION_CLONE(RespawnEntityAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new RespawnEntityAction; }
};

//------------------------------------------------------------------
// DespawnGroup
//------------------------------------------------------------------

class DespawnGroupAction : public TExpression
{
public:
	TEXPRESSION_CLONE(DespawnGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new DespawnGroupAction; }
};

//------------------------------------------------------------------
// RespawnGroup
//------------------------------------------------------------------

class RespawnGroupAction : public TExpression
{
public:
	TEXPRESSION_CLONE(RespawnGroupAction);
	//------------------------------------------
	virtual bool Evaluate(TExpression::EvaluateParms &ep);
	virtual bool Deterministic() { return false; }
	//------------------------------------------
	static TExpression *Create() { return new RespawnGroupAction; }
};

//==============================================================================================
// Implementation
//==============================================================================================

//----------------------------------------------------------------------------------------------
// OrderGroupToLocationAction
//----------------------------------------------------------------------------------------------

bool OrderGroupToLocationAction::Evaluate(TExpression::EvaluateParms &ep)
{
	UNREF_P(ep);

	long groupID = GetArg(0).GetGroupID();
	long entId = GetArg(1).GetEntity();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID);
	Entity *pEntity = ModObj::i()->GetEntityFactory()->GetEntityFromEID(entId);

	// error check all the arguments
	if (pEntity && egroup && egroup->m_egroup.size() > 0)
	{

		// build list ids
		std::vector<unsigned long> idSelArray;
		EntityGroup::iterator iter = egroup->m_egroup.begin();
		EntityGroup::iterator enditer = egroup->m_egroup.end();
		for (; iter != enditer; iter++)
		{
			Entity *ent = (*iter);
			idSelArray.push_back(ent->GetID());
		}

		// send command
		ModObj::i()->GetWorld()->DoCommandEntityPoint(
				CMD_Move,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&pEntity->GetPosition(),
				1);
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// OrderGroupToStopAction
//----------------------------------------------------------------------------------------------

bool OrderGroupToStopAction::Evaluate(TExpression::EvaluateParms &ep)
{
	UNREF_P(ep);

	long groupID = GetArg(ARG_GROUP).GetGroupID();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID);

	// error check all the arguments
	if (egroup && egroup->m_egroup.size() > 0)
	{
		// build list ids
		std::vector<unsigned long> idSelArray;
		EntityGroup::iterator iter = egroup->m_egroup.begin();
		EntityGroup::iterator enditer = egroup->m_egroup.end();
		for (; iter != enditer; iter++)
		{
			Entity *ent = (*iter);
			idSelArray.push_back(ent->GetID());
		}

		// send command
		ModObj::i()->GetWorld()->DoCommandEntity(
				CMD_Stop,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size());
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// OrderGroupToMoveRandomlyAction
//----------------------------------------------------------------------------------------------

bool OrderGroupToMoveRandomlyAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long percent = GetArg(ARG_PERCENT).GetInt();
	long groupID = GetArg(ARG_GROUP).GetGroupID();
	long maxDist = GetArg(ARG_DIST).GetInt();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID);

	// error check all the arguments
	float fraction = (float)percent / 100.0f;
	if (egroup && egroup->m_egroup.size() > 0)
	{
		Random random;
		random.SetSeed((unsigned long)(ep.world->GetGameTime())); // randomly select group elements

		EntityGroup::iterator iter = egroup->m_egroup.begin();
		EntityGroup::iterator enditer = egroup->m_egroup.end();
		for (; iter != enditer; iter++)
		{
			if (random.GetUnitInclusive() < fraction)
			{
				Entity *ent = (*iter);
				unsigned long eid = ent->GetID();

				// pick random distance
				float dist = (float)(random.GetUnitInclusive()) * maxDist;

				// pick random direction
				float theta = (float)(random.GetUnitInclusive()) * TWOPI;
				Vec3f moveVec(dist * cosf(theta), 0, dist * sinf(theta));
				Vec3f newPosition = ent->GetPosition() + moveVec;

				// send command
				ModObj::i()->GetWorld()->DoCommandEntityPoint(
						CMD_Move,
						0,
						false,
						Player::InvalidID,
						&eid,
						1,
						&newPosition,
						1);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// Group2GroupAction
//----------------------------------------------------------------------------------------------

bool Group2GroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	UNREF_P(ep);

	long groupID0 = GetArg(0).GetGroupID();
	long groupID1 = GetArg(1).GetGroupID();

	ESimGroup *egroup0 = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID0);
	ESimGroup *egroup1 = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID1);

	// error check all the arguments
	if (egroup0 && egroup1 && egroup0->m_egroup.size() > 0 && egroup1->m_egroup.size() > 0)
	{

		// build list ids
		std::vector<unsigned long> idSelArray;
		EntityGroup::iterator iter = egroup0->m_egroup.begin();
		EntityGroup::iterator enditer = egroup0->m_egroup.end();
		for (; iter != enditer; iter++)
		{
			Entity *ent = (*iter);
			idSelArray.push_back(ent->GetID());
		}

		// build target list
		std::vector<unsigned long> targetArray;
		iter = egroup1->m_egroup.begin();
		enditer = egroup1->m_egroup.end();
		for (; iter != enditer; iter++)
		{
			Entity *ent = (*iter);
			targetArray.push_back(ent->GetID());
		}

		ModObj::i()->GetWorld()->DoCommandEntityEntity(
				CMD_AttackMove,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&targetArray[0],
				targetArray.size());
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// OrderGroupToDoCommandAction
//----------------------------------------------------------------------------------------------

bool OrderGroupToDoCommandAction::Evaluate(TExpression::EvaluateParms &ep)
{
	UNREF_P(ep);

	long groupID = GetArg(ARG_GROUP).GetGroupID();
	long entityCmd = GetArg(ARG_ENTITYCMD).GetEnum();
	long entId = GetArg(ARG_ENTITY).GetEntity();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID);
	Entity *pEntity = ModObj::i()->GetEntityFactory()->GetEntityFromEID(entId);

	if (!egroup)
	{
		dbWarningf('TRIG', "OrderGroupToDoCommandAction: Group not found");
		return false;
	}
	if (!pEntity)
	{
		dbWarningf('TRIG', "OrderGroupToDoCommandAction: Entity not found");
		return false;
	}

	// build list ids
	std::vector<unsigned long> idSelArray;
	EntityGroup::iterator iter = egroup->m_egroup.begin();
	EntityGroup::iterator enditer = egroup->m_egroup.end();
	for (; iter != enditer; iter++)
	{
		Entity *ent = (*iter);
		idSelArray.push_back(ent->GetID());
	}

	// quick exit
	if (idSelArray.size() <= 0)
	{
		return true;
	}

	const unsigned long id = pEntity->GetID();

	// owner
	unsigned long playerId = Player::InvalidID;
	Player *player = egroup->m_egroup.front()->GetOwner();
	if (player)
	{
		playerId = player->GetID();
	}

	switch (entityCmd)
	{
	case ModTriggerTypes::CMD_Attack:
	{
		ModObj::i()->GetWorld()->DoCommandEntityEntity(
				CMD_Attack,
				0,
				false,
				playerId,
				&idSelArray[0],
				idSelArray.size(),
				&id,
				1);
		break;
	}

	case ModTriggerTypes::CMD_AttackMove:
	{
		ModObj::i()->GetWorld()->DoCommandEntityPoint(
				CMD_AttackMove,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&pEntity->GetPosition(),
				1);
		break;
	}

	case ModTriggerTypes::CMD_RallyPoint:
	{
		ModObj::i()->GetWorld()->DoCommandEntityPoint(
				CMD_RallyPoint,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&pEntity->GetPosition(),
				1);
		break;
	}

	case ModTriggerTypes::CMD_Move:
	{
		ModObj::i()->GetWorld()->DoCommandEntityPoint(
				CMD_Move,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&pEntity->GetPosition(),
				1);
		break;
	}

	case ModTriggerTypes::CMD_DefaultAction:
	{
		ModObj::i()->GetWorld()->DoCommandEntityEntity(
				CMD_DefaultAction,
				0,
				false,
				Player::InvalidID,
				&idSelArray[0],
				idSelArray.size(),
				&id,
				1);
		break;
	}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// GiveCashAction
//----------------------------------------------------------------------------------------------

bool GiveCashAction::Evaluate(TExpression::EvaluateParms &ep)
{
	// get arguments
	long pset = GetArg(0).GetPlayer();
	int CashAmount = GetArg(1).GetInt();

	int playerIndex;
	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(pset), playerIndex))
	{
		RDNPlayer *player = static_cast<RDNPlayer *>(ep.world->GetPlayerAt(playerIndex));
		player->IncResourceCash(float(CashAmount));
	}
	else if (pset == TArgument::PS_CURRENTPLAYER)
	{
		RDNPlayer *player = static_cast<RDNPlayer *>(ep.player);
		player->IncResourceCash(float(CashAmount));
	}
	else if (pset == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// kill all other players ?
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *checkPlayer = ep.world->GetPlayerAt(i);
			if (checkPlayer != ep.player)
			{
				RDNPlayer *player = static_cast<RDNPlayer *>(checkPlayer);
				player->IncResourceCash(float(CashAmount));
			}
		}
	}
	else
	{
		dbWarningf('TRIG', "GiveCashAction: player not defined... error!");
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// SetCashAction
//----------------------------------------------------------------------------------------------

bool SetCashAction::Evaluate(TExpression::EvaluateParms &ep)
{
	// get arguments
	long pset = GetArg(0).GetPlayer();
	int CashAmount = GetArg(1).GetInt();

	int playerIndex;
	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(pset), playerIndex))
	{
		RDNPlayer *player = static_cast<RDNPlayer *>(ep.world->GetPlayerAt(playerIndex));
		setPlayerCash(player, float(CashAmount));
	}
	else if (pset == TArgument::PS_CURRENTPLAYER)
	{
		RDNPlayer *player = static_cast<RDNPlayer *>(ep.player);
		setPlayerCash(player, float(CashAmount));
	}
	else if (pset == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// kill all other players ?
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *checkPlayer = ep.world->GetPlayerAt(i);
			if (checkPlayer != ep.player)
			{
				RDNPlayer *player = static_cast<RDNPlayer *>(checkPlayer);
				setPlayerCash(player, float(CashAmount));
			}
		}
	}
	else
	{
		dbWarningf('TRIG', "GiveCashAction: player not defined... error!");
		return false;
	}

	return true;
}

void SetCashAction::setPlayerCash(RDNPlayer *player, float CashAmount)
{
	float CashNow = player->GetResourceCash();
	float CashDiff = CashAmount - CashNow;
	if (CashDiff < 0)
	{
		player->DecResourceCash(-CashDiff);
	}
	else
	{
		player->IncResourceCash(+CashDiff);
	}
}

//----------------------------------------------------------------------------------------------
// LoseAction
//----------------------------------------------------------------------------------------------

bool LoseAction::Evaluate(TExpression::EvaluateParms &ep)
{
	int playerid = GetArg(0).GetPlayer();
	int playerIndex = 0;

	// find out what players should lose in relation to the player running this trigger

	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(playerid), playerIndex))
	{
		Player *player = ep.world->GetPlayerAt(playerIndex);
		player->KillPlayer(RDNPlayer::KPR_Trigger);
	}
	else if (playerid == TArgument::PS_CURRENTPLAYER)
	{
		Player *player = ep.player;
		player->KillPlayer(RDNPlayer::KPR_Trigger);
	}
	else if (playerid == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// kill all other players ?
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *checkPlayer = ep.world->GetPlayerAt(i);
			if (checkPlayer != ep.player)
			{
				checkPlayer->KillPlayer(RDNPlayer::KPR_Trigger);
			}
		}
	}
	else
	{
		dbWarningf('TRIG', "LoseAction: player not defined... error!");
	}

	// the game is not over if we have one pair of enemy players left
	bool gameOver = true;
	size_t numplayers = ep.world->GetPlayerCount();

	for (size_t i = 0; i < numplayers; ++i)
	{
		Player *playerA = ep.world->GetPlayerAt(i);
		if (playerA->IsPlayerDead())
			continue;

		for (size_t j = 0; j < numplayers; ++j)
		{
			Player *playerB = ep.world->GetPlayerAt(j);
			if (playerB->IsPlayerDead())
				continue;

			if (playerA != playerB)
			{
				gameOver = false;
				break;
			}
		}

		if (!gameOver)
			break;
	}
	if (gameOver)
	{
		ep.world->SetGameOver();
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// WinAction
//----------------------------------------------------------------------------------------------

bool WinAction::Evaluate(TExpression::EvaluateParms &ep)
{
	int playerset = GetArg(0).GetPlayer();
	int playerIndex = 0;

	Player *winPlayer;

	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(playerset), playerIndex))
	{
		winPlayer = ep.world->GetPlayerAt(playerIndex);
	}
	else if (playerset == TArgument::PS_CURRENTPLAYER)
	{
		winPlayer = ep.player;
	}
	else
	{
		dbWarningf('TRIG', "WinAction: the CURRENTPLAYER arg is the only valid one for now!");
		return false;
	}

	size_t numplayers = ep.world->GetPlayerCount();
	// kill all other players ? you win!
	for (size_t i = 0; i < numplayers; ++i)
	{
		Player *checkPlayer = ep.world->GetPlayerAt(i);
		if (winPlayer != checkPlayer)
		{
			//if not dead than kill them
			if (checkPlayer->IsPlayerDead() == false)
			{
				checkPlayer->KillPlayer(RDNPlayer::KPR_Trigger);
			}
		}
	}

	// since all enemies are dead and you have won the game is over
	ep.world->SetGameOver();

	return true;
}

//----------------------------------------------------------------------------------------------
// SpawnEntityAction
//----------------------------------------------------------------------------------------------

bool SpawnEntityAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long ebpNetID = GetArg(0).GetEBPNetID();
	unsigned long spawnerId = (unsigned long)(GetArg(1).GetEntity());

	const ControllerBlueprint *pCBP = ModObj::i()->GetEntityFactory()->GetControllerBP(ebpNetID);
	if (!pCBP)
	{
		dbWarningf('TRIG', "SpawnEntityAction: Controller blueprint not found for EBP network ID (%d)", ebpNetID);
		return false;
	}

	const ECStaticInfo *si =
			ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(pCBP);
	const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);
	if (cost == 0)
	{
		dbWarningf('TRIG', "SpawnEntityAction: Unable to determine entity cost");
		return false;
	}

	RDNPlayer *player = static_cast<RDNPlayer *>(ep.player);

	// give Cash
	player->IncResourceCash(cost->costCash);

	// do spawning
	ep.world->DoCommandEntity(
			CMD_BuildUnit,
			ebpNetID,
			false,
			ep.player->GetID(),
			&spawnerId,
			1);
	return true;
}

Player *getPlayerFromArgs(TArgument::PlayerSet playerSet, TExpression::EvaluateParms &ep)
{
	// the player set must select a specific player
	Player *player = 0;
	int playerIndex = 0;

	if (playerSet == TArgument::PS_CURRENTPLAYER)
	{
		player = ep.player;
	}
	else if (TArgument::GetPlayerIndex(playerSet, playerIndex))
	{
		player = ep.world->GetPlayerAt((int)playerIndex);
	}
	else
	{
		dbWarningf('TRIG', "getPlayerFromArgs: Player must either be the current player or one of the players");
	}

	return player;
}

//------------------------------------------------------------------
// GetEBPTypeGroupAction
//------------------------------------------------------------------

bool GetEBPTypeGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();
	TArgument::PlayerSet playerSet = (TArgument::PlayerSet)GetArg(1).GetPlayer();
	long ebpNetID = GetArg(2).GetEBPNetID();

	ESimGroup *group = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!group)
	{
		dbWarningf('TRIG', "GetEBPTypeGroupAction: Group not found");
		return false;
	}

	// clear group
	group->m_egroup.clear();

	// get player
	Player *player = 0;
	int playerIndex = 0;
	if (playerSet == TArgument::PS_CURRENTPLAYER)
	{
		player = ep.player;
		addEBPToGroup(player, group->m_egroup, ebpNetID);
	}
	else if (TArgument::GetPlayerIndex(playerSet, playerIndex))
	{
		player = ep.world->GetPlayerAt((int)playerIndex);
		addEBPToGroup(player, group->m_egroup, ebpNetID);
	}
	else if (playerSet == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// apply operation to all enemies
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *checkPlayer = ep.world->GetPlayerAt(i);
			if (checkPlayer != ep.player)
			{
				RDNPlayer *player = static_cast<RDNPlayer *>(checkPlayer);
				addEBPToGroup(player, group->m_egroup, ebpNetID);
			}
		}
	}
	else if (playerSet == TArgument::PS_ALLPLAYERS)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// apply operation to all enemies
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *player = ep.world->GetPlayerAt(i);
			addEBPToGroup(player, group->m_egroup, ebpNetID);
		}
	}
	else
	{
		dbWarningf('TRIG', "GetEBPTypeGroupAction: Unsupported player selection");
		return false;
	}

	return true;
}

void GetEBPTypeGroupAction::addEBPToGroup(Player *player, EntityGroup &group, long ebpNetID)
{
	const EntityGroup &playerEntities = player->GetEntities();
	EntityGroup::const_iterator ei = playerEntities.begin();
	EntityGroup::const_iterator ee = playerEntities.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		if (pEntity->GetControllerBP() && (pEntity->GetControllerBP()->GetEBPNetworkID() == ebpNetID))
		{
			// ebp net ID matched
			group.push_back(pEntity);
		}
	}
}

//------------------------------------------------------------------
// GetAllGroupAction
//------------------------------------------------------------------

bool GetAllGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();
	TArgument::PlayerSet playerSet = (TArgument::PlayerSet)GetArg(1).GetPlayer();

	ESimGroup *group = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!group)
	{
		dbWarningf('TRIG', "GetAllGroupAction: Group not found");
		return false;
	}

	// clear group
	group->m_egroup.clear();

	// get player
	Player *player = 0;
	int playerIndex = 0;
	if (playerSet == TArgument::PS_CURRENTPLAYER)
	{
		player = ep.player;
		addPlayerEntitiesToGroup(player, group->m_egroup);
	}
	else if (TArgument::GetPlayerIndex(playerSet, playerIndex))
	{
		player = ep.world->GetPlayerAt((int)playerIndex);
		addPlayerEntitiesToGroup(player, group->m_egroup);
	}
	else if (playerSet == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// apply operation to all enemies
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *checkPlayer = ep.world->GetPlayerAt(i);
			if (checkPlayer != ep.player)
			{
				RDNPlayer *player = static_cast<RDNPlayer *>(checkPlayer);
				addPlayerEntitiesToGroup(player, group->m_egroup);
			}
		}
	}
	else if (playerSet == TArgument::PS_ALLPLAYERS)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();

		// apply operation to all enemies
		for (size_t i = 0; i < numplayers; ++i)
		{
			Player *player = ep.world->GetPlayerAt(i);
			addPlayerEntitiesToGroup(player, group->m_egroup);
		}
	}
	else
	{
		dbWarningf('TRIG', "GetAllGroupAction: Unsupported player selection");
		return false;
	}

	return true;
}

void GetAllGroupAction::addPlayerEntitiesToGroup(Player *player, EntityGroup &group)
{
	const EntityGroup &playerEntities = player->GetEntities();
	EntityGroup::const_iterator ei = playerEntities.begin();
	EntityGroup::const_iterator ee = playerEntities.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;
		group.push_back(pEntity);
	}
}

//----------------------------------------------------------------------------------------------
// SetGroupOwnerAction
//----------------------------------------------------------------------------------------------

bool SetGroupOwnerAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();

	// retrieve group
	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!egroup)
	{
		dbWarningf('TRIG', "SetGroupOwnerAction: unknown group id(%d)", groupid);
		return false;
	}

	// determine which players to check by looking at and at the PlayerSet
	TArgument::PlayerSet playerSet = (TArgument::PlayerSet)GetArg(1).GetPlayer();

	// should use playerGroup concept here
	Player *newOwner = NULL;
	int playerIndex;

	if (playerSet == TArgument::PS_CURRENTPLAYER)
	{
		newOwner = ep.player;
	}
	else if (TArgument::GetPlayerIndex(playerSet, playerIndex))
	{
		newOwner = ep.world->GetPlayerAt((int)playerIndex);
	}
	else if (playerSet == TArgument::PS_MOTHERNATURE)
	{
		newOwner = NULL;
	}
	else
	{
		dbWarningf('TRIG', "SetGroupOwnerAction: unknown new owner - action aborted");
		return false;
	}

	EntityGroup::iterator ei = egroup->m_egroup.begin();
	EntityGroup::iterator ee = egroup->m_egroup.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		// remove from original owner
		const Player *kOrigOwner = pEntity->GetOwner();

		if (kOrigOwner != newOwner)
		{
			if (kOrigOwner)
			{
				Player *origOwner = ep.world->GetPlayerFromID(kOrigOwner->GetID());
				origOwner->RemoveEntity(pEntity);
				// this sets the owner to NULL (Mother Nature)
			}

			// add entity to new owner
			if (newOwner)
			{
				newOwner->AddEntity(pEntity);
			}

			// remove entity from all selection and hotkey groups
			Unselect(pEntity);

			// stop friendly fire
			StopFriendlyFire(pEntity->GetOwner());
		}
	}

	return true;
}

void SetGroupOwnerAction::Unselect(const Entity *pEntity)
{
	// validate parm
	dbAssert(pEntity);

	// find the new selection group
	EntityGroup selGroup = ModObj::i()->GetSelectionInterface()->GetSelection();
	selGroup.remove(pEntity);

	// update hotkey groups
	for (int groupNum = 0; groupNum != SelectionInterface::NUM_HOTKEYGROUPS; ++groupNum)
	{
		const EntityGroup &hkGroup = ModObj::i()->GetSelectionInterface()->GetHotkeyGroup(groupNum);
		if (hkGroup.find(pEntity) != hkGroup.end())
		{
			EntityGroup tmpSelGroup = hkGroup;
			tmpSelGroup.remove(pEntity);
			ModObj::i()->GetSelectionInterface()->SetSelection(tmpSelGroup);
			ModObj::i()->GetSelectionInterface()->AssignHotkeyGroupFromSelection(groupNum, RDNEntityFilter::Instance());
		}
	}

	// set the selection group back
	ModObj::i()->GetSelectionInterface()->SetSelection(selGroup);
}

void SetGroupOwnerAction::StopFriendlyFire(Player *pPlayer)
{
	// do nothing if entity is owned by mother nature
	if (pPlayer == 0)
		return;

	unsigned long ownerID = pPlayer->GetID();

	// go through all entities of this owner and tell them to stop attacking this entity
	const EntityGroup &eGroup = pPlayer->GetEntities();
	EntityGroup::const_iterator ei = eGroup.begin();
	EntityGroup::const_iterator ee = eGroup.end();

	for (; ei != ee; ei++)
	{
		const Entity *pAttacker = *ei;
		const StateAttack *pAttackState = QISubState<StateAttack>(pAttacker);
		if (pAttackState && pAttackState->GetTargetEntity() &&
				pAttackState->GetTargetEntity()->GetOwner() &&
				pAttackState->GetTargetEntity()->GetOwner()->GetID() == ownerID)
		{
			unsigned long attackerID = pAttacker->GetID();

			// tell the attacker to stop
			ModObj::i()->GetWorld()->DoCommandEntity(
					CMD_Stop,
					0,
					false,
					ownerID,
					&attackerID,
					1);
		}
	}
}

//----------------------------------------------------------------------------------------------
// CreatePrimObjectiveAction
//----------------------------------------------------------------------------------------------

bool CreatePrimObjectiveAction::Evaluate(TExpression::EvaluateParms &)
{
	long objectiveID = GetArg(0).GetInt();
	long shortDescID = GetArg(1).GetInt();
	long tipID = GetArg(2).GetInt();

	Objective *pObj = ModObj::i()->GetObjectiveFactory()->CreateObjective(objectiveID);
	if (!pObj)
	{
		dbWarningf('TRIG', "CreatePrimObjectiveAction: Unable to create objective");
		return false;
	}

	pObj->SetType(Objective::OT_Primary);
	pObj->SetShortDescID(shortDescID);
	pObj->SetTipID(tipID);

	return true;
}

//----------------------------------------------------------------------------------------------
// CreateSecObjectiveAction
//----------------------------------------------------------------------------------------------

bool CreateSecObjectiveAction::Evaluate(TExpression::EvaluateParms &)
{
	long objectiveID = GetArg(0).GetInt();
	long shortDescID = GetArg(1).GetInt();
	long tipID = GetArg(2).GetInt();

	Objective *pObj = ModObj::i()->GetObjectiveFactory()->CreateObjective(objectiveID);
	if (!pObj)
	{
		dbWarningf('TRIG', "CreateSecObjectiveAction: Unable to create objective");
		return false;
	}

	pObj->SetType(Objective::OT_Secondary);
	pObj->SetShortDescID(shortDescID);
	pObj->SetTipID(tipID);

	return true;
}

//----------------------------------------------------------------------------------------------
// RemoveObjectiveAction
//----------------------------------------------------------------------------------------------

bool RemoveObjectiveAction::Evaluate(TExpression::EvaluateParms &)
{
	long objectiveID = GetArg(0).GetInt();

	Objective *pObj = ModObj::i()->GetObjectiveFactory()->CreateObjective(objectiveID);
	if (pObj)
	{
		ModObj::i()->GetObjectiveFactory()->DeleteObjective(pObj);
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// SetObjectiveStateAction
//----------------------------------------------------------------------------------------------

bool SetObjectiveStateAction::Evaluate(TExpression::EvaluateParms &)
{
	long objID = GetArg(0).GetInt();
	ModTriggerTypes::ObjectiveState objState =
			static_cast<ModTriggerTypes::ObjectiveState>(GetArg(1).GetEnum());

	Objective *pObj = ModObj::i()->GetObjectiveFactory()->GetObjective(objID);
	if (pObj)
	{
		pObj->SetState(static_cast<Objective::State>(objState));
	}
	else
	{
		dbWarningf('TRIG', "SetObjectiveStateAction: Objective not found (%d)", objID);
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// BindObjectiveToLocAction
//----------------------------------------------------------------------------------------------

bool BindObjectiveToLocAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long objID = GetArg(0).GetInt();
	long entityID = GetArg(1).GetEntity();

	Objective *pObj = ModObj::i()->GetObjectiveFactory()->GetObjective(objID);
	if (!pObj)
	{
		dbWarningf('TRIG', "BindObjectiveToLocAction: Objective not found (%d)", objID);
		return false;
	}

	Entity *pEntity = ep.world->GetEntityFactory()->GetEntityFromEID(entityID);

	if (pEntity)
	{
		pObj->SetEntity(pEntity);
		return true;
	}

	dbWarningf('TRIG', "BindObjectiveToLocAction: Unable to find entity");
	return false;
}

//----------------------------------------------------------------------------------------------
// CreateBlipAction
//----------------------------------------------------------------------------------------------

bool CreateBlipAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long blipID = GetArg(0).GetInt();
	long entityID = GetArg(1).GetEntity();

	Entity *pEntity = ep.world->GetEntityFactory()->GetEntityFromEID(entityID);

	if (pEntity)
	{
		Blip *pBlip = ModObj::i()->GetBlipFactory()->CreateBlip(blipID);
		if (!pBlip)
		{
			dbWarningf('TRIG', "CreateBlipAction: Unable to create blip");
			return false;
		}

		pBlip->SetEntity(pEntity);
		return true;
	}

	dbWarningf('TRIG', "CreateBlipAction: Unable to find entity");
	return false;
}

//----------------------------------------------------------------------------------------------
// RemoveBlipAction
//----------------------------------------------------------------------------------------------

bool RemoveBlipAction::Evaluate(TExpression::EvaluateParms &)
{
	const long blipID = GetArg(0).GetInt();

	ModObj::i()->GetBlipFactory()->DeleteBlip(blipID);

	return true;
}

//----------------------------------------------------------------------------------------------
// GroupLookAtGroup
//----------------------------------------------------------------------------------------------

bool GroupLookAtGroupAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(ARG_SRC_GROUPID).GetGroupID();
	long dstGroupid = GetArg(ARG_DST_GROUPID).GetGroupID();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	ESimGroup *eDstGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(dstGroupid);
	if (!eSrcGroup || !eDstGroup)
	{
		return false;
	}

	if (eDstGroup->m_egroup.begin() == eDstGroup->m_egroup.end())
		return true;

	// set look at center to the first entity in the group (groups for this purpose typically only have one entity anyway)
	Vec3f center(eDstGroup->m_egroup.front()->GetPosition());
	EntityGroup::iterator iter;

	// make everybody face that way
	for (iter = eSrcGroup->m_egroup.begin(); iter != eSrcGroup->m_egroup.end(); iter++)
	{
		Entity *entity = *iter;

		ModController *modCont = static_cast<ModController *>(entity->GetController());
		if (modCont)
		{
			Vec2f dir;
			dir.x = center.x - entity->GetPosition().x;
			dir.y = center.z - entity->GetPosition().z;

			float len = dir.Length();
			if (len == 0)
				continue;

			dir /= len;

			Vec2f facing(entity->GetTransform().R.Z_axis.x, entity->GetTransform().R.Z_axis.z);
			facing.NormalizeSelf();

			// default rotation speed
			const float DegPerSec = 180.0f;
			float degPerTick = DegPerSec / ModObj::i()->GetWorld()->GetNumSimsPerSecond();

			// turn to face entity only if facing more than 60 degrees away
			if (facing % dir < 0.5f)
				modCont->GetEntityDynamics()->RequestEntityFacing(dir, degPerTick);

			if (entity->GetAnimator())
				entity->GetAnimator()->SetTargetLook(eDstGroup->m_egroup.front());
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// GroupStopLookingAtGroupAction
//----------------------------------------------------------------------------------------------

bool GroupStopLookingAtGroupAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(ARG_SRC_GROUPID).GetGroupID();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	if (!eSrcGroup)
	{
		return false;
	}

	EntityGroup::iterator iter;

	// stop everybody from following the target
	for (iter = eSrcGroup->m_egroup.begin(); iter != eSrcGroup->m_egroup.end(); iter++)
	{
		Entity *entity = *iter;

		ModController *modCont = static_cast<ModController *>(entity->GetController());
		if (modCont)
		{
			if (entity->GetAnimator())
				entity->GetAnimator()->SetTargetLook(NULL);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// GroupFaceGroup
//----------------------------------------------------------------------------------------------

bool GroupFaceGroupAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(0).GetGroupID();
	long dstGroupid = GetArg(1).GetGroupID();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	ESimGroup *eDstGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(dstGroupid);
	if (!eSrcGroup || !eDstGroup)
	{
		return false;
	}

	DoGroupFaceGroup(eSrcGroup->m_egroup, eDstGroup->m_egroup, 180.0f);

	return true;
}

void GroupFaceGroupAction::DoGroupFaceGroup(EntityGroup &srcGroup, EntityGroup &dstGroup, float degPerSec)
{
	// quick out
	if (srcGroup.empty() || dstGroup.empty())
		return;

	// convert degrees per second to degrees per tick
	float degPerTick = degPerSec / ModObj::i()->GetWorld()->GetNumSimsPerSecond();

	// find the center of the destination group
	Vec3f center(0, 0, 0);
	EntityGroup::iterator iter;
	for (iter = dstGroup.begin(); iter != dstGroup.end(); ++iter)
	{
		Entity *entity = *iter;
		center += entity->GetPosition();
	}
	center /= (float)(dstGroup.size());

	// make everybody face that way
	for (iter = srcGroup.begin(); iter != srcGroup.end(); iter++)
	{
		Entity *entity = *iter;

		ModController *modCont = static_cast<ModController *>(entity->GetController());
		if (modCont)
		{
			Vec2f dir;
			dir.x = center.x - entity->GetPosition().x;
			dir.y = center.z - entity->GetPosition().z;

			float len = dir.Length();
			if (len == 0)
				continue;

			dir /= len;
			modCont->GetEntityDynamics()->RequestEntityFacing(dir, degPerTick);
		}
	}
}

//----------------------------------------------------------------------------------------------
// GroupFaceGroupSpeedControlAction
//----------------------------------------------------------------------------------------------

bool GroupFaceGroupSpeedControlAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(0).GetGroupID();
	long dstGroupid = GetArg(1).GetGroupID();
	float degPerSec = GetArg(2).GetFloat();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	ESimGroup *eDstGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(dstGroupid);
	if (!eSrcGroup || !eDstGroup)
	{
		return false;
	}

	DoGroupFaceGroup(eSrcGroup->m_egroup, eDstGroup->m_egroup, degPerSec);

	return true;
}

//----------------------------------------------------------------------------------------------
// GroupFaceDirection
//----------------------------------------------------------------------------------------------

bool GroupFaceDirectionAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(0).GetGroupID();
	float angle = GetArg(1).GetFloat();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	if (!eSrcGroup)
	{
		return false;
	}

	DoGroupFaceDirection(eSrcGroup->m_egroup, angle, 180.0f);

	return true;
}

void GroupFaceDirectionAction::DoGroupFaceDirection(EntityGroup &group, float angle, float degPerSec)
{
	// convert degrees per second to degrees per tick
	float degPerTick = degPerSec / ModObj::i()->GetWorld()->GetNumSimsPerSecond();

	// make everybody face that way
	angle *= (PI / 180.0f);
	Vec2f dir(cosf(angle), sinf(angle));
	EntityGroup::iterator iter;
	for (iter = group.begin(); iter != group.end(); iter++)
	{
		Entity *entity = *iter;

		ModController *modCont = static_cast<ModController *>(entity->GetController());
		if (modCont)
		{
			modCont->GetEntityDynamics()->RequestEntityFacing(dir, degPerTick);
		}
	}
}

//----------------------------------------------------------------------------------------------
// GroupFaceDirectionSpeedControl
//----------------------------------------------------------------------------------------------

bool GroupFaceDirectionSpeedControlAction::Evaluate(TExpression::EvaluateParms &)
{
	long srcGroupid = GetArg(0).GetGroupID();
	float angle = GetArg(1).GetFloat();
	float degPerSec = GetArg(2).GetFloat();

	ESimGroup *eSrcGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(srcGroupid);
	if (!eSrcGroup)
	{
		return false;
	}

	DoGroupFaceDirection(eSrcGroup->m_egroup, angle, degPerSec);

	return true;
}

//----------------------------------------------------------------------------------------------
// UpdateProximityGroup
//----------------------------------------------------------------------------------------------

bool UpdateProximityGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(ARG_GROUPID).GetGroupID();
	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!egroup)
	{
		return false;
	}

	// clear group
	egroup->m_egroup.clear();

	long locationID = GetArg(ARG_ENTITY).GetEntity();
	Entity *location = ep.world->GetEntityFactory()->GetEntityFromEID(locationID);
	if (!location)
	{
		return false;
	}

	SimWorld *pSimWorld = static_cast<SimWorld *>(ep.world);

	int distance = GetArg(ARG_INT).GetInt();

	// determine which players to check by looking at and at the PlayerSet
	TArgument::PlayerSet playerSet = (TArgument::PlayerSet)GetArg(ARG_PLAYER).GetPlayer();

	// should use playerGroup concept here
	Player *player = NULL;
	int playerIndex = 0;

	if (playerSet == TArgument::PS_CURRENTPLAYER)
	{
		player = ep.player;
		updateGroupForPlayer(egroup->m_egroup, player, location, distance, pSimWorld);
		return true;
	}
	else if (TArgument::GetPlayerIndex(playerSet, playerIndex))
	{
		player = ep.world->GetPlayerAt((int)playerIndex);
		updateGroupForPlayer(egroup->m_egroup, player, location, distance, pSimWorld);
		return true;
	}
	else if (playerSet == TArgument::PS_ALLPLAYERS)
	{
		// find first enemy in list of players
		for (unsigned int i = 0; i < ep.world->GetPlayerCount(); i++)
		{
			Player *player = ep.world->GetPlayerAt(i);

			updateGroupForPlayer(egroup->m_egroup, player, location, distance, pSimWorld);
		}
		return true;
	}
	else if (playerSet == TArgument::PS_MOTHERNATURE)
	{
		updateGroupForPlayer(egroup->m_egroup, NULL, location, distance, pSimWorld);
	}
	else
	{
		dbWarningf('TRIG', "UpdateProximityGroupAction: Invalid player argument");
	}

	return false;
}

void UpdateProximityGroupAction::updateGroupForPlayer(
		EntityGroup &eGroup,
		const Player *player,
		const Entity *location,
		int distance,
		SimWorld *pSimWorld)
{
	FindClosestUnitWithOwnerFilter filter(player);

	pSimWorld->FindClosest(eGroup, filter, 0, location->GetPosition(), float(distance), location);
}

//----------------------------------------------------------------------------------------------
// DeselectAllAction
//----------------------------------------------------------------------------------------------

bool DeselectAllAction::Evaluate(TExpression::EvaluateParms &)
{
	// unselect entities
	EntityGroup emptyGroup;
	ModObj::i()->GetSelectionInterface()->SetSelection(emptyGroup);

	return true;
}

//----------------------------------------------------------------------------------------------
// SetGroupDeathFadeDelayAction
//----------------------------------------------------------------------------------------------

bool SetGroupDeathFadeDelayAction::Evaluate(TExpression::EvaluateParms &)
{
	long groupid = GetArg(0).GetGroupID();
	int delayCount = GetArg(1).GetInt();

	ESimGroup *pEGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!pEGroup)
	{
		dbWarningf('TRIG', "SetGroupDeathFadeDelayAction: Group not found");
		return false;
	}

	// compute group endurance = sum(entity endurance) / sum(max entity endurance)
	EntityGroup::iterator ei = pEGroup->m_egroup.begin();
	EntityGroup::iterator ee = pEGroup->m_egroup.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		StateDead *stateDead = QIState<StateDead>(pEntity);
		if (stateDead)
		{
			stateDead->SetFadeDelay(delayCount);
		}
		else
		{
			dbWarningf('TRIG', "SetGroupDeathFadeDelayAction: should only be applied to dying entities");
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// MoveEntityAction
//----------------------------------------------------------------------------------------------

bool MoveEntityAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long entityID = GetArg(0).GetEntity();
	long locationID = GetArg(1).GetEntity();

	Entity *pEntity = ep.world->GetEntityFactory()->GetEntityFromEID(entityID);
	Entity *pLocation = ep.world->GetEntityFactory()->GetEntityFromEID(locationID);

	if (pEntity && pLocation)
	{
		// only despawn entity if its in the world
		ep.world->DeSpawnEntity(pEntity);

		Matrix43f m = pEntity->GetTransform();
		m.T = pLocation->GetPosition();

		pEntity->SetTransform(m);
		pEntity->SetPrevTransform(m);
		pEntity->SetPrevPrevTransform(m);

		ep.world->SpawnEntity(pEntity);

		// make entity visible
		pEntity->SetEntityFlag(EF_IsVisible);
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// MoveGroupAction
//----------------------------------------------------------------------------------------------

bool MoveGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupID = GetArg(0).GetGroupID();
	long locationID = GetArg(1).GetEntity();

	ESimGroup *eGroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupID);
	Entity *pLocation = ep.world->GetEntityFactory()->GetEntityFromEID(locationID);

	if (eGroup && pLocation && (eGroup->m_egroup.size() > 0))
	{
		// move entities so that they are close to the target location
		EntityGroup::iterator ei = eGroup->m_egroup.begin();
		EntityGroup::iterator ee = eGroup->m_egroup.end();

		for (; ei != ee; ei++)
		{
			Entity *pEntity = *ei;

			// despawn
			ep.world->DeSpawnEntity(pEntity);

			// determine the best position to spawn entity around the target position
			Vec3f entPosition = pLocation->GetPosition();
			const MovingExtInfo *info = QIExtInfo<MovingExtInfo>(pEntity->GetController());

			if (info)
			{
				TCMask movementMask = info->GetMovementMask();

				Vec2f wantedPosition(entPosition.x, entPosition.z);
				Vec3f bestPosition;
				bool bGotPosition = ModObj::i()->GetWorld()->GetPathfinder()->Query()->GiveClosestFreePosition(
						wantedPosition, movementMask, 0, pEntity->GetShrunkenOBB(), bestPosition);
				if (bGotPosition)
				{
					entPosition = bestPosition;
				}
			}

			Matrix43f m = pEntity->GetTransform();
			m.T = entPosition;

			pEntity->SetTransform(m);
			pEntity->SetPrevTransform(m);
			pEntity->SetPrevPrevTransform(m);

			// respawn
			ep.world->SpawnEntity(pEntity);

			// make entity visible
			pEntity->SetEntityFlag(EF_IsVisible);
		}
	}

	return true;

#if 0
		// compute the center of the group
		Vec3f  centre(0,0,0);

		EntityGroup::iterator ei = eGroup->m_egroup.begin();
		EntityGroup::iterator ee = eGroup->m_egroup.end();
		for (; ei != ee; ei++)
		{
			Entity* entity = *ei;
			centre += entity->GetPosition();
		}
		centre *= 1.0f / eGroup->m_egroup.size();

		// compute offset vector
		Vec3f offset = pLocation->GetPosition() - centre;

		// compute new locations for the group members
		ei = eGroup->m_egroup.begin();
		for (; ei != ee; ei++)
		{
			Entity* pEntity = *ei;

			Vec3f newLocation = pEntity->GetPosition() + offset;

			ep.world->DeSpawnEntity(pEntity);

			Matrix43f m = pEntity->GetTransform();
			m.T = newLocation;

			pEntity->SetTransform( m );
			pEntity->SetPrevTransform( m );
			pEntity->SetPrevPrevTransform( m );

			ep.world->SpawnEntity(pEntity);

			// make entity visible
			pEntity->SetEntityFlag( EF_IsVisible );
		}
	}

	return false;
#endif
}

//----------------------------------------------------------------------------------------------
// SetGroupSpeedAction
//----------------------------------------------------------------------------------------------

bool SetGroupSpeedAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();
	float speed = GetArg(1).GetFloat();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!egroup)
	{
		dbWarningf('TRIG', "SetGroupSpeedAction: Group not found");
		SetValid(false);
		return false;
	}

	// convert speed from km/h to meters per sim-step time
	const float factor = (1.0f / 3.6f) / ep.world->GetNumSimsPerSecond();
	float metersPerTick = speed * factor;

	EntityGroup::iterator ei = egroup->m_egroup.begin();
	EntityGroup::iterator ee = egroup->m_egroup.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		if (pEntity->GetController())
		{
			pEntity->GetController()->SetSpeedCeil(metersPerTick);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// ResetGroupSpeedAction
//----------------------------------------------------------------------------------------------

bool ResetGroupSpeedAction::Evaluate(TExpression::EvaluateParms &)
{
	long groupid = GetArg(0).GetGroupID();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!egroup)
	{
		dbWarningf('TRIG', "ResetGroupSpeedAction: Group not found");
		SetValid(false);
		return false;
	}

	EntityGroup::iterator ei = egroup->m_egroup.begin();
	EntityGroup::iterator ee = egroup->m_egroup.end();

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		if (pEntity->GetController())
		{
			pEntity->GetController()->UnsetSpeedCeil();
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// GroupVisibleInFOWAction
//----------------------------------------------------------------------------------------------

bool GroupVisibleInFOWAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();
	long pset = GetArg(1).GetPlayer();
	bool bVis = GetArg(2).GetTruth() ? true : false;

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);
	if (!egroup)
	{
		dbWarningf('TRIG', "GroupVisibleInFOWAction: Group not found");
		SetValid(false);
		return false;
	}

	RDNPlayer *player = NULL;
	int playerIndex;
	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(pset), playerIndex))
	{
		player = static_cast<RDNPlayer *>(ep.world->GetPlayerAt(playerIndex));
	}
	else if (pset == TArgument::PS_CURRENTPLAYER)
	{
		player = static_cast<RDNPlayer *>(ep.player);
	}
	else
	{
		dbWarningf('TRIG', "GrupVisible in FOW: needs either the current player or one of the 6 players as arg");
		SetValid(false);
		return false;
	}
	dbAssert(player);

	PlayerFOW *pPlayerFOW = player->GetFogOfWar();

	EntityGroup::iterator ei = egroup->m_egroup.begin();
	EntityGroup::iterator ee = egroup->m_egroup.end();

	float revealTime = bVis ? 1000000.0f : 0.1f;

	for (; ei != ee; ei++)
	{
		Entity *pEntity = *ei;

		pPlayerFOW->RevealEntity(pEntity, revealTime);
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// DespawnEntityAction
//----------------------------------------------------------------------------------------------

bool DespawnEntityAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long entityID = GetArg(0).GetEntity();

	Entity *pEntity = ep.world->GetEntityFactory()->GetEntityFromEID(entityID);

	if (pEntity)
	{
		// ignore entities that are despawned
		if (!pEntity->GetEntityFlag(EF_IsSpawned))
			return false;

		// ignore entities that are dying
		if (QIState<StateDead>(pEntity))
			return false;

		// make entity invisible
		pEntity->ClearEntityFlag(EF_IsVisible);

		if (pEntity->GetEntityFlag(EF_CanCollide) && (pEntity->GetController() == NULL))
		{
			dbTracef("TRIGGER -- Error can't despawn collideable entities that have no controller, EBP '%s'", pEntity->GetControllerBP()->GetFileName());
		}

		// remove it from the world
		ep.world->DeSpawnEntity(pEntity);
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// RespawnEntityAction
//----------------------------------------------------------------------------------------------

bool RespawnEntityAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long entityID = GetArg(0).GetEntity();

	Entity *pEntity = ep.world->GetEntityFactory()->GetEntityFromEID(entityID);

	if (pEntity)
	{
		// ignore entities that are spawned
		if (pEntity->GetEntityFlag(EF_IsSpawned))
			return false;

		// reinsert entity into world
		if (!ep.world->IsEntitySpawned(pEntity))
		{
			ep.world->SpawnEntity(pEntity);

			if (pEntity->GetEntityFlag(EF_CanCollide))
			{
				// unsure that the Entity is 'constructed'
				SimController *pSimController = static_cast<SimController *>(pEntity->GetController());
				if (pSimController && pSimController->GetEntityDynamics())
				{
					pSimController->GetEntityDynamics()->OnConstructed();
				}
			}
		}

		// make entity visible
		pEntity->SetEntityFlag(EF_IsVisible);
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// DespawnGroupAction
//----------------------------------------------------------------------------------------------

bool DespawnGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);

	if (egroup)
	{
		EntityGroup::iterator ei = egroup->m_egroup.begin();
		EntityGroup::iterator ee = egroup->m_egroup.end();

		for (; ei != ee; ei++)
		{
			Entity *pEntity = *ei;

			// ignore entities that are despawned
			if (!pEntity->GetEntityFlag(EF_IsSpawned))
				continue;

			// ignore entities that are dying
			if (QIState<StateDead>(pEntity))
				continue;

			// make entity invisible
			pEntity->ClearEntityFlag(EF_IsVisible);

			if (pEntity->GetEntityFlag(EF_CanCollide) && (pEntity->GetController() == NULL))
			{
				dbTracef("TRIGGER -- Error can't despawn collideable entities that have no controller, EBP '%s'", pEntity->GetControllerBP()->GetFileName());
			}

			// remove it from the world
			ep.world->DeSpawnEntity(pEntity);
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// RespawnGroupAction
//----------------------------------------------------------------------------------------------

bool RespawnGroupAction::Evaluate(TExpression::EvaluateParms &ep)
{
	long groupid = GetArg(0).GetGroupID();

	ESimGroup *egroup = ModObj::i()->GetTriggerFactory()->GetEGroup(groupid);

	if (egroup)
	{
		EntityGroup::iterator ei = egroup->m_egroup.begin();
		EntityGroup::iterator ee = egroup->m_egroup.end();

		for (; ei != ee; ei++)
		{
			Entity *pEntity = *ei;

			// ignore entities that are spawned
			if (pEntity->GetEntityFlag(EF_IsSpawned))
				continue;

			// reinsert entity into world
			if (!ep.world->IsEntitySpawned(pEntity))
			{
				ep.world->SpawnEntity(pEntity);

				// unsure that the Entity is 'constructed'
				if (pEntity->GetEntityFlag(EF_CanCollide))
				{
					SimController *pSimController = static_cast<SimController *>(pEntity->GetController());
					if (pSimController && pSimController->GetEntityDynamics())
					{
						pSimController->GetEntityDynamics()->OnConstructed();
					}
				}
			}

			// make entity visible
			pEntity->SetEntityFlag(EF_IsVisible);
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// RegisterModActions
//----------------------------------------------------------------------------------------------

void RegisterModActions()
{
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Order group to location", OrderGroupToLocationAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Order group to stop", OrderGroupToStopAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Order group to move randomly", OrderGroupToMoveRandomlyAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Order group to do command", OrderGroupToDoCommandAction::Create);

	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Group2Group", Group2GroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GivePlayerCash", GiveCashAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("SetPlayerCash", SetCashAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Win", WinAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Lose", LoseAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("SpawnEntity", SpawnEntityAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("SetGroupOwner", SetGroupOwnerAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("SetGroupDeathFadeDelay", SetGroupDeathFadeDelayAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("MoveEntity", MoveEntityAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("MoveGroup", MoveGroupAction::Create);

	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Get entity group (creature type)", GetEBPTypeGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Get entity group (all)", GetAllGroupAction::Create);

	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupLookAtGroup", GroupLookAtGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupStopLooking", GroupStopLookingAtGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupFaceGroup", GroupFaceGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupFaceGroup (with speed)", GroupFaceGroupSpeedControlAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupFaceDirection", GroupFaceDirectionAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("GroupFaceDirection (with speed)", GroupFaceDirectionSpeedControlAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("UpdateProximityGroup", UpdateProximityGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Deselect all", DeselectAllAction::Create);

	// objective related actions
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Objective, create primary obj", CreatePrimObjectiveAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Objective, create secondary obj", CreateSecObjectiveAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Objective, remove obj", RemoveObjectiveAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Objective, set objective state", SetObjectiveStateAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Objective, bind obj to location", BindObjectiveToLocAction::Create);

	// blip related actions
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Blip, create", CreateBlipAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("Blip, remove", RemoveBlipAction::Create);

	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("DespawnEntity", DespawnEntityAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("RespawnEntity", RespawnEntityAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("DespawnGroup", DespawnGroupAction::Create);
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB("RespawnGroup", RespawnGroupAction::Create);

	return;
}
