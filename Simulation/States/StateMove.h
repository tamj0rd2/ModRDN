// StateMove.h

#pragma once

// Include Files
#include "State.h"
#include <Math/Vec3.h>

#include <SimEngine/Target.h>

// Forward Declarations
class Entity;
class EntityDynamics;

/////////////////////////////////////////////////////////////////////
// StateMove
// This state is responsible for moving the entity - (what does that entail?).

class StateMove : public State
{
	// types
public:
	enum
	{
		StateID = SID_Move,
	};

	enum RetryType
	{
		RT_None = 0x00,
		RT_BlockedOnEntity = 0x01,
		RT_BlockedOnEntityAllowSpace = 0x02,
		RT_BlockedOnTerrain = 0x04,
		RT_BlockedOnBuilding = 0x08
	};

	enum MoveExitState
	{
		MES_ReachedTarget,
		MES_StoppedBeforeTarget,
		MES_CantPathToTargetEntity,
		MES_CantPathToTargetTerrain,
		MES_CantPathToTargetBuilding,
		MES_NoTarget,
		MES_Invalid,
	};

	enum MoveSubState
	{
		MSS_Normal,
		MSS_WaitingToRetry,
		MSS_NormalEntityLost,
		MSS_MoveFailed,
	};

	// Data
private:
	std::vector<Vec3f> m_WayPoints;
	Vec3f m_Offset;

	Target m_Target;
	float m_AP; // Acceptible Proximity to target (only move to within this distance of target).
	bool m_bCheckFOW;

	unsigned long m_flags;

	MoveExitState m_exitstate;

	MoveSubState m_subState;

	RetryType m_retryType;
	long m_retryTime;
	int m_retryCount;
	int m_retryLimit;
	int m_waypointsVisited;

	// Functions.
public:
	StateMove(EntityDynamics *e_dynamics);

	void Enter(const Entity *pDest, float AP, bool bCheckFOW = false, unsigned long flags = 0, RetryType rt = RT_BlockedOnEntity, int retryLimit = 4);
	void Enter(const Vec3f &dest, float AP, unsigned long flags = 0, RetryType rt = RT_BlockedOnEntityAllowSpace, int retryLimit = 4);
	void Enter(const Vec3f &dest, const Vec3f &offset, float AP, unsigned long flags = 0, RetryType rt = RT_BlockedOnEntityAllowSpace, int retryLimit = 4);

	void AddWayPoint(const Vec3f &point);

	size_t GetNumWayPoints() const;
	const Vec3f *GetWayPoints() const;
	const int GetNumWayPointsVisited() const;

	const Vec3f &GetOffset() const;

	MoveExitState GetExitState();

	// Inherited -- State
public:
	virtual bool Update(); // Return values:
												 //	0 - still moving.
												 //	1 - stopped because reached its goal.
												 //	2 - can't find path to goal.
	virtual void RequestExit();

	virtual void ReissueOrder() const;

	virtual void ForceExit();

	// retrieve the ID of this state
	virtual State::StateIDType GetStateID() const;

	// Save Load
	virtual void SaveState(BiFF &) const;
	virtual void LoadState(IFF &);

	// Functions
private:
	void DoEnter(); // code common to bother Enter routines
	void DoRequestMove();

	void DoInternalEnter(const Vec3f &dest);

	void ToRetry(long retryTime);

	bool CantGetThereProcess();

	void IncrementWaypoint();

	bool HandleNormalState();
	bool HandleWaitingToRetry();
	bool HandleNormalEntityLost();
};
