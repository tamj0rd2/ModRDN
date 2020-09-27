/////////////////////////////////////////////////////////////////////
// File    : luaRDNworld.cpp
// Desc    : 
// Created : Thursday, June 14, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "luamodRDN.h"

#include "../../ModObj.h"

#include "../RDNPlayer.h"
#include "../RDNWorld.h"

#include "../AttackTypes.h"
#include "../CommandTypes.h"

#include <Lua/LuaConfig.h>

///////////////////////////////////////////////////////////////////// 
// 

static bool priv_getCommand( lua_State* L, int index, unsigned long& command )
{
	// init out parm
	command = ULONG_MAX;

	// check type
	if( lua_type( L, index ) != LUA_TNUMBER )
		return false;

	// grab it
	command = static_cast<unsigned long>( lua_tonumber( L, index ) );

	return true;
}

static bool priv_getCommandPoint( lua_State* L, int nX, int nY, int nZ, Vec3f& pos )
{
	// init out parm
	pos = Vec3f( 0, 0, 0 );

	// check type
	if( lua_type( L, nX ) != LUA_TNUMBER ||
		lua_type( L, nY ) != LUA_TNUMBER ||
		lua_type( L, nZ ) != LUA_TNUMBER )
		return false;

	// grab it
	pos.x = static_cast<float>( lua_tonumber( L, nX ) );
	pos.y = static_cast<float>( lua_tonumber( L, nY ) );
	pos.z = static_cast<float>( lua_tonumber( L, nZ ) );

	return true;
}

static bool priv_getCommandPlayer( lua_State* L, int index, unsigned long& player )
{
	// init out parm
	player = 0;

	// check type
	if( lua_type( L, index ) != LUA_TNUMBER )
		return false;

	// grab it
	player = static_cast<unsigned long>( lua_tonumber( L, index ) );

	return true;
}

static bool priv_getCommandGroup( lua_State* L, int index, EntityGroup& g )
{
	// init out parm
	g.clear();

	// check type
	if( lua_type( L, index ) != LUA_TUSERDATA )
		return false;

	// check tag
	const int tag = LuaConfig::GetTag( L, "entitygroup" );

	if( lua_tag( L, index ) != tag )
		return false;

	// grab it
	const EntityGroup* p = reinterpret_cast<const EntityGroup*>( lua_touserdata( L, index ) );
	g = *p;

	return true;
}

