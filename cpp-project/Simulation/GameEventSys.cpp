/////////////////////////////////////////////////////////////////////
// File    : GameEventSys.cpp
// Desc    :
// Created : Tuesday, June 05, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "GameEventSys.h"

/////////////////////////////////////////////////////////////////////
//

namespace
{
	typedef std::vector<GameEventSys::Listener *> ListenerList;
}

/////////////////////////////////////////////////////////////////////
// GameEventSys Static function

static GameEventSys *s_instance = NULL;

class GameEventSys::Data
{
public:
	// list of all the clients
	ListenerList m_listenerList;
};

GameEventSys::GameEventSys()
		: m_pimpl(new Data)
{
}

GameEventSys::~GameEventSys()
{
	DELETEZERO(m_pimpl);
}

void GameEventSys::Initialize()
{
	dbAssert(s_instance == NULL);

	s_instance = new GameEventSys;
}

void GameEventSys::Shutdown()
{
	dbAssert(s_instance);

	DELETEZERO(s_instance);
}

GameEventSys *GameEventSys::Instance()
{
	dbAssert(s_instance);

	return s_instance;
}

/////////////////////////////////////////////////////////////////////
// GameEventSys Main API functions

void GameEventSys::PublishEvent(const Event &event)
{
	dbTracef("GameEventSys::PublishEvent %d", event.GetType());

	const size_t n = m_pimpl->m_listenerList.size();

	// go through each client and call its virtual function
	ListenerList::const_iterator i = m_pimpl->m_listenerList.begin();
	ListenerList::const_iterator e = m_pimpl->m_listenerList.end();

	for (; i != e; ++i)
	{
		(*i)->OnEvent(event);
	}

	dbTracef("GameEventSys::PublishEvent events done publishing");

	//
	dbAssert(n == m_pimpl->m_listenerList.size());

	dbTracef("GameEventSys::PublishEvent returning");
	return;
}

void GameEventSys::RegisterClient(Listener *client)
{
	// validate parm
	dbAssert(client != 0);

	// check for duplicate
	ListenerList::const_iterator b = m_pimpl->m_listenerList.begin();
	ListenerList::const_iterator e = m_pimpl->m_listenerList.end();

	dbAssert(std::find(b, e, client) == e);

	// add client to list
	m_pimpl->m_listenerList.push_back(client);

	return;
}

void GameEventSys::UnregisterClient(Listener *client)
{
	// validate parm
	dbAssert(client != 0);

	// find client
	ListenerList::iterator b = m_pimpl->m_listenerList.begin();
	ListenerList::iterator e = m_pimpl->m_listenerList.end();

	ListenerList::iterator found = std::find(b, e, client);

	dbAssert(found != e);

	// remove from list
	m_pimpl->m_listenerList.erase(found);

	return;
}
