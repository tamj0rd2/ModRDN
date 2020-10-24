#include "pch.h"
#include "RDNEBPs.h"

#include "../ModObj.h"

#include <EngineAPI/EntityFactory.h>

/**
 * == RDNEBP ==
 * As far as I can tell, we only preload these specific EBPs because they are
 * used directly in our code. i.e for conditions about whether there are enough
 * labs etc.
 * */
const RDNEBP::EBPName RDNEBP::Lab = {"structures", "lab"};
const RDNEBP::EBPName RDNEBP::Henchmen = {"gatherers", "henchmen"};
const RDNEBP::EBPName RDNEBP::LightningRod = {"structures", "lightning_rod"};
const RDNEBP::EBPName RDNEBP::CreatureChamber = {"structures", "remote_chamber"};
const RDNEBP::EBPName RDNEBP::WaterChamber = {"structures", "water_chamber"};
const RDNEBP::EBPName RDNEBP::AirChamber = {"structures", "aviary"};
const RDNEBP::EBPName RDNEBP::GeoGenerator = {"structures", "geogenerator"};
const RDNEBP::EBPName RDNEBP::BrambleFence = {"structures", "bramble_fences"};
const RDNEBP::EBPName RDNEBP::Workshop = {"structures", "foundry"};
const RDNEBP::EBPName RDNEBP::SoundBeamTower = {"structures", "soundbeamtower"};

void RDNEBP::Preload()
{
#define LOAD(t) \
	ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);

	LOAD(Lab);
	LOAD(Henchmen);
	LOAD(CreatureChamber);
	LOAD(WaterChamber);
	LOAD(AirChamber);
	LOAD(GeoGenerator);
	LOAD(BrambleFence);
	LOAD(Workshop);
	LOAD(SoundBeamTower);
	LOAD(LightningRod);

#undef LOAD

	return;
}

const ControllerBlueprint *RDNEBP::Get(const EBPName &t)
{
	return ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);
}
