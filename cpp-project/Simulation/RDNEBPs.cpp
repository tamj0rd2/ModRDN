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

void RDNEBP::Preload()
{
#define LOAD(t) \
	ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);

	LOAD(Lab);
	LOAD(Henchmen);

#undef LOAD

	return;
}

const ControllerBlueprint *RDNEBP::Get(const EBPName &t)
{
	return ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);
}
