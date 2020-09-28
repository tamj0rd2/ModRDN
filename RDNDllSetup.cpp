/////////////////////////////////////////////////////////////////////
// File    : RDNDllSetup.cpp
// Desc    :
// Created : Wednesday, October 10, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNDllSetup.h"

#include "Simulation/RDNPlayer.h"
#include "Simulation/RDNWorld.h"

#include <Util/Random.h>

#include <Platform/Platform.h>

/////////////////////////////////////////////////////////////////////
//

namespace
{
	const size_t MAXPLAYERS = 6;

	struct SetupPlayer
	{
		bool human;
		std::wstring name;
		std::wstring passport;
		size_t race;
		size_t team;
	};

	struct SetupRace
	{
		std::wstring name;
		std::wstring desc;
		std::string imagename;
	};

	typedef std::vector<SetupPlayer> SetupPlayerArray;

	struct Options
	{
		unsigned long id;		// option identifier
		int name;						// name of this option (localized text lookup id)
		int tooltip;				// tooltip  of this option (localized text lookup id)
		int def;						// default choice
		int count;					// number of choices in this option
		int numeric;				// option choices are numbers instead of text strings
		const int *choices; // list of options (type based on numeric flag)
	};

	struct InvalidOptionChoices
	{
		size_t invalidChoiceCount; // number of invalid choices
		const int *invalidChoices; // list of invalid choices
	};

	const int OPTIONS_GAMETYPE[] =
			{
					40580, // destroy lab
	};

	const int OPTIONS_UNITCAP[] =
			{
					50, // standard
					75, // large
	};

	const int OPTIONS_CHEATS[] =
			{
					40590, // yes
					40591, // no
	};

	const Options OPTIONS[] =
			{
					{'TYPE', 40500, 53250, RDNDllSetup::GM_Fight, LENGTHOF(OPTIONS_GAMETYPE), 0, &OPTIONS_GAMETYPE[0]},	 // game type
					{'UCAP', 40504, 53254, RDNDllSetup::UC_Standard, LENGTHOF(OPTIONS_UNITCAP), 1, &OPTIONS_UNITCAP[0]}, // unit cap
					{'CHEA', 40507, 53257, RDNDllSetup::CHT_No, LENGTHOF(OPTIONS_CHEATS), 0, &OPTIONS_CHEATS[0]},				 // cheats
	};

	const InvalidOptionChoices INVALIDCHOICES_SKIRMISH[] =
			{
					{0, NULL},
					{0, NULL},
					{0, NULL},
	};
} // namespace

/////////////////////////////////////////////////////////////////////
//

namespace
{
	class ShuffleRandomGen
	{
	private:
		Random m_r;

	public:
		ShuffleRandomGen(unsigned long seed)
		{
			m_r.SetSeed(seed);
		}

	public:
		int operator()(int n)
		{
			return m_r.GetMax(n);
		}
	};
} // namespace

/////////////////////////////////////////////////////////////////////
//

static void AssignPlayers(
		const SetupPlayerArray &setup,
		const DLLSetupInterface::GameType gt,
		const unsigned long random,
		const RDNWorld *world,
		std::smallvector<unsigned long, MAXPLAYERS> &players)
{
	// init out parm
	players.assign(setup.size(), 0);

	// special case for SP games
	if (gt == DLLSetupInterface::GT_SP)
	{
		// assign players in order
		size_t i = 0;
		size_t e = setup.size();

		for (; i != e; ++i)
		{
			players[i] = world->GetPlayerAt(i)->GetID();
		}
	}
	else
	{
		// list all players on the map with lab
		std::smallvector<unsigned long, MAXPLAYERS> HQs;

		size_t i = 0;
		size_t e = world->GetPlayerCount();

		for (; i != e; ++i)
		{
			const RDNPlayer *p = static_cast<const RDNPlayer *>(world->GetPlayerAt(i));

			if (p->GetHQEntity())
			{
				HQs.push_back(p->GetID());
			}
		}

		// shuffle list (defaults to random)
		ShuffleRandomGen rg(random);
		std::random_shuffle(HQs.begin(), HQs.end(), rg);

		// match each player
		size_t mi = 0;
		size_t me = players.size();

		for (; mi != me; ++mi)
		{
			// validate
			if (HQs.empty())
			{
				// oops! invalid map
				dbTracef("MOD -- Not enough labs on this map");
				break;
			}

			// take the first one
			players[mi] = HQs.front();
			HQs.erase(HQs.begin());
		}
	}

	return;
}

