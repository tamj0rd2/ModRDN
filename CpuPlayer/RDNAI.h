/////////////////////////////////////////////////////////////////////
// File  : RDNAI.h
// Author: Shelby
// Date  : 2001-01-03
//      (c) relic entertainment inc.2001
// 

#pragma once

//------------------------------------------------------------------
// ModRDN includes
//------------------------------------------------------------------

//------------------------------------------------------------------
// External module includes
//------------------------------------------------------------------
#include <ModInterface/GameAI.h>

//------------------------------------------------------------------
// AI Components
//------------------------------------------------------------------

//------------------------------------------------------------------
// Forward declaration
//------------------------------------------------------------------
class CommandInterface;
class RDNPlayer;
class ChunkNode;

//------------------------------------------------------------------
// Constants
//------------------------------------------------------------------

//------------------------------------------------------------------
// Class: RDNAI
//------------------------------------------------------------------

class RDNAI : 
	public GameAI
{
// construction
public:	
	RDNAI( CommandInterface* dispatch );
	virtual	~RDNAI();

// inherited -- GameAI
public:

	virtual void			AIInit( unsigned long PlayerID, const char* script );

	virtual unsigned long	GetPlayerID() const;

	virtual void			Think( float currentTime );

	// save load, should be used for SP only.  That way AI transfer works in MP.
	virtual void			Save( IFF& iff ) const;

	virtual void			Load( IFF& iff );
	
private:
	static unsigned long	HandleAISV( IFF& iff, ChunkNode*, void* ctx1, void* ctx2);

private:
	RDNPlayer*				m_player;
};


