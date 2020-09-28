/////////////////////////////////////////////////////////////////////
// File    : MovingExt.h
// Desc    :
// Created : Thursday, June 28, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include "Extension.h"
#include "ExtensionTypes.h"

#include <SimEngine/EntityDynamics.h>

class MovingExtInfo;
class Entity;

///////////////////////////////////////////////////////////////////////////////
// MovingExt

class MovingExt : public Extension
{
	// types
public:
	enum
	{
		ExtensionID = EXTID_Moving,
	};

	// interface
public:
	float GetSpeed() const;

	void AddSpeedMultiplier(float multiplier);
	void RemSpeedMultiplier(float multiplier);

	void SetSpeedOverride(float speed);
	float GetSpeedOverride() const;

	void RemoveSpeedOverride();

	float GetNoOverrideSpeed() const;
	float GetSpeedBonus() const;
	// used for group movement
	float GetMaxNormalSpeed() const;

	// Hack to introduce dynamic movement behavior
	virtual void SetDynSwimmer(bool swimmer);
	bool IsSwimmer() const;

	// speed bonuses
	void AddLandSpeedBonus(float bonus);
	void RemLandSpeedBonus(float bonus);

	void AddWaterSpeedBonus(float bonus);
	void RemWaterSpeedBonus(float bonus);

	void AddAirSpeedBonus(float bonus);
	void RemAirSpeedBonus(float bonus);

	// inherited interface: Extension
private:
	virtual void SaveExt(BiFF &) const;
	virtual void LoadExt(IFF &);

	// Chunk Handlers
	static unsigned long HandleEMOV(IFF &iff, ChunkNode *, void *pContext1, void *);

	// construction
protected:
	MovingExt(Entity *pEntity);

	// fields
private:
	float m_Multiplier;
	unsigned char m_MultCount;

	float m_SpeedOveride;

	float m_landSpeedBonus;
	float m_waterSpeedBonus;
	float m_airSpeedBonus;

	unsigned char m_landBonusCount;
	unsigned char m_waterBonusCount;
	unsigned char m_airBonusCount;

	// dynamic behavior
	bool m_bIsDynSwimmer;
};
