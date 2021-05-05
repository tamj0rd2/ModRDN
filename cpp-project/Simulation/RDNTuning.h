/////////////////////////////////////////////////////////////////////
// File    : RDNTuning.h
// Desc    :
// Created : Wednesday, June 20, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

// * read-only global variables for the MOD

// * IMPORTANT NOTE: all the structures in RDNTuning **MUST** be pods.

#pragma once

// Are these defined elsewhere, or just explicitly assumed?
#define ST_NUM_ANIMAL_SIZES 10
#define ST_NUM_ANIMAL_RANKS 5

// forward declarations
class LuaConfig;

/////////////////////////////////////////////////////////////////////
// RDNTuning

class RDNTuning
{
	// construction
private:
	RDNTuning();
	~RDNTuning();

public:
	static RDNTuning *Instance();

	static bool Initialize();
	static void Shutdown();

	// interface
public:
	/////////////////////////////////////////////////////////////////////
	// Player
	struct PlayerInfo
	{
		float startingGatherRes;
		float startingRenewRes;

		float quickstartGatherRes;
		float quickstartRenewRes;

		float resourceModLow;
		float resourceModHigh;

		float donationPenaltyPercentage;
	};

	/////////////////////////////////////////////////////////////////////
	// Lab
	struct LabInfo
	{
		float healthMax;
	};

	/////////////////////////////////////////////////////////////////////
	// Effect Information
	struct EffectInfo
	{
		struct Effect
		{
			char fx[64];
			char location[64];
			long count;
		};

		Effect impact;
	};

	/////////////////////////////////////////////////////////////////////
	// FogOfWar Information
	struct FogOfWarInfo
	{
		float attackerRevealTime;
	};

	//
public:
	unsigned long GetSyncToken() const;

	const PlayerInfo GetPlayerInfo() const { return m_PlayerInfo; }
	const LabInfo GetLabInfo() const { return m_LabInfo; }
	const EffectInfo GetEffectInfo() const { return m_EffectInfo; }
	const FogOfWarInfo GetFogOfWarInfo() const { return m_FogOfWarInfo; }

private:
	PlayerInfo m_PlayerInfo;
	LabInfo m_LabInfo;
	EffectInfo m_EffectInfo;
	FogOfWarInfo m_FogOfWarInfo;

	// implementation
private:
	void LoadFrom(const char *);

	// copy -- do not define
private:
	RDNTuning(const RDNTuning &);
	RDNTuning &operator=(const RDNTuning &);
};
