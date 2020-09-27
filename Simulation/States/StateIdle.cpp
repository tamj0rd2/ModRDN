/////////////////////////////////////////////////////////////////////
// File    : StateIdle.cpp
// Desc    : 
// Created : Friday, January 25, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateIdle.h"

#include "../../ModObj.h" 

#include "../RDNWorld.h" 
#include "../RDNQuery.h"
#include "../RDNPlayer.h"
#include "../PlayerFoW.h"

#include "../Controllers/ModController.h"

#include "../Extensions/SightExt.h"

#include <SimEngine/SimEntity.h>
#include <SimEngine/World.h>
#include <SimEngine/SpatialBucketSystem.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>

namespace
{
	/////////////////////////////////////////////////////////////////////
	// Find the closest enemy to reveal self to
	class FindClosestEnemyToRevealToFilter : public FindClosestEnemyFilter
	{
	public:
		FindClosestEnemyToRevealToFilter(const Player* pPlayer)
			:FindClosestEnemyFilter(pPlayer){};
		
		virtual bool Check( const Entity* pEntity );

	};

	bool FindClosestEnemyToRevealToFilter::Check( const Entity* pEntity )
	{
		return FindClosestEnemyFilter::Check( pEntity );
	}
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
StateIdle::StateIdle( EntityDynamics *e_dynamics ) : 
	State( e_dynamics ),
	m_tickBegin( 0 )
{

}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::Enter()
{
	//	HACK : signal the entity to play an idle animation
	Entity* pEntity = GetEntity();
	if( pEntity->GetAnimator() )
		pEntity->GetAnimator()->SetStyle( (GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater) ? 'SWIM' : 'IDLE' );

	SetExitStatus( false );

	// remember when we started
	m_tickBegin = ModObj::i()->GetWorld()->GetGameTicks();

	return;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool StateIdle::Update()
{
	// Idle can be exited at any stage.
	return IsExiting();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::SoftExit()
{
	SetExitStatus( true );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::RequestExit()
{
	SetExitStatus( true );
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: 
//	Result	: 
//	Param.	: 
//	Author	: 
//
void StateIdle::ReissueOrder() const
{
	// nothing to do
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::ForceExit()
{
	// nothing
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
State::StateIDType StateIdle::GetStateID( ) const
{
	return (State::StateIDType)StateID;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::SaveState( BiFF& ) const
{
	
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateIdle::LoadState( IFF& )
{
	Enter( );
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
long StateIdle::GetIdleTicks() const 
{
	const long current = 
		ModObj::i()->GetWorld()->GetGameTicks();

	return current - m_tickBegin;
}
