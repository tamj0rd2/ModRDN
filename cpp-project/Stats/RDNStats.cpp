/////////////////////////////////////////////////////////////////////
// File    : RDNStats.cpp
// Desc    :
// Created : Thursday, April 04, 2003
// Author  :
//
// (c) 2003 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNStats.h"

#include <Util/IFF.h>
#include <Assist/FixedString.h>

#include "../ModObj.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNPlayer.h"
#include "../Simulation/GameEventDefs.h"

/////////////////////////////////////////////////////////////////////
//

namespace
{
	enum StatPlayerState
	{
		SPS_Playing,
		SPS_Killed,
		SPS_Won
	};

	struct StatPlayer
	{
		unsigned long idplayer;
		StatPlayerState state;
		fwstring<31> name;
	};

	typedef std::vector<StatPlayer> StatPlayers;
} // namespace

static StatPlayer *GetPlayerByID(StatPlayers &players, unsigned long idplayer)
{
	unsigned long numPlayers = players.size();
	for (unsigned long index = 0; index < numPlayers; index++)
	{
		if (players[index].idplayer == idplayer)
		{
			// found it
			return &players[index];
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
//

class RDNStats::Data
{
public:
	StatPlayers m_players;
	long m_timeTotal;
};

/////////////////////////////////////////////////////////////////////
// RDNStats

RDNStats *s_instance = NULL;

RDNStats::RDNStats() : m_pimpl(new Data)
{
	m_pimpl->m_timeTotal = 0;
}

RDNStats::~RDNStats()
{
	DELETEZERO(m_pimpl);
}

RDNStats *RDNStats::Instance()
{
	dbAssert(s_instance != NULL);
	return s_instance;
}

void RDNStats::Initialize()
{
	dbAssert(s_instance == NULL);
	s_instance = new RDNStats();
}

void RDNStats::Shutdown()
{
	dbAssert(s_instance != NULL);
	DELETEZERO(s_instance);
}

void RDNStats::OnEvent(const GameEventSys::Event &event)
{
	dbTracef("RDNStats::OnEvent");

	switch (event.GetType())
	{
	case GE_PlayerKilled:
	{
		const GameEvent_PlayerKilled &eventPK =
				static_cast<const GameEvent_PlayerKilled &>(event);

		// get the player stats
		StatPlayer *pPlayer =
				GetPlayerByID(m_pimpl->m_players, eventPK.m_killed->GetID());
		if (pPlayer != NULL)
		{
			// set the player as killed
			pPlayer->state = SPS_Killed;
		}
	}
	break;
	}
}

void RDNStats::RecordStart(const RDNWorld *pWorld)
{
	// initialize starting state
	if (pWorld->GetGameTime() == 0.0f)
	{
		RecordInit(pWorld);
	}
	else
	{
		// make sure stats are up to date
		dbAssert(m_pimpl->m_timeTotal == static_cast<long>(pWorld->GetGameTime()));
	}

	// register self for game events
	GameEventSys::Instance()->RegisterClient(this);
}

void RDNStats::RecordFrame(const RDNWorld *pWorld)
{
	// update last recorded frame
	m_pimpl->m_timeTotal = static_cast<long>(pWorld->GetGameTime());
}

void RDNStats::RecordStop(const RDNWorld *pWorld)
{
	UNREF_P(pWorld);

	// unregister self from game events
	GameEventSys::Instance()->UnregisterClient(this);

	// if the game is over, players still playing won
	if (pWorld->IsGameOver())
	{
		unsigned long numPlayers = m_pimpl->m_players.size();
		for (unsigned long index = 0; index < numPlayers; index++)
		{
			if (m_pimpl->m_players[index].state == SPS_Playing)
			{
				m_pimpl->m_players[index].state = SPS_Won;
			}
		}
	}
}

void RDNStats::RecordInit(const RDNWorld *pWorld)
{
	unsigned long numPlayers = pWorld->GetPlayerCount();
	unsigned long index = m_pimpl->m_players.size();

	m_pimpl->m_players.resize(index + numPlayers);

	for (; index < numPlayers; index++)
	{
		const RDNPlayer *pPlayer =
				static_cast<const RDNPlayer *>(pWorld->GetPlayerAt(index));
		dbAssert(pPlayer);

		m_pimpl->m_players[index].idplayer = pPlayer->GetID();
		m_pimpl->m_players[index].state = SPS_Playing;
		m_pimpl->m_players[index].name = pPlayer->GetName();
	}
}

void RDNStats::Reset()
{
	// clear the player stats
	m_pimpl->m_timeTotal = 0;
	m_pimpl->m_players.clear();
}

void RDNStats::Save(IFF &iff) const
{
	// special case for frame zero
	if (ModObj::i()->GetWorld()->GetGameTime() == 0.0f)
		return;

	// save everything
	iff.PushChunk(Type_Form, 'SLOG', 1000);

	// game stats
	iff.PushChunk(Type_NormalVers, 'SLGS', 1000);

	IFFWrite(iff, m_pimpl->m_timeTotal);

	iff.PopChunk();

	// player stats
	StatPlayers::const_iterator pi = m_pimpl->m_players.begin();
	StatPlayers::const_iterator pe = m_pimpl->m_players.end();
	for (; pi != pe; ++pi)
	{
		iff.PushChunk(Type_NormalVers, 'SLPS', 1000);

		IFFWrite(iff, pi->idplayer);

		IFFWriteString(iff, pi->name.c_str());
		IFFWrite(iff, long(pi->state));

		iff.PopChunk();
	}

	iff.PopChunk();
}

void RDNStats::Load(IFF &iff)
{
	iff.AddParseHandler(&HandleSLOG, Type_Form, 'SLOG', this, 0);
}

unsigned long RDNStats::HandleSLOG(IFF &iff, ChunkNode *node, void *ctx1, void *ctx2)
{
	UNREF_P(node);

	// player stats
	iff.AddParseHandler(&HandleSLGS, Type_NormalVers, 'SLGS', ctx1, ctx2);
	iff.AddParseHandler(&HandleSLPS, Type_NormalVers, 'SLPS', ctx1, ctx2);

	return iff.Parse();
}

unsigned long RDNStats::HandleSLGS(IFF &iff, ChunkNode *node, void *ctx1, void *ctx2)
{
	UNREF_P(node);
	UNREF_P(ctx2);

	RDNStats *pThis = static_cast<RDNStats *>(ctx1);
	dbAssert(pThis);

	IFFRead(iff, pThis->m_pimpl->m_timeTotal);

	return 0;
}

unsigned long RDNStats::HandleSLPS(IFF &iff, ChunkNode *node, void *ctx1, void *ctx2)
{
	UNREF_P(node);
	UNREF_P(ctx2);

	//
	RDNStats *pThis = static_cast<RDNStats *>(ctx1);
	dbAssert(pThis);

	// load player
	unsigned long idplayer = 0;
	IFFRead(iff, idplayer);

	// create entry for that player
	unsigned long index = pThis->m_pimpl->m_players.size();
	pThis->m_pimpl->m_players.resize(pThis->m_pimpl->m_players.size() + 1);

	pThis->m_pimpl->m_players[index].idplayer = idplayer;

	wchar_t name[256];
	IFFReadString(iff, name, LENGTHOF(name));
	pThis->m_pimpl->m_players[index].name = name;

	IFFRead(iff, reinterpret_cast<long &>(pThis->m_pimpl->m_players[index].state));

	return 0;
}

long RDNStats::TotalDuration() const
{
	return m_pimpl->m_timeTotal;
}

DLLScoreInterface::PlayerState
RDNStats::PlayerFinalState(unsigned long idplayer) const
{
	// get the player stats
	StatPlayer *pPlayer = GetPlayerByID(m_pimpl->m_players, idplayer);
	if (pPlayer != NULL)
	{
		switch (pPlayer->state)
		{
		case SPS_Killed:
			return DLLScoreInterface::PS_KILLED;

		case SPS_Playing:
		case SPS_Won:
			return DLLScoreInterface::PS_WON;
		}
	}

	return DLLScoreInterface::PS_WON; // assume Won!
}

const wchar_t *RDNStats::PlayerName(unsigned long idplayer) const
{
	// get the player stats
	StatPlayer *pPlayer =
			GetPlayerByID(m_pimpl->m_players, idplayer);
	if (pPlayer != NULL)
	{
		return pPlayer->name.c_str();
	}

	return L"";
}