static bool FindOption(unsigned long id, size_t &option)
{
	size_t numOptions = LENGTHOF(OPTIONS);

	for (size_t i = 0; i < numOptions; i++)
	{
		if (OPTIONS[i].id == id)
		{
			option = i;
			return true;
		}
	}

	dbPrintf("Warning: option ID not found (%c%c%c%c)",
					 (char)((id >> 24) & 0xff),
					 (char)((id >> 16) & 0xff),
					 (char)((id >> 8) & 0xff),
					 (char)(id & 0xff));

	return false;
}

/////////////////////////////////////////////////////////////////////
// RDNDllSetup

static RDNDllSetup *s_instance = 0;

class RDNDllSetup::Data
{
public:
	DLLSetupInterface::GameType
			m_gt;

	bool m_bNetworkGame;

	SetupPlayerArray m_players;

	std::vector<SetupRace>
			m_race;

	unsigned long m_randomSeed;

	std::smallvector<unsigned long, MAXPLAYERS>
			m_playerMap;
	size_t m_playerMax; // maximum number of players in this game

	// stats stuff
	std::wstring m_statsScenName;
	std::string m_statsScenFile;

	GUID m_statsGameId;

	// options data
	long m_opt_unitCap;
	bool m_opt_cheats;

	const InvalidOptionChoices *
			m_opt_invalidChoices;
};

RDNDllSetup::RDNDllSetup()
		: m_pimpl(new Data)
{
	ctAssert(LENGTHOF(OPTIONS) == OPT_COUNT);
	ctAssert(LENGTHOF(INVALIDCHOICES_SKIRMISH) == OPT_COUNT);

	ctAssert(RDNDllSetup::UC_COUNT == LENGTHOF(OPTIONS_UNITCAP));

	Reset();

	return;
}

RDNDllSetup::~RDNDllSetup()
{
	DELETEZERO(m_pimpl);
}

RDNDllSetup *RDNDllSetup::Instance()
{
	return s_instance;
}

void RDNDllSetup::Initialize()
{
	dbAssert(s_instance == 0);

	s_instance = new RDNDllSetup;

	return;
}

void RDNDllSetup::Shutdown()
{
	dbAssert(s_instance != 0);

	DELETEZERO(s_instance);

	return;
}

DLLSetupInterface *RDNDllSetup::Get()
{
	return this;
}

void RDNDllSetup::Release(DLLSetupInterface *p)
{
	dbAssert(p == this);
}

void RDNDllSetup::Reset()
{
	// reset all member vars to their default values
	m_pimpl->m_gt = DLLSetupInterface::GT_MP;
	m_pimpl->m_players.clear();
	m_pimpl->m_randomSeed = 0L;
	m_pimpl->m_playerMap.clear();
	m_pimpl->m_bNetworkGame = false;
	m_pimpl->m_playerMax = 0;

	OptionsInit(m_pimpl->m_gt, m_pimpl->m_bNetworkGame);

	m_pimpl->m_opt_unitCap = 100;
	m_pimpl->m_opt_cheats = false;

	m_pimpl->m_statsScenName.clear();
	m_pimpl->m_statsScenFile.clear();

	m_pimpl->m_statsGameId = GUIDNil();

	m_pimpl->m_race.resize(RACE_COUNT);
	m_pimpl->m_race[RACE_Faster].name = L"Faster";
	m_pimpl->m_race[RACE_Faster].desc = L"They are faster.";
	m_pimpl->m_race[RACE_Faster].imagename = "data:RDN/faster_icon.tga";
	m_pimpl->m_race[RACE_Stronger].name = L"Stronger";
	m_pimpl->m_race[RACE_Stronger].desc = L"They are stronger.";
	m_pimpl->m_race[RACE_Stronger].imagename = "data:RDN/stronger_icon.tga";
	m_pimpl->m_race[RACE_Cheaper].name = L"Cheaper";
	m_pimpl->m_race[RACE_Cheaper].desc = L"They are cheaper.";
	m_pimpl->m_race[RACE_Cheaper].imagename = "data:RDN/cheaper_icon.tga";

	return;
}

unsigned long RDNDllSetup::MapPlayerToSimulation(const RDNWorld *w, size_t playerIndex) const
{
	// find player
	if (playerIndex >= m_pimpl->m_players.size())
	{
		dbBreak();
		return 0;
	}

	if (m_pimpl->m_playerMap.empty())
	{
		AssignPlayers(
				m_pimpl->m_players,
				m_pimpl->m_gt,
				m_pimpl->m_randomSeed,
				w,
				m_pimpl->m_playerMap);
	}

	//
	return m_pimpl->m_playerMap[playerIndex];
}

