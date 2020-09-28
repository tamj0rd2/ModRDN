/////////////////////////////////////////////////////////////////////
// File    : MovingExt.cpp
// Desc    :
// Created : Thursday, June 28, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"

#include "MovingExt.h"

#include "../RDNTuning.h"
#include "../RDNPlayer.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/ModStaticInfo.h"
#include "../ExtInfo/MovingExtInfo.h"

#include <SimEngine/GroundDynamics.h>
#include <SimEngine/EntityAnimator.h>

#include <Util/Iff.h>
#include <Util/Biff.H>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
MovingExt::MovingExt(Entity *pEntity) : m_SpeedOveride(0.0f),
																				m_Multiplier(1.0f),
																				m_MultCount(0),
																				m_bIsDynSwimmer(false),
																				m_landSpeedBonus(0.0f),
																				m_waterSpeedBonus(0.0f),
																				m_airSpeedBonus(0.0f),
																				m_landBonusCount(0),
																				m_waterBonusCount(0),
																				m_airBonusCount(0)
{
	EntityAnimator *ea = pEntity->GetAnimator();
	if (ea)
	{
		const MovingExtInfo *pMovInfo = QIExtInfo<MovingExtInfo>(pEntity);
		if (pMovInfo)
		{
			if (pMovInfo->movingType == MovingExtInfo::MOV_GROUND ||
					pMovInfo->movingType == MovingExtInfo::MOV_AMPHIBIOUS)
			{
				ea->SetDrawStyle(EntityAnimator::DS_OrientGround);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float MovingExt::GetSpeed() const
{
	float mult = 1.0;

	// check to see if we should modify speed
	const MovingExtInfo *info = QIExtInfo<MovingExtInfo>(GetSelf());
	/*	if (info->bCannotBeSlowed && mult < 1.0f)
	{
		mult = 1.0f;
	}
*/
	if (m_SpeedOveride == 0.0f)
	{
		// note that the race multiplier is applied within GetNoOverrideSpeed
		return (GetNoOverrideSpeed() * m_Multiplier * mult) + GetSpeedBonus();
	}
	else
	{
		// note that the race multiplier is applied within GetNoOverrideSpeed

		// make sure the speed override isn't more that the inherent speed of the Entity
		const float speedBonus = GetSpeedBonus();
		float speed = __min(GetNoOverrideSpeed() + speedBonus, m_SpeedOveride);

		// when the speed override is in effect we don't apply bonuses, but slow down multipliers are used
		float multipliers = m_Multiplier * mult;
		if (multipliers > 1.0f)
		{
			multipliers = 1.0f;
		}
		return speed * multipliers;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::AddSpeedMultiplier(float multiplier)
{
	// validate parm
	dbAssert(multiplier >= 0.0f);

	// Note: applying N multipliers, m_i, has the following interpretation.
	//
	//		net_multiplier = 1.0 + sum( from i=0, to i=N-1, m_i - 1 );
	//
	// e.g. if m_0 = 1.5 and m_1 = 2.0, then applying both multipliers would give
	//		net_multiplier = 1.0 + (0.5 + 1.0) = 2.5
	//

	m_Multiplier += (multiplier - 1.0f);
	m_MultCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::RemSpeedMultiplier(float multiplier)
{
	// validate parm
	dbAssert(multiplier >= 0.0f);
	dbAssert(m_MultCount != 0);

	//
	m_Multiplier -= (multiplier - 1.0f);
	m_MultCount--;
	if (m_MultCount == 0)
	{
		m_Multiplier = 1.0f;
	}

	// validate object state
	dbAssert(m_Multiplier >= 0.0f);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::SetSpeedOverride(float speed)
{
	m_SpeedOveride = speed;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
float MovingExt::GetSpeedOverride() const
{
	return m_SpeedOveride;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::RemoveSpeedOverride()
{
	m_SpeedOveride = 0.0f;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float MovingExt::GetNoOverrideSpeed() const
{
	const MovingExtInfo *pMovInfo = QIExtInfo<MovingExtInfo>(GetSelf());

	const RDNPlayer *pPlayer = static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());
	float raceMult = pPlayer->GetRaceBonusSpeed(GetSelf()->GetEntity()->GetControllerBP());

	switch (pMovInfo->movingType)
	{
	//
	case MovingExtInfo::MOV_FLYER:
		return (raceMult * pMovInfo->airSpeed);
	//
	case MovingExtInfo::MOV_GROUND:
		return (raceMult * pMovInfo->speed);
	//
	case MovingExtInfo::MOV_SWIMMER:
		return (raceMult * pMovInfo->waterSpeed);
	//
	case MovingExtInfo::MOV_AMPHIBIOUS:
	{
		EntityDynamics::EDmovementType type = GetSelf()->GetEntityDynamics()->GetVisualMovementType();
		if (type == EntityDynamics::eEDLand)
		{
			return (raceMult * pMovInfo->speed);
		}
		else
		{
			return (raceMult * pMovInfo->waterSpeed);
		}
	}
	default:
		// should never happen
		dbBreak();
		return 0;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
float MovingExt::GetSpeedBonus() const
{
	const MovingExtInfo *pMovInfo = QIExtInfo<MovingExtInfo>(GetSelf());

	switch (pMovInfo->movingType)
	{
	//
	case MovingExtInfo::MOV_FLYER:
		return m_airSpeedBonus;
	//
	case MovingExtInfo::MOV_GROUND:
		return m_landSpeedBonus;
	//
	case MovingExtInfo::MOV_SWIMMER:
		return m_waterSpeedBonus;
	//
	case MovingExtInfo::MOV_AMPHIBIOUS:
	{
		EntityDynamics::EDmovementType type = GetSelf()->GetEntityDynamics()->GetVisualMovementType();
		if (type == EntityDynamics::eEDLand)
		{
			return m_landSpeedBonus;
		}
		else
		{
			return m_waterSpeedBonus;
		}
	}
	default:
		// should never happen
		dbBreak();
		return 0;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     : returns the speed of the Entity plus any speed bonuses.
//             Note: does not include multipliers (except race).
// Result    :
// Param.    :
// Author    :
//
float MovingExt::GetMaxNormalSpeed() const
{
	const MovingExtInfo *pMovInfo = QIExtInfo<MovingExtInfo>(GetSelf());

	const RDNPlayer *pPlayer = static_cast<const RDNPlayer *>(GetSelf()->GetEntity()->GetOwner());
	float raceMult = pPlayer->GetRaceBonusSpeed(GetSelf()->GetEntity()->GetControllerBP());

	float speed = 0.0f;

	switch (pMovInfo->movingType)
	{
	//
	case MovingExtInfo::MOV_FLYER:
		speed = raceMult * pMovInfo->airSpeed;
		break;
	//
	case MovingExtInfo::MOV_GROUND:
		speed = raceMult * pMovInfo->speed;
		break;
	//
	case MovingExtInfo::MOV_SWIMMER:
		speed = raceMult * pMovInfo->waterSpeed;
		break;
	//
	case MovingExtInfo::MOV_AMPHIBIOUS:
		speed = raceMult * std::max(pMovInfo->speed, pMovInfo->waterSpeed);
		break;
	//
	default:
		// should never happen
		dbBreak();
		break;
	}

	speed += GetSpeedBonus();

	return speed;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::SetDynSwimmer(bool swimmer)
{
	if (m_bIsDynSwimmer != swimmer)
	{
		EntityDynamics::EDmovementType type = GetSelf()->GetEntityDynamics()->GetVisualMovementType();

		if (type == EntityDynamics::eEDLand)
		{
			GroundDynamics *gd = static_cast<GroundDynamics *>(GetSelf()->GetEntityDynamics());

			TCMask pfCellMask = gd->GetPassibleTypes();
			if (swimmer)
			{
				// set the amphibian impassible bit
				pfCellMask |= eAmphibianImpassible;

				// clear the land impassible bit
				pfCellMask &= (0xff ^ eLandImpassible);
			}
			else
			{
				// set the land impassible bit
				pfCellMask |= eLandImpassible;

				// clear the amphibian impassible bit
				pfCellMask &= (0xff ^ eAmphibianImpassible);
			}

			gd->SetMovementMask(pfCellMask);
		}
		else
		{
			dbFatalf("MOD -- Setting dynamic swimming for entities without ground dynamics is not supported");
		}

		m_bIsDynSwimmer = swimmer;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool MovingExt::IsSwimmer() const
{
	if (!m_bIsDynSwimmer)
	{
		const MovingExtInfo *pExtInfo = QIExtInfo<MovingExtInfo>(GetSelf());
		return (pExtInfo->movingType == MovingExtInfo::MOV_SWIMMER) || (pExtInfo->movingType == MovingExtInfo::MOV_AMPHIBIOUS);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::SaveExt(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	biff.StartChunk(Type_NormalVers, 'EMOV', "Extension: Moving", 3);

	IFFWrite(iff, m_bIsDynSwimmer);

	// save states affected by research and upgrades
	IFFWrite(iff, m_landSpeedBonus);
	IFFWrite(iff, m_waterSpeedBonus);
	IFFWrite(iff, m_airSpeedBonus);

	IFFWrite(iff, m_landBonusCount);
	IFFWrite(iff, m_waterBonusCount);
	IFFWrite(iff, m_airBonusCount);

	IFFWrite(iff, m_Multiplier);
	IFFWrite(iff, m_MultCount);
	IFFWrite(iff, m_SpeedOveride);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleEMOV, Type_NormalVers, 'EMOV', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long MovingExt::HandleEMOV(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	long version = iff.GetNormalVersion();

	MovingExt *pMovingExt = static_cast<MovingExt *>(pContext1);
	dbAssert(pMovingExt);

	bool bDynSwimmer;
	IFFRead(iff, bDynSwimmer);

	pMovingExt->SetDynSwimmer(bDynSwimmer);

	if (version >= 2)
	{
		IFFRead(iff, pMovingExt->m_landSpeedBonus);
		IFFRead(iff, pMovingExt->m_waterSpeedBonus);
		IFFRead(iff, pMovingExt->m_airSpeedBonus);

		IFFRead(iff, pMovingExt->m_landBonusCount);
		IFFRead(iff, pMovingExt->m_waterBonusCount);
		IFFRead(iff, pMovingExt->m_airBonusCount);
	}

	if (version >= 3)
	{
		IFFRead(iff, pMovingExt->m_Multiplier);
		IFFRead(iff, pMovingExt->m_MultCount);
		IFFRead(iff, pMovingExt->m_SpeedOveride);

		dbAssert(pMovingExt->m_Multiplier >= 0.0f);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::AddLandSpeedBonus(float bonus)
{
	m_landSpeedBonus += bonus;
	m_landBonusCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::AddWaterSpeedBonus(float bonus)
{
	m_waterSpeedBonus += bonus;
	m_waterBonusCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::AddAirSpeedBonus(float bonus)
{
	m_airSpeedBonus += bonus;
	m_airBonusCount++;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::RemLandSpeedBonus(float bonus)
{
	m_landSpeedBonus -= bonus;
	m_landBonusCount--;
	if (m_landBonusCount == 0)
	{
		m_landSpeedBonus = 0.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::RemWaterSpeedBonus(float bonus)
{
	m_waterSpeedBonus -= bonus;
	m_waterBonusCount--;
	if (m_waterBonusCount == 0)
	{
		m_waterSpeedBonus = 0.0f;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void MovingExt::RemAirSpeedBonus(float bonus)
{
	m_airSpeedBonus -= bonus;
	m_airBonusCount--;
	if (m_airBonusCount == 0)
	{
		m_airSpeedBonus = 0.0f;
	}
}
