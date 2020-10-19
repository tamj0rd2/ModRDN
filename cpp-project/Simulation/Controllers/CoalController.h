#pragma once

#include "ModController.h"

#include "../Extensions/ResourceExt.h"

#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/ResourceExtInfo.h"
#include "../ExtInfo/UIExtInfo.h"

#include "../States/State.h"
#include "../States/StateIdle.h"

#include "../CommandProcessor.h"

class CoalController : public ModController,
											 private ResourceExt
{
	// types
public:
	class StaticInfo : private UIExtInfo,
										 private SiteExtInfo,
										 private ResourceExtInfo,
										 public ModStaticInfo
	{
	public:
		StaticInfo(const ControllerBlueprint *);

	public:
		virtual const ModStaticInfo::ExtInfo *QInfo(unsigned char id) const;
	};

	// construction
public:
	CoalController(Entity *pEntity, const ECStaticInfo *);
	virtual ~CoalController();

public:
	virtual bool Update(const EntityCommand *currentCommand);

	virtual Extension *QI(unsigned char InterfaceID);

	virtual State *QIActiveState(unsigned char stateid);

protected:
	virtual void OnZeroResources();

private:
	void OnSpawnEntity();

	virtual ModController *GetSelf();

	State *m_pCurrentState;
	StateIdle m_stateidle;

	virtual void SetActiveState(unsigned char);

	virtual Extension *QIAll(unsigned char);

	virtual State *QIStateAll(unsigned char);

	CommandProcessor m_commandproc;
};
