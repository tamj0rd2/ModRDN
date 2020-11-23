/////////////////////////////////////////////////////////////////////
//	File	: ModController.h
//	Desc.	:
//		12.Dec.00 (c) Relic Entertainment Inc.
//
#pragma once

#include <SimEngine/SimController.h>

#include "ControllerTypes.h"
#include "../Extensions/Extension.h"
#include "../States/State.h"

class ECStaticInfo;

/////////////////////////////////////////////////////////////////////
// ModController

class ModController : public SimController
{
	// construction
protected:
	ModController(Entity *e, EntityDynamics *pDynamics, const ECStaticInfo *pStatinf);

public:
	virtual ~ModController();

	// interface
public:
	inline const ECStaticInfo *GetECStaticInfo() const { return m_pStatinf; }

	ControllerType GetControllerType() const;

	// query for the Interface of the specified ID
	virtual Extension *QI(unsigned char InterfaceID);

	// Retrieve the state asked for, only if it is the current state.
	// E.G. if you Querry for the ActiveState and specify, StateAttack::StateID, and the current
	// state of that controller was StateIdle, you will get NULL.
	// If you want a pointer to the current state, regardless of what it is, just call QIActiveState with no parameters
	// and you will get the currently active state, without filtering.
	State *QIActiveState();

	inline const State *QIActiveState() const;

	virtual State *QIActiveState(unsigned char StateID);

	inline const State *QIActiveState(unsigned char StateID) const;

	inline State *QISubState(unsigned char StateID);

	// inherited -- EntityController
public:
	// Controllers should override this OnSpawnEntity function and fixify their controller states
	//	They must also call the ModController base OnSpawnEntity() function.
	virtual void OnSpawnEntity();
	virtual void OnDeSpawnEntity();

	virtual bool CommandDoProcessNow(const EntityCommand *);
	virtual bool CommandIsClearQueue(const EntityCommand *) const;

	virtual bool Update(const EntityCommand *currentCommand);

	// This command is called once per simstep.
	virtual void Execute();

	// Called before just before the simulation starts
	virtual void SimulatePre();

	// Save and Load functions.  You MUST call the base class version of this function first
public:
	virtual void Save(BiFF &biff) const;

	virtual void Load(IFF &iff);

protected:
	// the PlayerFOW needs access to the SightExt for processing
	// of entities death, this is hackish but will re-visit
	friend class PlayerFOW;

	// specifically needs access to QIStateAll. I don't understand why it's not public
	friend class RDNPlayer;

	// this class can be used by derived controllers to process all EntityCommands and perform
	// the appropriate state change
	friend class CommandProcessor;

	// query for the Interface of the specified ID, the inheriting class must always return the appropriate extension
	// Regardless of state.  The save and load code use this function
	virtual Extension *QIAll(unsigned char);

	// query for the Interface of the specified ID
	inline const Extension *QIAll(unsigned char id) const;

	// retrieve the State of the specified ID if the controller has one
	virtual State *QIStateAll(unsigned char StateID);

	// retrieve the State of the specified ID if the controller has one
	inline const State *QIStateAll(unsigned char StateID) const;

	// Tell the controller which state to set as Active
	virtual void SetActiveState(unsigned char StateID);

protected:
	// If a command processor is set then derived controllers,
	// don't need to override CommandProcessNow etc. functions
	void SetCommandProcessor(CommandProcessor *);

	// fields
private:
	// static info
	const ECStaticInfo *m_pStatinf;

	//
	CommandProcessor *m_pCommandProc;

	// chunk handler
private:
	// Chunk Handlers for the Save Game code
	static unsigned long HandleMCEC(IFF &, ChunkNode *, void *, void *);
};

/////////////////////////////////////////////////////////////////////
// inline functions

inline const State *ModController::QIActiveState() const
{
	return const_cast<ModController *>(this)->QIActiveState();
}

inline const State *ModController::QIActiveState(unsigned char StateID) const
{
	return const_cast<ModController *>(this)->QIActiveState(StateID);
}

inline const Extension *ModController::QIAll(unsigned char id) const
{
	return const_cast<ModController *>(this)->QIAll(id);
}

inline const State *ModController::QIStateAll(unsigned char StateID) const
{
	return const_cast<ModController *>(this)->QIStateAll(StateID);
}

inline State *ModController::QISubState(unsigned char StateID)
{
	State *pCurState = QIActiveState();
	if (pCurState)
	{
		return pCurState->GetSubState(StateID);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
// helpers

// Extension helpers
template <class T>
inline T *QIExt(ModController *pMC)
{
	return (pMC == 0) ? 0 : static_cast<T *>(pMC->QI(T::ExtensionID));
}

template <class T>
inline T *QIExt(EntityController *pEC)
{
	return QIExt<T>(static_cast<ModController *>(pEC));
}

template <class T>
inline const T *QIExt(const EntityController *pEC)
{
	return QIExt<T>(const_cast<EntityController *>(pEC));
}

template <class T>
inline T *QIExt(Entity *pE)
{
	return (pE == 0) ? 0 : QIExt<T>(pE->GetController());
}

template <class T>
inline const T *QIExt(const Entity *pE)
{
	return QIExt<T>(const_cast<Entity *>(pE));
}

// State helpers
template <class T>
inline T *QIState(ModController *pMC)
{
	return (pMC == 0) ? 0 : static_cast<T *>(pMC->QIActiveState(T::StateID));
}

template <class T>
inline T *QIState(EntityController *pEC)
{
	return QIState<T>(static_cast<ModController *>(pEC));
}

template <class T>
inline const T *QIState(const EntityController *pEC)
{
	return QIState<T>(const_cast<EntityController *>(pEC));
}

template <class T>
inline T *QIState(Entity *pE)
{
	return (pE == 0) ? 0 : QIState<T>(pE->GetController());
}

template <class T>
inline const T *QIState(const Entity *pE)
{
	return QIState<T>(const_cast<Entity *>(pE));
}

// SubState helpers
template <class T>
inline T *QISubState(ModController *pMC)
{
	return (pMC == 0) ? 0 : static_cast<T *>(pMC->QISubState(T::StateID));
}

template <class T>
inline T *QISubState(EntityController *pEC)
{
	return QISubState<T>(static_cast<ModController *>(pEC));
}

template <class T>
inline const T *QISubState(const EntityController *pEC)
{
	return QISubState<T>(const_cast<EntityController *>(pEC));
}

template <class T>
inline T *QISubState(Entity *pE)
{
	return (pE == 0) ? 0 : QISubState<T>(pE->GetController());
}

template <class T>
inline const T *QISubState(const Entity *pE)
{
	return QISubState<T>(const_cast<Entity *>(pE));
}
