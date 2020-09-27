
#include "pch.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"

#include "CommandTypes.h"
#include "RDNWorld.h"
#include "RDNQuery.h"
#include "RDNPlayer.h"
#include "RDNEBPs.h"
#include "ModTriggerTypes.h"

#include "Controllers/ModController.h"

#include "Extensions/HealthExt.h"
#include "Extensions/ResourceExt.h"

#include "GameEventSys.h"
#include "GameEventDefs.h"

#include "../UI/Objective.h"
#include "../UI/ObjectiveFactory.h"

#include <EngineAPI/entityfactory.h>
#include <EngineAPI/triggerfactory.h>
#include <EngineApi/CommandInterface.h>
#include <EngineAPI/SelectionInterface.h>
#include <EngineAPI/CameraInterface.h>

#include <SimEngine/texpression.h>
#include <SimEngine/entity.h>
#include <SimEngine/entitycontroller.h>
#include <SimEngine/SpatialBucket.h>
#include <SimEngine/SpatialBucketSystem.h>

//------------------------------------------------------------------
// HQDeadCondition
//------------------------------------------------------------------

class HQDeadCondition: public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(HQDeadCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new HQDeadCondition;}
};

//----------------------------------------------------------------------------------------------
// WinAction Class
//----------------------------------------------------------------------------------------------

class EnemiesDeadCondition: public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(EnemiesDeadCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new EnemiesDeadCondition;}
};

//----------------------------------------------------------------------------------------------
// EntityHealthCondition Class
//----------------------------------------------------------------------------------------------

class EntityHealthCondition: public TExpression
{
public:
	enum {DEF_ENTITY, DEF_COMPARE, DEF_INT};
	//------------------------------------------
	TEXPRESSION_CLONE(EntityHealthCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new EntityHealthCondition;}
};

//----------------------------------------------------------------------------------------------
// GroupHealthCondition Class
//----------------------------------------------------------------------------------------------

class GroupHealthCondition: public TExpression
{
public:
	enum {DEF_GROUPID, DEF_COMPARE, DEF_INT};
	//------------------------------------------
	TEXPRESSION_CLONE(GroupHealthCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new GroupHealthCondition;}
};

//----------------------------------------------------------------------------------------------
// IsEntityDeadCondition Class
//----------------------------------------------------------------------------------------------

class IsEntityDeadCondition: public TExpression
{
public:
	enum {DEF_ENTITY};
	//------------------------------------------
	TEXPRESSION_CLONE(IsEntityDeadCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new IsEntityDeadCondition;}

};

//----------------------------------------------------------------------------------------------
// IsGroupDeadCondition Class
//----------------------------------------------------------------------------------------------

class IsGroupDeadCondition: public TExpression
{
public:
	enum {DEF_GROUP};
	//------------------------------------------
	TEXPRESSION_CLONE(IsGroupDeadCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new IsGroupDeadCondition;}

};

//----------------------------------------------------------------------------------------------
// ArmySizeCondition Class
//----------------------------------------------------------------------------------------------

class ArmySizeCondition: public TExpression
{
public:
	enum {DEF_COMPARE, DEF_INT};
	//------------------------------------------
	TEXPRESSION_CLONE(ArmySizeCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new ArmySizeCondition;}
};

//----------------------------------------------------------------------------------------------
// GroupMemebersSelectedCondition Class
//----------------------------------------------------------------------------------------------

class GroupMemebersSelectedCondition: public TExpression
{
public:
	TEXPRESSION_CLONE(GroupMemebersSelectedCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return false; }
	//------------------------------------------
	static TExpression*	Create( ) {return new GroupMemebersSelectedCondition;}
};

//----------------------------------------------------------------------------------------------
// SinglySelectedEntityTypeCondition Class
//----------------------------------------------------------------------------------------------

class SinglySelectedEntityTypeCondition: public TExpression
{
public:
	TEXPRESSION_CLONE(SinglySelectedEntityTypeCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return false; }
	//------------------------------------------
	static TExpression*	Create( ) {return new SinglySelectedEntityTypeCondition;}
};

