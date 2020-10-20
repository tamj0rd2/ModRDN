/////////////////////////////////////////////////////////////////////
// File    : RDNDllSetup.h
// Desc    :
// Created : Wednesday, October 10, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/DllInterface.h>

// forward declarations
class RDNWorld;

/////////////////////////////////////////////////////////////////////
// RDNDllSetup

class RDNDllSetup : private DLLSetupInterface
{
	// types
public:
	enum OptionTypes
	{
		OPT_GameMode,
		OPT_UnitCap,
		OPT_Cheats,

		OPT_COUNT
	};

	// GameMode
	enum GameMode
	{
		GM_Fight,
		GM_Love,

		GM_COUNT
	};

	// UnitCap
	enum UnitCap
	{
		UC_Standard,
		UC_Large,

		UC_COUNT
	};

	// Cheats
	enum Cheats
	{
		CHT_Yes,
		CHT_No,

		CHT_COUNT
	};

	// construction -- singleton
private:
	RDNDllSetup();

public:
	~RDNDllSetup();

	static RDNDllSetup *Instance();
	static void Initialize();
	static void Shutdown();

	// interface
public:
	DLLSetupInterface *
	Get();
	void Release(DLLSetupInterface *p);

public:
	void Reset();

	unsigned long MapPlayerToSimulation(const RDNWorld *w, size_t playerIndex) const;

	unsigned long GetRandomSeed() const;

public:
	// -- accessor functions

	DLLSetupInterface::GameType
	GetGameType() const;

	size_t PlayerGetTeam(unsigned long idSim) const;
	bool PlayerIsHuman(unsigned long idSim) const;
	const wchar_t *PlayerName(unsigned long idSim) const;
	size_t PlayerRace(unsigned long idSim) const;

	// may be empty
	const wchar_t *PlayerPassport(size_t n) const;
	const wchar_t *PlayerPassportFromId(unsigned long id, const RDNWorld *w) const;

	const wchar_t *GetScenarioName() const;
	const char *GetScenarioFile() const;

	const GUID &GetGameID() const;

	// query for scenario compatibility
	bool IsScenarioCompatible(const char *modname) const;

	// -- option accessor functions --

	bool GetNetworkGame() const;
	const char *GetGameModeScript() const;
	long GetUnitCap() const;
	bool GetCheats() const;

	size_t GetHumanPlayerCount() const;

	// inherited -- DLLSetupInterface
public:
	virtual size_t PlayerGetCount() const;

private:
	virtual void SetGameType(DLLSetupInterface::GameType gt);
	virtual void SetNetworkGame(bool bNetwork);

	virtual void SetRandomSeed(unsigned long seed);

	virtual void SetStatsScenario(const char *scenfile, const wchar_t *scenname);

	virtual void SetStatsGameID(const GUID &id);

	virtual unsigned long
	GetGameModeOptionID() const;

	virtual void PlayerSetCount(size_t n);
	virtual void PlayerSetMax(size_t n);
	virtual size_t PlayerGetMax() const;

	virtual void PlayerSetToAI(size_t playerIndex);
	virtual void PlayerSetToHuman(size_t playerIndex);
	virtual void PlayerSetName(size_t playerIndex, const wchar_t *name);
	virtual void PlayerSetPassport(size_t playerIndex, const wchar_t *passport);
	virtual void PlayerSetRace(size_t playerIndex, size_t race);
	virtual void PlayerSetTeam(size_t playerIndex, size_t team);

	virtual void OptionsInit(GameType gt, bool bNetwork);
	virtual size_t OptionsCount() const;

	virtual unsigned long
	OptionID(size_t optionIndex) const;
	virtual void OptionName(wchar_t *out, size_t outlen, unsigned long optionID) const;
	virtual void OptionTooltip(wchar_t *out, size_t outlen, unsigned long optionID) const;
	virtual size_t OptionChoices(unsigned long optionID) const;
	virtual size_t OptionChoiceDef(unsigned long optionID) const;
	virtual bool OptionChoiceValid(unsigned long optionID, size_t choice) const;
	virtual void OptionChoiceName(wchar_t *out, size_t outlen, unsigned long optionID, size_t choice) const;
	virtual bool OptionChoiceAISupport(unsigned long optionID, size_t choice) const;
	virtual void OptionSet(unsigned long optionID, size_t choice);

	virtual size_t RaceGetCount() const;
	virtual const wchar_t *
	RaceGetName(size_t index) const;
	virtual const wchar_t *
	RaceGetDesc(size_t index) const;
	virtual const char *RaceGetImageFilename(size_t index) const;

	virtual bool TeamGetEnabled() const;

	// fields
private:
	class Data;
	Data *m_pimpl;

	// copy -- do not define
private:
	RDNDllSetup(const RDNDllSetup &);
	RDNDllSetup &operator=(const RDNDllSetup &);
};