void RDNDllSetup::SetGameType(DLLSetupInterface::GameType gt)
{
	// validate parm
	dbAssert(
			gt == DLLSetupInterface::GT_SP ||
			gt == DLLSetupInterface::GT_MP);

	m_pimpl->m_gt = gt;

	// update the options available
	OptionsInit(gt, GetNetworkGame());

	return;
}

void RDNDllSetup::SetNetworkGame(bool bNetwork)
{
	m_pimpl->m_bNetworkGame = bNetwork;

	// update the options available
	OptionsInit(GetGameType(), bNetwork);
}

void RDNDllSetup::SetRandomSeed(unsigned long seed)
{
	m_pimpl->m_randomSeed = seed;

	return;
}

unsigned long RDNDllSetup::GetRandomSeed() const
{
	return m_pimpl->m_randomSeed;
}

void RDNDllSetup::PlayerSetCount(size_t n)
{
	// validate parm
	dbAssert(n > 0 && n <= MAXPLAYERS);

	//
	m_pimpl->m_players.resize(n);

	size_t i = 0;
	size_t e = m_pimpl->m_players.size();

	for (; i != e; ++i)
	{
		m_pimpl->m_players[i].human = true;
		m_pimpl->m_players[i].team = i;
	}

	return;
}

void RDNDllSetup::PlayerSetMax(size_t n)
{
	// validate parm
	dbAssert(n > 0 && n <= MAXPLAYERS);

	//
	m_pimpl->m_playerMax = n;
}

size_t RDNDllSetup::PlayerGetMax() const
{
	return m_pimpl->m_playerMax;
}

void RDNDllSetup::PlayerSetToAI(size_t n)
{
	// validate parm
	dbAssert(n < PlayerGetCount());

	//
	m_pimpl->m_players[n].human = false;
	m_pimpl->m_players[n].name = L"";
	m_pimpl->m_players[n].passport = L"";

	return;
}

void RDNDllSetup::PlayerSetToHuman(size_t n)
{
	// validate parm
	dbAssert(n < PlayerGetCount());

	//
	m_pimpl->m_players[n].human = true;

	return;
}

void RDNDllSetup::PlayerSetName(size_t n, const wchar_t *name)
{
	// validate parm
	dbAssert(n < PlayerGetCount());

	//
	m_pimpl->m_players[n].name = name;

	return;
}

void RDNDllSetup::PlayerSetPassport(size_t n, const wchar_t *passport)
{
	// validate parm
	dbAssert(n < PlayerGetCount());

	//
	m_pimpl->m_players[n].passport = passport;

	return;
}

void RDNDllSetup::PlayerSetRace(size_t n, size_t race)
{
	m_pimpl->m_players[n].race = race;

	return;
}

void RDNDllSetup::PlayerSetTeam(size_t n, size_t t)
{
	// validate parm
	dbAssert(n < PlayerGetCount());
	dbAssert(n < MAXPLAYERS);

	//
	m_pimpl->m_players[n].team = t;

	return;
}

// query for scenario compatibility
bool RDNDllSetup::IsScenarioCompatible(const char *modname) const
{
	bool isCompatible = (strcmp("RDNMod", modname) == 0);

	return isCompatible;
}

// setup the invalid option choices
void RDNDllSetup::OptionsInit(GameType gt, bool bNetwork)
{
	m_pimpl->m_opt_invalidChoices = NULL;

	// do game type specific overrides
	if (gt == DLLSetupInterface::GT_MP && !bNetwork)
	{
		// Skirmish overrides
		m_pimpl->m_opt_invalidChoices = &INVALIDCHOICES_SKIRMISH[0];
	}
}

size_t RDNDllSetup::OptionsCount() const
{
	return LENGTHOF(OPTIONS);
}

unsigned long RDNDllSetup::OptionID(size_t optionIndex) const
{
	return OPTIONS[optionIndex].id;
}

void RDNDllSetup::OptionName(wchar_t *out, size_t outlen, unsigned long optionID) const
{
	// validate parm
	if (out == 0 || outlen == 0)
	{
		dbBreak();
		return;
	}

	// init out parm
	out[0] = L'\0';

	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return;
	}

	// validate option
	if (option >= OptionsCount())
	{
		dbBreak();
		return;
	}

	//
	Localizer::GetString(out, outlen, OPTIONS[option].name);

	return;
}