//----------------------------------------------------------------------------------------------
// SelectedMultEntitiesOfTypeCondition Class
//----------------------------------------------------------------------------------------------

class SelectedMultEntitiesOfTypeCondition: public TExpression
{
public:
	TEXPRESSION_CLONE(SelectedMultEntitiesOfTypeCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return false; }
	//------------------------------------------
	static TExpression*	Create( ) {return new SelectedMultEntitiesOfTypeCondition;}
};

//----------------------------------------------------------------------------------------------
// PlayerHasEntityTypeCondition Class
//----------------------------------------------------------------------------------------------

class PlayerHasEntityTypeCondition: public TExpression
{
public:
	TEXPRESSION_CLONE(PlayerHasEntityTypeCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new PlayerHasEntityTypeCondition;}
};

//----------------------------------------------------------------------------------------------
// GetObjectiveCondition Class
//----------------------------------------------------------------------------------------------

class GetObjectiveCondition: public TExpression
{
public:
	TEXPRESSION_CLONE(GetObjectiveCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return false; }
	//------------------------------------------
	static TExpression*	Create( ) {return new GetObjectiveCondition;}
};

//----------------------------------------------------------------------------------------------
// EntityInProximityCondition Class
//----------------------------------------------------------------------------------------------

class EntityInProximityCondition: public TExpression
{
public:
	enum {DEF_DIST, DEF_LOCATION};
	//------------------------------------------
	TEXPRESSION_CLONE(EntityInProximityCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new EntityInProximityCondition;}
};

//----------------------------------------------------------------------------------------------
// HowMuchCashCondition Class
//----------------------------------------------------------------------------------------------

class HowMuchCashCondition: public TExpression
{
public:
	enum {DEF_PLAYER, DEF_COMPARE, DEF_INT};
	//------------------------------------------
	TEXPRESSION_CLONE(HowMuchCashCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new HowMuchCashCondition;}

private:
	bool check(Player* player, TArgument::CompareType compare, long num);
};

//----------------------------------------------------------------------------------------------
// GroupCashCondition Class
//----------------------------------------------------------------------------------------------

class GroupCashCondition: public TExpression
{
public:
	//------------------------------------------
	TEXPRESSION_CLONE(GroupCashCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new GroupCashCondition;}

private:
	bool CompareCash( float Cash, TArgument::CompareType compare, long amount );
};

//----------------------------------------------------------------------------------------------
// PlayerSpawnedNewEntityCondition Class
//----------------------------------------------------------------------------------------------

class PlayerSpawnedNewEntityCondition: public TExpression
{
public:
	PlayerSpawnedNewEntityCondition() : m_player( NULL ), m_bNewEntity( false ) {}
	//------------------------------------------
	TEXPRESSION_CLONE(PlayerSpawnedNewEntityCondition);
	//------------------------------------------
	virtual bool		Evaluate( TExpression::EvaluateParms& ep );
	virtual bool		Deterministic() { return true; }
	//------------------------------------------
	static TExpression*	Create( ) {return new PlayerSpawnedNewEntityCondition;}

private:
	RDNPlayer*	m_player;
	bool			m_bNewEntity;
};

//==============================================================================================
// Implementation
//==============================================================================================

//----------------------------------------------------------------------------------------------
// HQDeadCondition
//----------------------------------------------------------------------------------------------

bool HQDeadCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	RDNPlayer* splayer = static_cast<RDNPlayer*>(ep.player);

	if (splayer->GetHQEntity() == NULL && splayer->IsPlayerDead() == false)
	{
		return true;
	}
	
	// this means homebase equals null and the base is dead ( the player has to be alive for this )
	return false;
}


//----------------------------------------------------------------------------------------------
// EnemiesDeadCondition
//----------------------------------------------------------------------------------------------

