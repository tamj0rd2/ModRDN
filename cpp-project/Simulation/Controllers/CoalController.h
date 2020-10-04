#pragma once

#include "ModController.h"

#include "../Extensions/ResourceExt.h"
#include "../Extensions/ResourceExt.h"

#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/ResourceExtInfo.h"
#include "../ExtInfo/UIExtInfo.h"

class CoalController : public ModController
{
	// types
public:
	class StaticInfo : private UIExtInfo,
										 private SiteExtInfo,
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

private:
	virtual ModController *GetSelf();
};
