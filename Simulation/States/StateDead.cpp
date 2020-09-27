/////////////////////////////////////////////////////////////////////
// File    : StateDead.cpp
// Desc    : 
// Created : Wednesday, March 07, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "StateDead.h"

#include "../../ModObj.h"

#include "../RDNWorld.h"
#include "../RDNTuning.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/HealthExtInfo.h"

#include <SimEngine/Entity.h>
#include <SimEngine/SimEntity.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/TerrainHMBase.h>

#include <SimEngine/Target.h>

//////////////////////////////////////////////////////////////////////////////////////

namespace
{
	const int k_deathTicks = 16;
};

//////////////////////////////////////////////////////////////////////////////////////
// StateDead
//////////////////////////////////////////////////////////////////////////////////////

void StateDead::Enter()
{
	//	HACK : signal the entity to play a death animation
	Entity* pEntity = GetEntity();
	if ( pEntity->GetAnimator() )
	{
		bool bPlayDefDeathAnim = true;

		if ( m_bUseWaterDeathAnim ) 
		{
			// is this entity in water?
			if (GetDynamics()->GetVisualMovementType() == EntityDynamics::eEDWater)
			{
				pEntity->GetAnimator()->SetMotionTreeNode( "SwimStumbleDie" );
				bPlayDefDeathAnim = false;
			}
		}

		if ( bPlayDefDeathAnim )
		{
			pEntity->GetAnimator()->SetStyle( 'DEAD' );
		}

		pEntity->GetAnimator()->SetOccludee( false );
	}

	// handle any dynamics death action (like falling to the ground for a flyer)
	GetDynamics()->RequestDie( );

	//	Calculate when the death is done
	m_DeathCount = ModObj::i()->GetWorld()->GetGameTicks() + k_deathTicks;

	SetExitStatus( false );
}

void StateDead::UseWaterDeathAnim( bool bUseWaterDeathAnim )
{
	m_bUseWaterDeathAnim = bUseWaterDeathAnim;
}

void StateDead::SetFadeDelay( long ticks )
{
	m_fadeDelayCount = ticks;
}

bool StateDead::Update()
{
	// do nothing if this entity does not fade away or get deleted when it dies
	ModController* pController = static_cast< ModController* >( GetEntity()->GetController() );
	const HealthExtInfo* healthExtInfo = QIExtInfo< HealthExtInfo >( pController );
	if ( healthExtInfo )
	{
		if ( !healthExtInfo->fadeAndDeleteWhenDead )
		{
			return false;
		}
	}

	if ( ModObj::i()->GetWorld()->GetGameTicks() > ( m_DeathCount + m_fadeDelayCount ) )
	{
		ModObj::i()->GetWorld()->DeleteEntity( GetEntity() );
	}

	//	Update the death opacity
	Entity* pEntity = GetEntity();
	if ( pEntity->GetAnimator() )
	{
		int deathStart = m_DeathCount-k_deathTicks;
		float t = (ModObj::i()->GetWorld()->GetGameTicks() - deathStart - m_fadeDelayCount)/float(k_deathTicks);
		if ( t >= 0 )
		{
			pEntity->GetAnimator()->SetOpacity( 1.f-t );
		}
	}

	// Death can't be exited EVER bwahahahaha.
	return false;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateDead::RequestExit()
{
	//
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateDead::ForceExit()
{
	//
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool StateDead::AcceptCommand( int )
{
	return false;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
State::StateIDType StateDead::GetStateID( ) const
{
	return (State::StateIDType)StateID;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateDead::SaveState( BiFF& ) const
{
	
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void StateDead::LoadState( IFF& )
{
	Enter( );
}