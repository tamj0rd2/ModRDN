#pragma once

#include "ModController.h"

#include "../Extensions/ResourceExt.h"

#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/ResourceExtInfo.h"
#include "../ExtInfo/UIExtInfo.h"

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

	// inherited from ResourceExt.h
public:
	virtual float GetResources() const;
	virtual void SetResources(float amount);

	// return amount decreased
	virtual float DecResources(float amount);

protected:
	virtual void OnZeroResources();

private:
	virtual void OnResourceProgress(float amount);

private:
	virtual ModController *GetSelf();
};