static bool priv_getCommandParm( lua_State* L, int index, unsigned long& parm )
{
	// init out parm
	parm = 0;

	// check type
	if( lua_type( L, index ) == LUA_TNUMBER )
	{
		parm = static_cast<unsigned long>( lua_tonumber( L, index ) );
	}
	else
	if( lua_type( L, index ) == LUA_TUSERDATA )
	{
		const int tagCBP = LuaConfig::GetTag( L, "controllerBP" );

		if( lua_tag( L, index ) == tagCBP )
		{
			const ControllerBlueprint* cbp = reinterpret_cast<const ControllerBlueprint*>( lua_touserdata( L, index ) );
			parm = cbp->GetEBPNetworkID();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------
// Lua RDNWorld function interface
//--------------------------------------------------------------------------------------

static int world_gettime( lua_State* state )
{
	float time = ModObj::i()->GetWorld()->GetGameTime();
	lua_pushnumber( state, time );
	return 1;
}

static int world_getticks( lua_State* state )
{
	unsigned long time = ModObj::i()->GetWorld()->GetGameTicks();
	lua_pushnumber( state, (double)time );
	return 1;
}

static int world_getnumentities( lua_State* state )
{
	int numentities = ModObj::i()->GetWorld()->GetNumEntities();
	lua_pushnumber( state, (double)numentities );
	return 1;
}

static int world_getplayercount( lua_State* state )
{
	size_t playercount = ModObj::i()->GetWorld()->GetPlayerCount();
	lua_pushnumber( state, (double)playercount );
	return 1;
}

static int world_getplayerat( lua_State* state )
{
	//
	const size_t playercount = ModObj::i()->GetWorld()->GetPlayerCount();

	size_t index;
	
	if( !lua_isnumber( state, 1 ) || ( index = size_t( lua_tonumber( state, 1 ) ) ) >= playercount )
	{
		lua_print("world_getplayerat( index ): invalid args");

		return 0;
	}

	//
	const Player* player = ModObj::i()->GetWorld()->GetPlayerAt( index );

	lua_pushnumber( state, double( player->GetID() ) );
	return 1;
}

static int world_getrand( lua_State* state )
{
	float frand = ModObj::i()->GetWorld()->GetRand();
	lua_pushnumber( state, (double)frand );
	return 1;
}

static int world_isgameover( lua_State* state )
{
	bool isit = ModObj::i()->GetWorld()->IsGameOver();
	lua_pushnumber( state, isit?1.0:0.0 );
	return 1;
}

static int world_setgameover( lua_State* state )
{
	UNREF_P( state );

	ModObj::i()->GetWorld()->SetGameOver();
	return 0;
}

static int world_doentity( lua_State* state )
{
	unsigned long command = 0;
	unsigned long parm    = 0;
	unsigned long player  = 0;
	EntityGroup group;

	if( priv_getCommand      ( state, 1, command ) &&
		priv_getCommandParm  ( state, 2, parm	 ) &&
		priv_getCommandPlayer( state, 3, player  ) &&
		priv_getCommandGroup ( state, 4, group	 ) )
	{
		//
		dbTracef( "LUA -- DoEntity(cmd:%d, param:%d, playerid:%d)", command, parm, player );

		//
		WorldDoCommandEntity
			(
			ModObj::i()->GetWorld(),
			command,
			parm,
			0,
			player,
			group
			);
	}
	else
	{
		lua_print("world_doentity( command, param, playerid, entity group ): invalid args");
	}

	return 0;
}

static int world_doentitypoint( lua_State* state )
{
	unsigned long command = 0;
	unsigned long parm    = 0;
	unsigned long player  = 0;
	EntityGroup group;
	Vec3f pos;

	if( priv_getCommand      ( state, 1, command ) &&
		priv_getCommandParm  ( state, 2, parm	 ) &&
		priv_getCommandPlayer( state, 3, player  ) &&
		priv_getCommandGroup ( state, 4, group	 ) &&
		priv_getCommandPoint ( state, 5, 6, 7, pos ) )
	{
		//
		dbTracef( "LUA -- DoEntityPoint(cmd:%d, param:%d, playerid:%d)", command, parm, player );

		//
		WorldDoCommandEntityPoint
			(
			ModObj::i()->GetWorld(),
			command,
			parm,
			0,
			player,
			group,
			&pos,
			1
			);
	}
	else
	{
		lua_print("world_doentitypoint( command, param, playerid, entity group, x, y ,z ): invalid args");
	}

	return 0;
}

static int world_doentityentity( lua_State* state )
{
	unsigned long command = 0;
	unsigned long parm    = 0;
	unsigned long player  = 0;
	EntityGroup group;
	EntityGroup target;

	if( priv_getCommand      ( state, 1, command ) &&
		priv_getCommandParm  ( state, 2, parm	 ) &&
		priv_getCommandPlayer( state, 3, player  ) &&
		priv_getCommandGroup ( state, 4, group	 ) &&
		priv_getCommandGroup ( state, 5, target  ) )
	{
		//
		dbTracef( "LUA -- DoEntityEntity(cmd:%d, param:%d, playerid:%d)", command, parm, player );

		//
		WorldDoCommandEntityEntity
			(
			ModObj::i()->GetWorld(),
			command,
			parm,
			0,
			player,
			group,
			target
			);
	}
	else
	{
		lua_print("world_doentityentity( command, param, playerid, entity group, entity group ): invalid args");
	}
	return 0;
}

static int world_doplayerplayer( lua_State* state )
{
	unsigned long command = 0;
	unsigned long parm    = 0;
	unsigned long player0 = 0;
	unsigned long player1 = 0;
	
	if( priv_getCommand      ( state, 1, command ) &&
		priv_getCommandParm  ( state, 2, parm	 ) &&
		priv_getCommandPlayer( state, 3, player0 ) &&
		priv_getCommandPlayer( state, 4, player1 ) )
	{
		//
		dbTracef( "LUA -- DoPlayerPlayer(cmd:%d, param:%d, playerid0:%d playerid1:%d)", command, parm, player0, player1 );

		//
		ModObj::i()->GetWorld()->DoCommandPlayerPlayer
			(
			command,
			parm,
			0,
			player0,
			player1
			);
	}
	else
	{
		lua_print("world_doplayerplayer( command, param, player id, player id ): invalid args");
	}
	return 0;
}

static int world_doplayerentity( lua_State* state )
{
	if (lua_type( state, 1 ) == LUA_TNUMBER && 
		lua_type( state, 2 ) == LUA_TNUMBER && 
		lua_type( state, 3 ) == LUA_TNUMBER && 
		lua_type( state, 4 ) == LUA_TNUMBER &&
		lua_type( state, 5 ) == LUA_TUSERDATA &&
		lua_type( state, 6 ) == LUA_TNUMBER &&
		lua_type( state, 7 ) == LUA_TNUMBER &&
		lua_type( state, 8 ) == LUA_TNUMBER
		)
	{

		int egroupTag = LuaConfig::GetTag( state, "entitygroup" );
		if (lua_tag(state, 5) == egroupTag)
		{
			EntityGroup* egroup = (EntityGroup*)lua_touserdata( state, 5 );
						
			if (egroup && egroup->size())
			{
				const int command   = (int)lua_tonumber(state, 1);		
				const int param     = (int)lua_tonumber(state, 2);
				const int playerid0 = (int)lua_tonumber(state, 3);
				const int playerid1 = (int)lua_tonumber(state, 4);

				//
				Vec3f pos;
				pos.x = (float)lua_tonumber(state, 6);
				pos.y = (float)lua_tonumber(state, 7);
				pos.z = (float)lua_tonumber(state, 8);

				//
				std::vector< unsigned long > v;

				EntityGroup::const_iterator gi = egroup->begin();
				EntityGroup::const_iterator ge = egroup->end();

				for( ; gi != ge; ++gi )
				{
					v.push_back( (*gi)->GetID() );
				}
				
				//
				dbTracef( "LUA -- DoPlayerEntity(cmd:%d, param:%d, playerid0:%d playerid1:%d)", command, param, playerid0, playerid1);

				//
				ModObj::i()->GetWorld()->DoCommandPlayerEntity
					(
					command, 
					param,
					0,
					playerid0,
					playerid1,
					&v[0],
					v.size(),
					&pos,
					1
					);
			}
		}
	}
	else
	{
		lua_print("world_doplayerentity( command, param, player id, player id, entity group, x, y, z ): invalid args");
	}
	return 0;
}



///////////////////////////////////////////////////////////////////// 
// 

namespace
{
	struct LuaFunction
	{
		LuaConfig::LuaFunc	funcPtr;
		const char*			funcName;
	};

#define LUAFUNC(n) { &n, #n }

	static const LuaFunction s_exported[] = 
	{
		LUAFUNC( world_gettime			),
		LUAFUNC( world_getticks			),
		LUAFUNC( world_getnumentities	),
		LUAFUNC( world_getplayercount	),
		LUAFUNC( world_getplayerat		),
		LUAFUNC( world_getrand			),
		LUAFUNC( world_isgameover		),
		LUAFUNC( world_setgameover		),
		LUAFUNC( world_doentity			),
		LUAFUNC( world_doentitypoint	),
		LUAFUNC( world_doentityentity	),
		LUAFUNC( world_doplayerplayer	),
		LUAFUNC( world_doplayerentity	),
	};

#undef LUAFUNC

}

/* Open function */
void LuaRDNWorldLib::Initialize( LuaConfig* lc )
{
	// register any functions
	size_t i = 0;
	size_t e = sizeof( s_exported ) / sizeof( s_exported[ 0 ] );

	for( ; i != e; ++i )
	{
		lc->RegisterCFunc( s_exported[ i ].funcName, s_exported[ i ].funcPtr );
	}

#define BINDCONSTANT(c)\
	lc->SetNumber( #c, double( c ) )

	BINDCONSTANT( CMD_DefaultAction		 );
	BINDCONSTANT( CMD_Stop				 );
	BINDCONSTANT( CMD_Destroy			 );
	BINDCONSTANT( CMD_BuildUnit			 );
	BINDCONSTANT( CMD_CancelBuildUnit	 );
	BINDCONSTANT( CMD_Move				 );
	BINDCONSTANT( CMD_Attack			 );
	BINDCONSTANT( CMD_AttackMove		 );

	BINDCONSTANT( CMD_RallyPoint		 );

#undef BINDCONSTANT

	return;
}

/* Close function */
void LuaRDNWorldLib::Shutdown( LuaConfig* lc )
{
	// register any functions
	size_t i = 0;
	size_t e = sizeof( s_exported ) / sizeof( s_exported[ 0 ] );

	for( ; i != e; ++i )
	{
		lc->ClearFunction( s_exported[ i ].funcName );
	}

	return;
}
