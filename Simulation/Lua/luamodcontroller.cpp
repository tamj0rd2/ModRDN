/////////////////////////////////////////////////////////////////////
// File    : luamodcontroller.cpp
// Desc    :
// Created : Wednesday, September 26, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "luamodRDN.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"

#include "../Controllers/ModController.h"
#include "../Controllers/GuyController.h"
#include "../Controllers/HQController.h"

#include "../States/State.h"

#include "../Extensions/HealthExt.h"

#include "../ExtInfo/CostExtInfo.h"
#include "../ExtInfo/HealthExtInfo.h"
#include "../ExtInfo/MovingExtInfo.h"

#include <SimEngine/Entity.h>

#include <EngineAPI/EntityFactory.h>

#include <Lua/LuaBinding.h>

/////////////////////////////////////////////////////////////////////
//

// internal func for getting a controllerblueprint arg from a lua function call

static EntityController *priv_getcontroller(lua_State *state)
{
	int entitytag = LuaConfig::GetTag(state, "entity");
	int controllertag = LuaConfig::GetTag(state, "entitycontroller");

	if (entitytag == lua_tag(state, 1))
	{
		Entity *entity = (Entity *)lua_touserdata(state, 1);
		return entity->GetController();
	}
	else if (controllertag == lua_tag(state, 1))
	{
		EntityController *controller = (EntityController *)lua_touserdata(state, 1);
		return controller;
	}

	return NULL;
}

static const ControllerBlueprint *priv_getcbp(lua_State *state)
{
	int entitytag = LuaConfig::GetTag(state, "entity");
	int cbptag = LuaConfig::GetTag(state, "controllerBP");

	const ControllerBlueprint *cbp = NULL;

	if (cbptag == lua_tag(state, 1))
	{
		cbp = (const ControllerBlueprint *)lua_touserdata(state, 1);
	}
	else if (entitytag == lua_tag(state, 1))
	{
		Entity *entity = (Entity *)lua_touserdata(state, 1);
		if (entity)
		{
			cbp = entity->GetControllerBP();
		}
	}

	return cbp;
}

static const Entity *priv_getentity(lua_State *state, int argIndex)
{
	int entitytag = LuaConfig::GetTag(state, "entity");

	Entity *entity = NULL;

	if (entitytag == lua_tag(state, argIndex))
	{
		entity = (Entity *)lua_touserdata(state, argIndex);
	}

	return entity;
}

// bunch of queries to the extensions of an entity

static int ent_costcash(lua_State *state)
{
	const ControllerBlueprint *cbp = priv_getcbp(state);

	if (cbp)
	{
		const ECStaticInfo *si =
				ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

		const CostExtInfo *cost = QIExtInfo<CostExtInfo>(si);

		if (cost)
		{
			lua_pushnumber(state, (double)cost->costCash);
			return 1;
		}
	}
	else
	{
		lua_print("ent_costcash(entity OR cbp): invalid arguments");
	}

	return 0;
}

static int ent_maxspeed(lua_State *state)
{
	const ControllerBlueprint *cbp = priv_getcbp(state);

	if (cbp)
	{
		const ECStaticInfo *si =
				ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

		const MovingExtInfo *move = QIExtInfo<MovingExtInfo>(si);

		if (move)
		{
			lua_pushnumber(state, (double)move->speed);
			return 1;
		}
	}
	else
	{
		lua_print("ent_maxspeed(entity OR cbp): invalid arguments");
	}

	return 0;
}

static int ent_maxhealth(lua_State *state)
{
	const ControllerBlueprint *cbp = priv_getcbp(state);

	if (cbp)
	{
		const ECStaticInfo *si =
				ModObj::i()->GetWorld()->GetEntityFactory()->GetECStaticInfo(cbp);

		const HealthExtInfo *health = QIExtInfo<HealthExtInfo>(si);

		if (health)
		{
			lua_pushnumber(state, (double)health->health);
			return 1;
		}
		//	lua_print("ent_maxhealth(entity OR cbp): no health extension");
	}
	else
	{
		lua_print("ent_maxhealth(entity OR cbp): invalid arguments");
	}

	return 0;
}

static int ent_health(lua_State *state)
{
	EntityController *controller = priv_getcontroller(state);

	if (controller)
	{
		HealthExt *healthExt = QIExt<HealthExt>(controller);

		if (healthExt)
		{
			// set function
			if (lua_isnumber(state, 2))
			{
				float newhealth = (float)lua_tonumber(state, 2);
				healthExt->SetHealth(newhealth);
				return 0;
			}
			// get function
			else
			{
				lua_pushnumber(state, (double)healthExt->GetHealth());
				return 1;
			}
		}
		//	lua_print("ent_health(entity): no health extension");
	}
	else
	{
		lua_print("ent_health(entity): invalid arguments");
	}

	return 0;
}

static int ent_state(lua_State *state)
{
	const Entity *ent = priv_getentity(state, 1);

	if (ent)
	{
		if (ent->GetController())
		{
			const ModController *pMC = static_cast<const ModController *>(ent->GetController());
			if (pMC->QIActiveState(State::SID_Current))
			{
				lua_pushnumber(state, (int)pMC->QIActiveState(State::SID_Current)->GetStateID());
				return 1;
			}
		}
	}
	else
	{
		lua_print("ent_state(entity): invalid arguments");
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
//

namespace
{
#define LUAFUNC(n) \
	{                \
#n, &n         \
	}

	const LuaBinding::StaticBound s_exported[] =
			{
					LUAFUNC(ent_costcash),
					LUAFUNC(ent_maxspeed),
					LUAFUNC(ent_maxhealth),
					LUAFUNC(ent_health),
					LUAFUNC(ent_state)};

#undef LUAFUNC

} // namespace

/* Open function */
void LuaModControllerLib::Initialize(LuaConfig *lc)
{
	//
	LuaBinding::StaticBind(*lc, s_exported, LENGTHOF(s_exported));

	//
	lc->AddTag("entity");
	lc->AddTag("controllerBP");

#define BINDCONSTANT(c) \
	lc->SetNumber(#c, double(c))

	BINDCONSTANT(Guy_EC);
	BINDCONSTANT(Lab_EC);

#undef BINDCONSTANT
}

/* Close functions */
void LuaModControllerLib::Shutdown(LuaConfig *lc)
{
	LuaBinding::StaticClear(*lc, s_exported, LENGTHOF(s_exported));
}