/////////////////////////////////////////////////////////////////////
// File    : StatePause.cpp
// Desc    :
// Created : Thursday, June 4, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "StatePause.h"

#include "../../ModObj.h"

#include "../CommandTypes.h"
#include "../RDNWorld.h"

#include "../Controllers/ModController.h"

#include "../Extensions/ModifierExt.h"

#include <SimEngine/Entity.h>
#include <SimEngine/Player.h>
#include <SimEngine/EntityDynamics.h>
#include <SimEngine/EntityAnimator.h>

#include <Util/BiFF.h>
#include <Util/iff.h>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
StatePause::StatePause(EntityDynamics *e_dynamics)
		: State(e_dynamics),
			m_bIgnoreCmds(false),
			m_pauseCount(0),
			m_bImmobilized(false)
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::IncrementPauseCount()
{
	dbAssert(m_pauseCount > 0);
	++m_pauseCount;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::Enter(bool bIgnoreCmds)
{
	dbAssert(m_pauseCount == 0);
	m_pauseCount = 1;

	SetExitStatus(false);

	m_bIgnoreCmds = bIgnoreCmds;

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool StatePause::Update()
{
	if (IsExiting())
	{ // done

		// if we immobilized the thing, mobilize it
		if (m_bImmobilized)
		{
			EntityDynamics *pDynamics = GetDynamics();
			pDynamics->RequestStop();
		}

		m_bImmobilized = false;
		return true;
	}
	else
	{ // not done yet
		return false;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::RequestExit()
{
	dbAssert(m_pauseCount > 0);
	if (m_pauseCount > 0)
	{
		--m_pauseCount;
		if (m_pauseCount > 1)
		{
			return;
		}
		SetExitStatus(true);
	}
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
void StatePause::ReissueOrder() const
{
	const unsigned long playerID = GetEntity()->GetOwner() ? GetEntity()->GetOwner()->GetID() : 0;
	const unsigned long entityID = GetEntity()->GetID();

	ModObj::i()->GetWorld()->DoCommandEntity(CMD_Pause,
																					 0,
																					 CMDF_Queue,
																					 playerID,
																					 &entityID,
																					 1);
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
bool StatePause::AcceptCommand(int cmdtype)
{
	if (!m_bIgnoreCmds)
		return true;

	if (cmdtype == CMD_Destroy || cmdtype == CMD_Pause || cmdtype == CMD_UnPause)
	{
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::ForceExit()
{
	m_pauseCount = 0;
	SetExitStatus(true);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
State::StateIDType StatePause::GetStateID() const
{
	return (State::StateIDType)StateID;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::SaveState(BiFF &) const
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void StatePause::LoadState(IFF &)
{
	Enter();
}