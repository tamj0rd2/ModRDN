/////////////////////////////////////////////////////////////////////
// File    : RDNEntityFilter.cpp
// Desc    : 
// Created : Tuesday, July 03, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//
#include "pch.h"
#include "RDNEntityFilter.h"

#include "../ModObj.h"

#include "../Simulation/RDNPlayer.h"
#include "../Simulation/RDNWorld.h"
#include "../Simulation/Controllers/ModController.h"

///////////////////////////////////////////////////////////////////// 
// 
namespace 
{
	class LocalPlayerPred : public std::unary_function< const Entity*, bool >
	{
	public:
		LocalPlayerPred( const RDNPlayer* pPlayer )
		:	m_pPlayer( pPlayer )
		{
		}

		bool operator() ( const Entity* pEntity ) const
		{
			if ( m_pPlayer == NULL )
				return false;

			return m_pPlayer->CanControlEntity( pEntity );
		}

	private:
		const RDNPlayer*	m_pPlayer;
	};

	struct SingleSelectPred : public std::unary_function< const Entity*, bool >
	{
	public:
		SingleSelectPred( const RDNPlayer* pPlayer )
		:	m_pPlayer( pPlayer )
		{
		}

		bool operator() ( const Entity* pEntity ) const
		{
			if ( pEntity->GetEntityFlag( EF_SingleSelectOnly ) )
				return true;
			if ( m_pPlayer == NULL )
				return true;
			if ( !m_pPlayer->CanControlEntity( pEntity ) )
				return true;
			return false;
		}

	private:
		const RDNPlayer*	m_pPlayer;
	};

};

///////////////////////////////////////////////////////////////////// 
// 

