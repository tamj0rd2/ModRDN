/////////////////////////////////////////////////////////////////////
// File    : State.h
// Desc    :
// Created : Wednesday, September 19, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

/////////////////////////////////////////////////////////////////////
// Forward Declarations

class EntityDynamics;
class ChunkNode;
class IFF;
class BiFF;
class Entity;

/////////////////////////////////////////////////////////////////////
// State
//

class State
{
	// types
public:
	/////////////////////////////////////////////////////////////////////
	// NOTE: Do NOT modify the order of this list.  If you need to add a
	// new state ID then make sure you add it to the bottom.
	// Also do not delete ID's in the middle these values are saved to disk
	// and need to remain the same always.
	//

	enum StateIDType
	{
		SID_Current = 0,
		SID_Begin = 1,

		SID_Idle = 1,
		SID_Move = 2,
		SID_GroupMove = 3,
		SID_Attack = 4,
		SID_AttackMove = 5,
		SID_Dead = 6,
		SID_Pause = 7,

		SID_Gather = 8,
		SID_Build = 9,

		SID_NULLState,
	};

	// fields
private:
	EntityDynamics *m_pDynamics;

	// construction
public:
	State(EntityDynamics *e_dynamics);

	// interface
public:
	const EntityDynamics *GetDynamics() const;
	EntityDynamics *GetDynamics();

public:
	// Note: Classes should specify their own Begin functions with the required parameters.
	//	The controller is responsible for changing States and has the logic of what
	//	to pass to each State to begin it.

	// Pure virtual function set to be overloaded by specific states.
	// A return of true means the state is done.
	// a return of false means the state needs more updates
	// to exit (e.g. a tank going into siege mode).
	virtual bool Update() = 0;

	// Request that the state exit, when it feels like, e.g. for construction
	// exit as soon as the first building is built
	virtual void SoftExit();

	// Request the state to exit, as soon as possible.
	virtual void RequestExit() = 0;

	// Tell the state that it needs to reset itself to be re-entered on the next update
	// cause its being force exited
	virtual void ForceExit() = 0;

	// tell the state to reissue itself as an order
	virtual void ReissueOrder() const = 0;

	// does this state allow this command to be processed
	virtual bool AcceptCommand(int);

	// retrieve the ID of this state
	virtual StateIDType GetStateID() const = 0;

	// Must be implemented by the state, even if the implementation does nothing.
	virtual void SaveState(BiFF &) const = 0;
	virtual void LoadState(IFF &) = 0;

	virtual State *GetSubState(unsigned char StateID);

public:
	inline bool IsExiting() const;

	Entity *GetEntity();
	const Entity *GetEntity() const;

protected:
	inline void SetExitStatus(bool bExit);

private:
	bool m_bExiting;
};

/////////////////////////////////////////////////////////////////////
// inlines

inline const EntityDynamics *State::GetDynamics() const
{
	return m_pDynamics;
}

inline EntityDynamics *State::GetDynamics()
{
	return m_pDynamics;
}

inline bool State::IsExiting() const
{
	return m_bExiting;
}

inline void State::SetExitStatus(bool bExit)
{
	m_bExiting = bExit;
}
