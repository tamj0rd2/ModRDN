/////////////////////////////////////////////////////////////////////
// File    : luaRDNplayer.cpp
// Desc    :
// Created : Tuesday, August 07, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "luamodRDN.h"

#include "../../ModObj.h"

#include "../RDNPlayer.h"
#include "../RDNWorld.h"
#include "../RDNEBPs.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/MovingExtInfo.h"

#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>

#include <Lua/LuaConfig.h>

//--------------------------------------------------------------------------------------
// RDNPlayer State data
//--------------------------------------------------------------------------------------

namespace
{
	static RDNPlayer *s_currentPlayer = NULL;
};

//--------------------------------------------------------------------------------------
// Lua RDNPlayer function interface
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_set(lua_State *state)
{
	// validate parm
	if (lua_isnumber(state, 1) == 0)
	{
		lua_print("player_set( ID ): must take a valid player ID");
		return 0;
	}

	// get id
	const long playerId = long(lua_tonumber(state, 1));

	// validate id
	RDNPlayer *player =
			static_cast<RDNPlayer *>(ModObj::i()->GetWorld()->GetPlayerFromID(playerId));

	if (player == 0)
	{
		lua_print("player_set( ID ): invalid player id");
		return 0;
	}

	// set
	s_currentPlayer = player;

	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_addcash(lua_State *state)
{
	if (lua_isnumber(state, 1))
	{
		int numresources = (int)lua_tonumber(state, 1);
		if (numresources < 0)
		{
			goto error;
		}
		if (!s_currentPlayer)
		{
			lua_print("player_addcash(): must call player_set( id ) first.");
			return 0;
		}

		// add renewable resources
		s_currentPlayer->IncResourceCash(float(numresources));

		return 0;
	}
error:
	lua_print("player_addcash( num ): must take a quantity greater than zero");
	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_getcash(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_getcash(): must call player_set( id ) first.");
		return 0;
	}

	// add renewable resources
	const double cash = s_currentPlayer->GetResourceCash();
	lua_pushnumber(state, cash);

	return 1;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_numguys(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_numcreatures() : must call player_set( id ) first.");
		return 0;
	}

	// add renewable resources
	int num = s_currentPlayer->GetNumEntity(Henchmen_EC);
	lua_pushnumber(state, double(num));

	return 1;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_numoftype(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_numoftype( controllertype ) : must call player_set( id ) first.");
		return 0;
	}

	if (lua_isnumber(state, 1))
	{
		int type = (int)lua_tonumber(state, 1);
		if (type > 0 && type < MAX_EC)
		{
			int num = s_currentPlayer->GetNumEntity(type);
			lua_pushnumber(state, double(num));
			return 1;
		}
	}

	lua_print("player_numoftype( controllertype ): invalid argument or range");
	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_addentity(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_addentity(): must call player_set( id ) first.");
		return 0;
	}

	int tag = LuaConfig::GetTag(state, "entity");

	if (tag == lua_tag(state, 1))
	{
		Entity *entity = (Entity *)lua_touserdata(state, 1);
		s_currentPlayer->AddEntity(entity);
		return 0;
	}

	lua_print("player_addentity( entity ): invalid armument");

	return 0;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

static int player_population(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_population(): must call player_set( id ) first.");
		return 0;
	}

	const double population = s_currentPlayer->PopulationCurrent();
	lua_pushnumber(state, population);

	return 1;
}

static int player_isdead(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_isdead( ): must call player_set( id ) first.");
		return 0;
	}

	lua_pushnumber(state, (double)s_currentPlayer->IsPlayerDead());
	return 1;
}

static int player_kill(lua_State *)
{
	if (!s_currentPlayer)
	{
		lua_print("player_kill( ): must call player_set( id ) first.");
		return 0;
	}

	if (s_currentPlayer->IsPlayerDead() == 0)
		s_currentPlayer->KillPlayer(s_currentPlayer->KPR_Trigger);

	return 0;
}

static int player_isally(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_isally( playerid ): must call player_set( id ) first.");
		return 0;
	}

	if (lua_isnumber(state, 1))
	{
		int playerId = static_cast<int>(lua_tonumber(state, 1));
		const Player *otherPlayer = ModObj::i()->GetWorld()->GetPlayerFromID(playerId);
		if (otherPlayer)
		{
			int val = s_currentPlayer == otherPlayer;
			lua_pushnumber(state, val);
			return 1;
		}
	}

	lua_print("player_isally( playerid ): returns 1 if allies 0 if not");

	return 0;
}

static int player_getentities(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_getentities( ): must call player_set( id ) first.");
		return 0;
	}

	const EntityGroup *pGroup = &s_currentPlayer->GetEntities();
	int tag = LuaConfig::GetTag(state, "entitygroup");

	lua_pushusertag(state, const_cast<EntityGroup *>(pGroup), tag);
	return 1;
}

static int player_getgroup(lua_State *state)
{
	if (!s_currentPlayer)
	{
		lua_print("player_getgroup( type ): must call player_set( id ) first.");
		return 0;
	}

	if (lua_isnumber(state, 1))
	{
		int type = (int)lua_tonumber(state, 1);
		const EntityGroup *pGroup = &s_currentPlayer->GetEntityGroup(type);
		int tag = LuaConfig::GetTag(state, "entitygroup");

		lua_pushusertag(state, (void *)pGroup, tag);
		return 1;
	}

	lua_print("entitygroup = player_getgroup(type): pass in controller type, returns entitygroup");

	return 0;
}

/////////////////////////////////////////////////////////////////////
//

namespace
{
	struct LuaFunction
	{
		LuaConfig::LuaFunc funcPtr;
		const char *funcName;
	};

#define LUAFUNC(n) \
	{                \
		&n, #n         \
	}

	static const LuaFunction s_exported[] =
			{
					LUAFUNC(player_set),
					LUAFUNC(player_addcash),
					LUAFUNC(player_getcash),
					LUAFUNC(player_numguys),
					LUAFUNC(player_numoftype),
					LUAFUNC(player_addentity),
					LUAFUNC(player_population),
					LUAFUNC(player_isally),
					LUAFUNC(player_kill),
					LUAFUNC(player_isdead),
					LUAFUNC(player_getentities),
					LUAFUNC(player_getgroup)};

#undef LUAFUNC

} // namespace

/* Open function */
void LuaRDNPlayerLib::Initialize(LuaConfig *lc)
{
	// register any functions
	size_t i = 0;
	size_t e = sizeof(s_exported) / sizeof(s_exported[0]);

	for (; i != e; ++i)
	{
		lc->RegisterCFunc(s_exported[i].funcName, s_exported[i].funcPtr);
	}

	return;
}

/* Close function */
void LuaRDNPlayerLib::Shutdown(LuaConfig *lc)
{
	// register any functions
	size_t i = 0;
	size_t e = sizeof(s_exported) / sizeof(s_exported[0]);

	for (; i != e; ++i)
	{
		lc->ClearFunction(s_exported[i].funcName);
	}

	return;
}