static RDNEntityFilter* s_instance = NULL; 

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNEntityFilter::RDNEntityFilter()
:	m_pLocalPlayer(NULL),
	m_filterMode(FILTER_Invalid),
	m_bContext_Enabled(false),
	m_bContext_PickLocal(false)
{

}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNEntityFilter::Initialize( void )
{
	// already initialized 
	dbAssert(!s_instance);

	s_instance = new RDNEntityFilter;

	return true;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::Shutdown( void )
{
	// never initialized 
	dbAssert(s_instance);

	DELETEZERO(s_instance);
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
RDNEntityFilter* RDNEntityFilter::Instance(void)
{
	// never initialized 
	dbAssert(s_instance);

	return s_instance;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::SetMode( FilterMode filterMode )
{
	m_filterMode = filterMode;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::ClearMode()
{
	m_filterMode = FILTER_Invalid;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNEntityFilter::QueryFilter( const Entity* pEntity ) const
{
	switch ( m_filterMode )
	{
		case FILTER_Select :
		case FILTER_Bandbox	:
			// Check Selectable flag
			if( !pEntity->GetEntityFlag( EF_Selectable )  )
			{
				return false;
			}

			// Check fog of war visibility
			if( m_pLocalPlayer )
			{
				if( !ModObj::i()->GetWorld()->IsEntityVisible(m_pLocalPlayer,pEntity) )
				{
					// isn't visible
					return false;	
				}
			}
			break;

		case FILTER_SingleSelect :
			// Check Selectable flag
			if( !pEntity->GetEntityFlag( EF_Selectable )  )
			{
				return false;
			}

			// Check the single-select-only flag
			if( IsSingleSelect( pEntity ) )
			{
				return true;
			}

			if( m_pLocalPlayer )
			{
				// Reject local multi-select units
				if ( m_pLocalPlayer->CanControlEntity( pEntity ) )
				{
					return false;
				}

				// Check fog of war visibility
				if ( !ModObj::i()->GetWorld()->IsEntityVisible(m_pLocalPlayer,pEntity) )
				{
					return false;
				}
			}
			break;

		case FILTER_Context :
			if( !pEntity->GetEntityFlag( EF_Selectable )  )
			{
				return false;
			}
			if( !m_bContext_Enabled )
			{
				return true;
			}
			break;

		case FILTER_LocalPlayer :
			if( !IsLocalPlayerUnit(pEntity) )
			{
				return false;
			}
			break;

		default :
			dbBreak();
	}

	//	defaults to success of the filter
	return true;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
int RDNEntityFilter::QueryPriority( const Entity* pEntity ) const
{
	int priority = -1;

	switch ( m_filterMode )
	{
		case FILTER_Select :
		case FILTER_SingleSelect :
			// priority order is:
			// [Highest]	multi-select units			(local and non-local)
			// [Lowest]		single-select-only units	(local and non-local)
			if( IsSingleSelect( pEntity ) )
				return 1;
			else
				return 0;
			break;

		case FILTER_Bandbox :
			// priority order is:
			// [Highest]	multi-select units			(local)
			// [.]			single-select-only units	(local)
			// [.]			single-select-only units	(non-local)
			// [Lowest]		multi-select units			(non-local)
			if( IsLocalPlayerUnit(pEntity) )
			{
				if( IsSingleSelect( pEntity ) )
					return 2;	// single-select-only units		(local)
				else
					return 3;	// multi-select units			(local)
			}
			else
			{
				if( IsSingleSelect( pEntity ) )
					return 1;	// single-select-only units		(non-local)
				else
					return 0;	// multi-select units			(non-local)
			}
			break;

		case FILTER_Context :
			if ( !m_bContext_Enabled )
			{
				return 0;
			}

			if ( m_bContext_PickLocal )
			{
				if ( IsLocalPlayerUnit(pEntity) )
					return 1;
				else
					return 0;
			}
			else
			{
				if ( IsLocalPlayerUnit(pEntity) )
					return 0;
				else
					return 1;
			}
			break;
	}

	dbAssert( priority >= 0 );
	return priority;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::FilterGroup( EntityGroup& eg ) const
{
	switch ( m_filterMode )
	{
		case FILTER_Select :
		{
			break;
		}
		case FILTER_Bandbox :
		{
			if ( eg.size() < 2 )
				break;

			//	Filter out any single select when there are multi-selects
			size_t nSingleSelect = std::count_if( eg.begin(), eg.end(), SingleSelectPred( m_pLocalPlayer ) );
			if ( nSingleSelect > 0 )
			{
				if ( nSingleSelect == size_t(eg.size()) )
				{
					//	Leave the first one
					Entity* pEntity = eg.front();
					eg.clear();
					eg.push_back( pEntity );
				}
				else
				{
					//	Remove all single selects
					eg.erase( std::partition( eg.begin(), eg.end(), std::not1(SingleSelectPred( m_pLocalPlayer )) ), eg.end() );
				}
			}
			break;
		}

		case FILTER_SingleSelect :
		case FILTER_Context :
			break;

		case FILTER_LocalPlayer :
			//	Remove any that don't belong to the local player
			eg.erase( std::partition( eg.begin(), eg.end(), LocalPlayerPred( m_pLocalPlayer ) ), eg.end() );
			break;

		default :
			dbBreak();
	}

	return;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::SetContextGroup( const EntityGroup& )
{
	//	Default to acting on allies
	m_bContext_Enabled = true;
	m_bContext_PickLocal = true;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::ClearContextGroup()
{
	m_bContext_Enabled = false;
	m_bContext_PickLocal = false;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::SetExclusionGroup( const EntityGroup& eg )
{
	m_exclusionGroup = eg;
}

//////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::ClearExclusionGroup()
{
	m_exclusionGroup.clear();
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
void RDNEntityFilter::SetLocalPlayer( const RDNPlayer *localPlayer )
{
	m_pLocalPlayer = localPlayer;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNEntityFilter::IsLocalPlayerUnit( const Entity *pEntity ) const
{
	// Check if unit is an enemy or unowned unit
	if( m_pLocalPlayer == 0 || !m_pLocalPlayer->CanControlEntity( pEntity ) )
	{
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////// 
// Desc.     : 
// Result    : 
// Param.    : 
// Author    : 
//
bool RDNEntityFilter::IsSingleSelect( const Entity* pEntity ) const
{
	if ( pEntity->GetEntityFlag( EF_SingleSelectOnly ) )
		return true;
	if ( !IsLocalPlayerUnit( pEntity ) )
		return true;
	return false;
}