bool EnemiesDeadCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	Player* player = ep.player;

	size_t playerCount = ep.world->GetPlayerCount();

	for (size_t i=0; i< playerCount; ++i)
	{
		Player* checkPlayer = ep.world->GetPlayerAt(i);

		if (checkPlayer != player)
		{
			if (checkPlayer->IsPlayerDead() == false)
			{
				// one of your enemies is still alive
				return false;
			}
		}
	}
	
	// this means all your enemies are dead cuz it didn't hit the false above
	return true;
}

//----------------------------------------------------------------------------------------------
// EntityHealthCondition Class
//----------------------------------------------------------------------------------------------

bool EntityHealthCondition::Evaluate( TExpression::EvaluateParms& ep)
{
	long entityid					= GetArg(DEF_ENTITY).GetEntity();
	TArgument::CompareType compare	= GetArg(DEF_COMPARE).GetCompare();
	int val							= GetArg(DEF_INT).GetInt();

	// retrieve entity
	Entity* pEntity = ep.world->GetEntityFactory()->GetEntityFromEID( entityid );	
	if (!pEntity)
	{
		dbWarningf('TRIG', "EntityHealthCondition: Unknown entity id(%d)", entityid);
		return false;
	}

	// retrieve health
	float healthVal = 0.0f;
	HealthExt* h = QIExt< HealthExt >( pEntity->GetController() );
	if (h)
	{
		healthVal = h->GetHealth();
	}

	if (compare == TArgument::CMP_LTEQ && (healthVal <= val))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (healthVal >= val))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (healthVal == val))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (healthVal != val))
	{
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------
// GroupHealthCondition Class
//----------------------------------------------------------------------------------------------

bool GroupHealthCondition::Evaluate( TExpression::EvaluateParms& )
{
	long groupid					= GetArg(DEF_GROUPID).GetGroupID();
	TArgument::CompareType compare	= GetArg(DEF_COMPARE).GetCompare();
	int percent						= GetArg(DEF_INT).GetInt();
	
	ESimGroup* pEGroup = ModObj::i()->GetTriggerFactory()->GetEGroup( groupid );
	if (!pEGroup)
	{
		dbWarningf('TRIG', "GroupHealthCondition: Group not found");
		return false;
	}

	// compute group health = sum(entity hitpoint) / sum(max entity hitpoint)
	EntityGroup::iterator ei = pEGroup->m_egroup.begin();
	EntityGroup::iterator ee = pEGroup->m_egroup.end();

	float healthTotal    = 0.0f;
	float healthMaxTotal = 0.0f;

	for (; ei != ee; ei++)
	{
		Entity* pEntity = *ei;

		// retrieve health
		HealthExt* h = QIExt< HealthExt >( pEntity->GetController() );
		if (h)
		{
			healthTotal		+= h->GetHealth();
			healthMaxTotal	+= h->GetHealthMax();
		}
	}

	float groupHealth = 0.0f;
	if (healthMaxTotal > 0.0f)
	{
		groupHealth = healthTotal / healthMaxTotal;
	}

	float frac = percent / 100.0f;

	if (compare == TArgument::CMP_LTEQ && (groupHealth <= frac))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (groupHealth >= frac))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (groupHealth == frac))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (groupHealth != frac))
	{
		return true;
	}
	
	return false;
}



//----------------------------------------------------------------------------------------------
// ArmySizeCondition Class
//----------------------------------------------------------------------------------------------

bool ArmySizeCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	TArgument::CompareType compare	= GetArg(DEF_COMPARE).GetCompare();
	int size = GetArg(DEF_INT).GetInt();

	RDNPlayer* splayer = static_cast<RDNPlayer*>(ep.player);
	int armySize = splayer->GetArmy().size();

	if (compare == TArgument::CMP_LTEQ && (armySize <= size))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (armySize >= size))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (armySize == size))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (armySize != size))
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// GroupMemebersSelectedCondition Class
//----------------------------------------------------------------------------------------------