void RDNDllSetup::OptionTooltip(wchar_t *out, size_t outlen, unsigned long optionID) const
{
	// validate parm
	if (out == 0 || outlen == 0)
	{
		dbBreak();
		return;
	}

	// init out parm
	out[0] = L'\0';

	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return;
	}

	// validate option
	if (option >= OptionsCount())
	{
		dbBreak();
		return;
	}

	//
	Localizer::GetString(out, outlen, OPTIONS[option].tooltip);

	return;
}

size_t RDNDllSetup::OptionChoices(unsigned long optionID) const
{
	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return 0;
	}

	// validate option
	if (option >= OptionsCount())
	{
		dbBreak();
		return 0;
	}

	//
	return OPTIONS[option].count;
}

size_t RDNDllSetup::OptionChoiceDef(unsigned long optionID) const
{
	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return 0;
	}

	// validate option
	if (option >= OptionsCount())
	{
		dbBreak();
		return 0;
	}

	//
	return OPTIONS[option].def;
}

bool RDNDllSetup::OptionChoiceValid(unsigned long optionID, size_t choice) const
{
	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return 0;
	}

	// validate option
	if (option >= OptionsCount() || choice >= OptionChoices(optionID))
	{
		dbBreak();
		return 0;
	}

	if (m_pimpl->m_opt_invalidChoices)
	{
		// check to see if the choice is one of the invalid ones
		for (size_t i = 0; i < m_pimpl->m_opt_invalidChoices[option].invalidChoiceCount; i++)
		{
			if (choice == (size_t)(m_pimpl->m_opt_invalidChoices[option].invalidChoices[i]))
			{
				return false;
			}
		}
	}

	return true;
}

void RDNDllSetup::OptionChoiceName(wchar_t *out, size_t outlen, unsigned long optionID, size_t choice) const
{
	// validate parm
	if (out == 0 || outlen == 0)
	{
		dbBreak();
		return;
	}

	// init out parm
	out[0] = L'\0';

	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return;
	}

	// validate option & choice
	if (option >= OptionsCount() || choice >= OptionChoices(optionID))
	{
		dbBreak();
		return;
	}

	// numeric case (options are numbers not text strings)
	if (OPTIONS[option].numeric)
	{
		Localizer::ConvertNumber2Localized(out, outlen, OPTIONS[option].choices[choice]);
	}
	// normal case
	else
	{
		Localizer::GetString(out, outlen, OPTIONS[option].choices[choice]);
	}

	return;
}

bool RDNDllSetup::OptionChoiceAISupport(unsigned long optionID, size_t choice) const
{
	UNREF_P(optionID);
	UNREF_P(choice);

	//	No AI yet
	return false;
}

void RDNDllSetup::OptionSet(unsigned long optionID, size_t val)
{
	// find option index
	size_t option;
	if (!FindOption(optionID, option))
	{
		return;
	}

	// validate option & choice
	if (option >= OptionsCount() || val >= OptionChoices(optionID))
	{
		dbBreak();
		return;
	}

	// Handle each option

	switch (option)
	{
	// -- Unit Cap
	case OPT_UnitCap:
	{
		// validate
		dbAssert(val >= 0 && val < UC_COUNT);
		// store
		m_pimpl->m_opt_unitCap = OPTIONS[OPT_UnitCap].choices[val];
	}
	break;

	// -- Cheats
	case OPT_Cheats:
	{
		// validate
		dbAssert(val >= 0 && val < CHT_COUNT);
		// store
		m_pimpl->m_opt_cheats = (OPTIONS[OPT_UnitCap].choices[val] == CHT_Yes);
	}
	break;

	default:
		dbTracef("Unhandled MOD option (%d)", option);
		break;
	}

	return;
}

size_t RDNDllSetup::RaceGetCount() const
{
	return RACE_COUNT;
}

const wchar_t *RDNDllSetup::RaceGetName(size_t index) const
{
	dbAssert(index < m_pimpl->m_race.size());

	return m_pimpl->m_race[index].name.c_str();
}

const wchar_t *RDNDllSetup::RaceGetDesc(size_t index) const
{
	dbAssert(index < m_pimpl->m_race.size());

	return m_pimpl->m_race[index].desc.c_str();
}

const char *RDNDllSetup::RaceGetImageFilename(size_t index) const
{
	dbAssert(index < m_pimpl->m_race.size());

	return m_pimpl->m_race[index].imagename.c_str();
}

