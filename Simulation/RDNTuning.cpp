/////////////////////////////////////////////////////////////////////
// File    : RDNTuning.cpp
// Desc    :
// Created : Wednesday, June 20, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNTuning.h"

#include "RDNWorld.h"

#include "UnitConversion.h"

#include <Filesystem/CRC.h>
#include <Filesystem/FilePath.h>

#include <Lua/LuaUtils.h>

#define ST_MAX_STR_CHARS 256

/////////////////////////////////////////////////////////////////////
//

static void LoadInfo(RDNTuning::PlayerInfo &inf, LuaConfig &lc)
{
	lc.PushTableEx("Player");

	LCGetValW(lc, "tuning: player,", 'RDNM', "startingcash", 1.0f, inf.startingCash);

	lc.PopTable();
}

/////////////////////////////////////////////////////////////////////
//

static void LoadRace(const char *name, unsigned long id, RDNTuning::RaceInfo::Race &race, LuaConfig &lc)
{
	char field[64];

	strcpy(field, name);
	strcat(field, "_healthMultiplier");
	LCGetValW(lc, "tuning : health,", id, field, 100.f, race.healthMultiplier);

	strcpy(field, name);
	strcat(field, "_costMultiplier");
	LCGetValW(lc, "tuning : cost,", id, field, 100.f, race.costMultiplier);

	strcpy(field, name);
	strcat(field, "_speedMultiplier");
	LCGetValW(lc, "tuning : speed,", id, field, 100.f, race.speedMultiplier);
}

static void LoadInfo(RDNTuning::RaceInfo &inf, LuaConfig &lc)
{
	lc.PushTableEx("Race");

	LoadRace("stronger", 'RDNM', inf.stronger, lc);
	LoadRace("cheaper", 'RDNM', inf.cheaper, lc);
	LoadRace("faster", 'RDNM', inf.faster, lc);

	lc.PopTable();
}

/////////////////////////////////////////////////////////////////////
//

static void LoadInfo(RDNTuning::HQInfo &inf, LuaConfig &lc)
{
	lc.PushTableEx("HQ");

	/** 
	 * LCGetValW - As far as I can tell, the interface for this is:
	 * lc
	 * <filename>: <podname>,
	 * RDNM
	 * <propertyname>
	 * default value
	 * value container
	 **/
	LCGetValW(lc, "tuning: hq,", 'RDNM', "health", 1.0f, inf.healthMax);

	lc.PopTable();
}

/////////////////////////////////////////////////////////////////////
//

static void LoadEffect(const char *name, unsigned long id, RDNTuning::EffectInfo::Effect &e, LuaConfig &lc)
{
	char field[64];

	strcpy(field, name);
	strcat(field, "_fx");
	memset(&e.fx, 0, 64);
	LCGetStringW(lc, "tuning : effect,", id, field, "NoEffect", e.fx, sizeof(e.fx));

	strcpy(field, name);
	strcat(field, "_location");
	memset(&e.location, 0, 64);
	LCGetStringW(lc, "tuning : effect,", id, field, "", e.location, sizeof(e.location));

	strcpy(field, name);
	strcat(field, "_count");
	LCGetValW(lc, "tuning : effect,", id, field, 0L, 4L, e.count);
}

static void LoadInfo(RDNTuning::EffectInfo &inf, LuaConfig &lc)
{
	lc.PushTableEx("Effect");

	LoadEffect("impact", 'RDNM', inf.impact, lc);

	lc.PopTable();
}

/////////////////////////////////////////////////////////////////////
//

static void LoadInfo(RDNTuning::FogOfWarInfo &inf, LuaConfig &lc)
{
	lc.PushTableEx("FogOfWar");

	LCGetValW(lc, "tuning: fogOfWar,", 'RDNM', "attackerRevealTime", 0.0f, 1000.0f, inf.attackerRevealTime);

	lc.PopTable();
}

/////////////////////////////////////////////////////////////////////
//

namespace
{
	RDNTuning *s_instance = NULL;
};

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNTuning::Initialize()
{
	dbAssert(s_instance == NULL);

	//
	s_instance = new RDNTuning;
	s_instance->LoadFrom("data:RDN/tuning.lua");

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTuning::Shutdown()
{
	dbAssert(s_instance);
	DELETEZERO(s_instance);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNTuning *RDNTuning::Instance()
{
	dbAssert(s_instance);
	return s_instance;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNTuning::RDNTuning()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNTuning::~RDNTuning()
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNTuning::LoadFrom(const char *file)
{
	LuaConfig lc;

	if (!lc.LoadFile(file))
	{
		dbFatalf("MOD -- Failed to load tuning file %s", file);
	}

#define LOADINFO(inf)               \
	memset(&m_##inf, 0, sizeof(inf)); \
	LoadInfo(m_##inf, lc);

	LOADINFO(PlayerInfo)
	LOADINFO(RaceInfo)
	LOADINFO(HQInfo)
	LOADINFO(EffectInfo)
	LOADINFO(FogOfWarInfo)

#undef LOADINFO

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long RDNTuning::GetSyncToken() const
{
	// calculate crc
	// NOTE: EXCLUDE ALL UI STRUCTURES
	CRC crc;

#define ADDTUNINGSTRUCT(name) \
	crc.AddValues(&s_instance->Get##name(), sizeof(name));

	ADDTUNINGSTRUCT(PlayerInfo)
	ADDTUNINGSTRUCT(RaceInfo)
	ADDTUNINGSTRUCT(HQInfo)
	//	ADDTUNINGSTRUCT( EffectInfo )
	ADDTUNINGSTRUCT(FogOfWarInfo)

#undef ADDTUNINGSTRUCT

	return crc.GetCRC();
}
