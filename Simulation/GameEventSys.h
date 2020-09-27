/////////////////////////////////////////////////////////////////////
//	File	: GameEventSys.h
//	Desc.	: 
//	Date	: May 31, 2001
//  Author  : Shelby	
//

#pragma once

// forward declaration
class RDNPlayer;

///////////////////////////////////////////////////////////////////// 
// GameEventSys

class GameEventSys
{
// construction
private:
	// singleton
	 GameEventSys();
	~GameEventSys();

public:
	// static interface
	static void				Initialize();
	static void				Shutdown();
	static GameEventSys*	Instance();

// types
public:
	class Event;
	class Listener;
	
// interface
public:
	void	RegisterClient  ( Listener* );
	void	UnregisterClient( Listener* );

	void	PublishEvent( const Event& );

// fields
private:
	class Data;
	Data* m_pimpl;
	
// copy -- do not define
private:
	GameEventSys( const GameEventSys& );
	GameEventSys& operator= ( const GameEventSys& );
};

///////////////////////////////////////////////////////////////////// 
// GameEventSys::GameEventBase

// * the base class for all game events

class GameEventSys::Event
{
// fields
private:
	const RDNPlayer*	
			m_player;
	int		m_type;			// the type of event this is, so we can static cast to it

// construction
protected:
	Event( int type, const RDNPlayer* p )
		: m_type  ( type ), 
		  m_player( p ) 
	{
	}

public:
	~Event()
	{
	}

// interface
public:
	const RDNPlayer*	GetPlayer() const {return m_player;}
	int					GetType  () const {return m_type;}
};
	
///////////////////////////////////////////////////////////////////// 
// GameEventSys::GameEventListener

// * listener class - clients should inherit from this

class GameEventSys::Listener
{
public:
	virtual void OnEvent( const Event& ) = 0;
};