bool RDNDllSetup::TeamGetEnabled() const
{
	return false; //	We don't allow teams
}

unsigned long RDNDllSetup::GetGameModeOptionID() const
{
	return OPTIONS[OPT_GameMode].id;
}

DLLSetupInterface::GameType RDNDllSetup::GetGameType() const
{
	return m_pimpl->m_gt;
}

size_t RDNDllSetup::PlayerGetCount() const
{
	return m_pimpl->m_players.size();
}

size_t RDNDllSetup::PlayerGetTeam(unsigned long idSim) const
{
	// validate object state
	dbAssert(!m_pimpl->m_playerMap.empty());

	// find player index
	size_t i = 0;
	size_t e = m_pimpl->m_playerMap.size();

	for (; i != e; ++i)
	{
		if (m_pimpl->m_playerMap[i] == idSim)
			break;
	}

	dbAssert(i != e);

	//
	return m_pimpl->m_players[i].team;
}

bool RDNDllSetup::PlayerIsHuman(unsigned long idSim) const
{
	// validate object state
	dbAssert(!m_pimpl->m_playerMap.empty());

	// find player index
	size_t i = 0;
	size_t e = m_pimpl->m_playerMap.size();

	for (; i != e; ++i)
	{
		if (m_pimpl->m_playerMap[i] == idSim)
			break;
	}

	dbAssert(i != e);

	//
	return m_pimpl->m_players[i].human;
}

const wchar_t *RDNDllSetup::PlayerName(unsigned long idSim) const
{
	// validate object state
	dbAssert(!m_pimpl->m_playerMap.empty());

	// find player index
	size_t i = 0;
	size_t e = m_pimpl->m_playerMap.size();

	for (; i != e; ++i)
	{
		if (m_pimpl->m_playerMap[i] == idSim)
			break;
	}

	dbAssert(i != e);

	//
	return m_pimpl->m_players[i].name.c_str();
}

size_t RDNDllSetup::PlayerRace(unsigned long idSim) const
{
	// validate object state
	dbAssert(!m_pimpl->m_playerMap.empty());

	// find player index
	size_t i = 0;
	size_t e = m_pimpl->m_playerMap.size();

	for (; i != e; ++i)
	{
		if (m_pimpl->m_playerMap[i] == idSim)
			break;
	}
	dbAssert(i != e);

	//
	return m_pimpl->m_players[i].race;
}

bool RDNDllSetup::GetNetworkGame() const
{
	return m_pimpl->m_bNetworkGame;
}

const char *RDNDllSetup::GetGameModeScript() const
{
	return "";
}

long RDNDllSetup::GetUnitCap() const
{
	return m_pimpl->m_opt_unitCap;
}

bool RDNDllSetup::GetCheats() const
{
	return m_pimpl->m_opt_cheats;
}

const wchar_t *RDNDllSetup::PlayerPassport(size_t n) const
{
	dbAssert(n < PlayerGetCount());

	return m_pimpl->m_players[n].passport.c_str();
}

const wchar_t *RDNDllSetup::PlayerPassportFromId(unsigned long id, const RDNWorld *w) const
{
	// find player index
	size_t i = 0;
	size_t e = PlayerGetCount();

	for (; i != e; ++i)
	{
		if (MapPlayerToSimulation(w, i) == id)
			break;
	}

	if (i == e)
	{
		// wtf??
		dbBreak();
		return 0;
	}

	//
	return PlayerPassport(i);
}

size_t RDNDllSetup::GetHumanPlayerCount() const
{
	// find player index
	size_t humans = 0;

	size_t i = 0;
	size_t e = m_pimpl->m_players.size();

	for (; i != e; ++i)
	{
		if (m_pimpl->m_players[i].human)
			++humans;
	}

	return humans;
}

void RDNDllSetup::SetStatsScenario(const char *scenfile, const wchar_t *scenname)
{
	m_pimpl->m_statsScenName = scenname;
	m_pimpl->m_statsScenFile = scenfile;
}

void RDNDllSetup::SetStatsGameID(const GUID &id)
{
	m_pimpl->m_statsGameId = id;
}

const wchar_t *RDNDllSetup::GetScenarioName() const
{
	return m_pimpl->m_statsScenName.c_str();
}

const char *RDNDllSetup::GetScenarioFile() const
{
	return m_pimpl->m_statsScenFile.c_str();
}

const GUID &RDNDllSetup::GetGameID() const
{
	return m_pimpl->m_statsGameId;
}