bool GroupMemebersSelectedCondition::Evaluate( TExpression::EvaluateParms& )
{
	long						groupid		= GetArg(0).GetGroupID();
	TArgument::CompareType		compare		= GetArg(1).GetCompare();
	long						num			= GetArg(2).GetInt();

	ESimGroup* egroup = ModObj::i()->GetTriggerFactory()->GetEGroup( groupid );
	const EntityGroup& selGroup = ModObj::i()->GetSelectionInterface()->GetSelection();
	
	if (egroup)
	{
		EntityGroup::const_iterator ei = egroup->m_egroup.begin();
		EntityGroup::const_iterator ee = egroup->m_egroup.end();

		int numSelected = 0;
		for (; ei != ee; ei++) 
		{
			Entity* pEntity = *ei;

			if (selGroup.find(pEntity) != selGroup.end())
			{
				numSelected++;
			}
		}

		if (compare == TArgument::CMP_LTEQ && (numSelected <= num))
		{
			return true;
		}
		else
		if (compare == TArgument::CMP_GTEQ && (numSelected >= num))
		{
			return true;
		}
		else
		if (compare == TArgument::CMP_EQ && (numSelected == num))
		{
			return true;
		}
		else
		if (compare == TArgument::CMP_NEQ && (numSelected != num))
		{
			return true;
		}
	}
	else
	{
		dbWarningf('TRIG', "GroupMemebersSelectedCondition: group does not exist");
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// SinglySelectedEntityTypeCondition Class
//----------------------------------------------------------------------------------------------

bool SinglySelectedEntityTypeCondition::Evaluate( TExpression::EvaluateParms& ep )
{	
	long ebpNetId = GetArg(0).GetEBPNetID();

	const EntityGroup& selGroup = ModObj::i()->GetSelectionInterface()->GetSelection();
	EntityGroup::const_iterator ei = selGroup.begin();
	EntityGroup::const_iterator ee = selGroup.end();

	int numMatch = 0;

	for (; ei != ee; ei++)
	{
		Entity* pEntity = *ei;

		// right player and right EBP
		if ((pEntity->GetOwner() == ep.player) && (pEntity->GetControllerBP()->GetEBPNetworkID() == ebpNetId))
		{
			numMatch++;
		}
	}

	return (numMatch == 1);
}

//----------------------------------------------------------------------------------------------
// SelectedMultEntitiesOfTypeCondition Class
//----------------------------------------------------------------------------------------------

bool SelectedMultEntitiesOfTypeCondition::Evaluate( TExpression::EvaluateParms& ep )
{	
	TArgument::CompareType compare	= GetArg(0).GetCompare();
	long num						= GetArg(1).GetInt();
	long ebpNetId					= GetArg(2).GetEBPNetID();

	const EntityGroup& selGroup = ModObj::i()->GetSelectionInterface()->GetSelection();
	EntityGroup::const_iterator ei = selGroup.begin();
	EntityGroup::const_iterator ee = selGroup.end();

	int numMatch = 0;

	for (; ei != ee; ei++)
	{
		Entity* pEntity = *ei;

		// right player and right EBP
		if ((pEntity->GetOwner() == ep.player) && (pEntity->GetControllerBP()->GetEBPNetworkID() == ebpNetId))
		{
			numMatch++;
		}
	}

	if (compare == TArgument::CMP_LTEQ && (numMatch <= num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (numMatch >= num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (numMatch == num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (numMatch != num))
	{
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------
// PlayerHasEntityTypeCondition Class
//----------------------------------------------------------------------------------------------

bool PlayerHasEntityTypeCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	long ebpNetID = GetArg(0).GetEBPNetID();

	const EntityGroup& egroup = ep.player->GetEntities();
	EntityGroup::const_iterator ei = egroup.begin();
	EntityGroup::const_iterator ee = egroup.end();

	for (; ei != ee; ei++)
	{
		Entity* pEntity = *ei;

		if (pEntity->GetControllerBP()->GetEBPNetworkID() == ebpNetID)
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// GetObjectiveCondition
//----------------------------------------------------------------------------------------------

bool GetObjectiveCondition::Evaluate( TExpression::EvaluateParms& )
{
	long objID									= GetArg(0).GetInt();
	ModTriggerTypes::ObjectiveState objState	= 
		static_cast<ModTriggerTypes::ObjectiveState>( GetArg(1).GetEnum() );

	Objective* pObj = ModObj::i()->GetObjectiveFactory()->GetObjective( objID );
	if ( pObj )
	{
		return (pObj->GetState() == static_cast<Objective::State>(objState));
	}

	dbWarningf('TRIG', "GetObjectiveCondition: Objective not found (%d)", objID);
	return false;
}

//----------------------------------------------------------------------------------------------
// EntityInProximityCondition Class
//----------------------------------------------------------------------------------------------

bool EntityInProximityCondition::Evaluate( TExpression::EvaluateParms& ep)
{
	int   distance   = GetArg(DEF_DIST).GetInt();

	long    locationID = GetArg(DEF_LOCATION).GetEntity();
	Entity* location   = ep.world->GetEntityFactory()->GetEntityFromEID(locationID);
	if (!location)
	{
		dbWarningf('TRIG', "EntityInProximityCondition: location not found");
		return false;
	}

	// find spatial buckets in the proximity of the location
	SimWorld*				pSimWorld	= static_cast<SimWorld*>( ep.world );

	FindClosestUnitWithOwnerFilter filter( ep.player );

	return ( pSimWorld->FindClosestEntity( filter, location->GetPosition(), float(distance), location ) != NULL );
}

//----------------------------------------------------------------------------------------------
// HowMuchCashCondition Class
//----------------------------------------------------------------------------------------------

bool HowMuchCashCondition::Evaluate( TExpression::EvaluateParms& ep)
{
	int							playerid	= GetArg(DEF_PLAYER).GetPlayer();
	TArgument::CompareType		compare		= GetArg(DEF_COMPARE).GetCompare();
	long						num			= GetArg(DEF_INT).GetInt();
	int playerIndex = 0;
	
	// find out what players should lose in relation to the player running this trigger

	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(playerid), playerIndex))
	{
		Player* player = ep.world->GetPlayerAt(playerIndex);
		return check( player, compare, num );
	}
	else
	if (playerid == TArgument::PS_CURRENTPLAYER)
	{
		Player* player = ep.player;
		return check( player, compare, num );
	}
	else
	if (playerid == TArgument::PS_ALLENEMIES)
	{
		// find all this players enemies
		size_t numplayers = ep.world->GetPlayerCount();
		
		// kill all other players ?
		for (size_t i=0; i<numplayers; ++i)
		{
			Player* checkPlayer = ep.world->GetPlayerAt(i);
			if ( checkPlayer != ep.player )
			{
				if (! check( checkPlayer, compare, num ) )
				{
					return false;
				}
			}
		}

		return true;
	}
	else
	{
		dbWarningf('TRIG', "HowMuchCashCondition: player not defined... error!");
		SetValid( false );
	}

	return false;
}

bool HowMuchCashCondition::check(Player* player, TArgument::CompareType compare, long num)
{
	RDNPlayer* pRDNPlayer = static_cast<RDNPlayer*>(player);
	float Cash = pRDNPlayer->GetResourceCash();

	if (compare == TArgument::CMP_LTEQ && (Cash <= num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (Cash >= num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (Cash == num))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (Cash != num))
	{
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------
// IsGroupDeadCondition
//----------------------------------------------------------------------------------------------

bool IsGroupDeadCondition::Evaluate( TExpression::EvaluateParms& )
{
	long groupid = GetArg(0).GetGroupID();

	//
	ESimGroup* egroup = ModObj::i()->GetTriggerFactory()->GetEGroup( groupid );
	
	// check parm
	if (egroup && egroup->m_egroup.size() == 0)
	{
		return true;
	}

	// check to see if all entities have zero health
	EntityGroup::iterator ei = egroup->m_egroup.begin();
	EntityGroup::iterator ee = egroup->m_egroup.end();

	for (; ei != ee; ei++)
	{
		Entity* pEntity = *ei;
		HealthExt* h = QIExt< HealthExt >( pEntity->GetController() );

		if (h && h->GetHealth() > 0.0f)
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// IsEntityDeadCondition
//----------------------------------------------------------------------------------------------

bool IsEntityDeadCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	long entityid = GetArg(0).GetEntity();

	Entity* pEntity = ep.world->GetEntityFactory()->GetEntityFromEID( entityid );
	
	// if we can't find the entity then it must be dead
	if (pEntity == NULL)
	{
		return true;
	}
	else
	{
		// check to see if the entity's got zero health
		HealthExt* h = QIExt< HealthExt >( pEntity->GetController() );
		if (h && h->GetHealth() == 0.0f)
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------------------------------
// GroupCashCondition
//----------------------------------------------------------------------------------------------

bool GroupCashCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	UNREF_P( ep );

	long					groupID = GetArg(0).GetGroupID();
	TArgument::CompareType	compare	= GetArg(1).GetCompare();
	float					amount	= GetArg(2).GetFloat();

	ESimGroup* egroup = ModObj::i()->GetTriggerFactory()->GetEGroup( groupID );

	if (egroup)
	{
		EntityGroup::iterator iter;
		for (iter = egroup->m_egroup.begin(); iter != egroup->m_egroup.end(); iter++)
		{
			Entity*      e = *iter;

			// ignore group members that are not Cash yards
			ResourceExt* resourceExt = QIExt< ResourceExt >( e );
			if (!resourceExt)
				continue;

			if ( !CompareCash( resourceExt->GetResources(), compare, (long)amount ) )
			{
				return false;
			}
		}

		return true;
	}

	dbWarningf('TRIG', "GroupCashCondition: Unable to retrieve group (%d)", groupID);
	SetValid( false );
	return false;
}

bool GroupCashCondition::CompareCash( float Cash, TArgument::CompareType compare, long amount )
{
	if (compare == TArgument::CMP_LTEQ && (Cash <= amount))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_GTEQ && (Cash >= amount))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_EQ && (Cash == amount))
	{
		return true;
	}
	else
	if (compare == TArgument::CMP_NEQ && (Cash != amount))
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// PlayerSpawnedNewEntityCondition
//----------------------------------------------------------------------------------------------

bool PlayerSpawnedNewEntityCondition::Evaluate( TExpression::EvaluateParms& ep )
{
	// if this is not the first evaluation...
	if (m_player)
	{
		bool newEntity = m_bNewEntity;

		// reset flag
		m_bNewEntity = false;

		return newEntity;			
	}

	// otherwise, find out what player this call is for
	long playerid = GetArg(0).GetPlayer();
	int playerIndex = 0;
	if (TArgument::GetPlayerIndex(static_cast<TArgument::PlayerSet>(playerid), playerIndex))
	{
		m_player = static_cast< RDNPlayer* >(ep.world->GetPlayerAt(playerIndex));
	}
	else
	if (playerid == TArgument::PS_CURRENTPLAYER)
	{
		m_player = static_cast< RDNPlayer* >(ep.player);
	}
	else
	{
		dbFatalf("MOD -- Trigger PlayerSpawnedNewEntityCondition has an invalid player" );
	}

	return false;
}

//----------------------------------------------------------------------------------------------
// RegisterModActions
//----------------------------------------------------------------------------------------------

void RegisterModConditions()
{
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "HQDead", HQDeadCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "EnemiesDead", EnemiesDeadCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "EntityHealth", EntityHealthCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "GroupHealth", GroupHealthCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Group members selected", GroupMemebersSelectedCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Player has selected entity-type", SinglySelectedEntityTypeCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Player has selected so many of entity-type", SelectedMultEntitiesOfTypeCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Player has entity-type", PlayerHasEntityTypeCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Objective, get objective state", GetObjectiveCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "EntityInProximity", EntityInProximityCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "EntityNotAlive", IsEntityDeadCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "GroupNotAlive", IsGroupDeadCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "Group Cash", GroupCashCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "PlayerSpawnedNewEntity", PlayerSpawnedNewEntityCondition::Create );
	ModObj::i()->GetTriggerFactory()->RegisterExpressionCB( "HowMuchCash", HowMuchCashCondition::Create );
}