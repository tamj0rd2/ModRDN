#include "pch.h"
#include "RDNEBPs.h"

#include "../ModObj.h"

#include <EngineAPI/EntityFactory.h>

/**
 * == RDNEBP ==
 * As far as I can tell, we only preload these specific EBPs because they are
 * used directly in our code. i.e for conditions about whether there are enough
 * labs etc. I imagine Rock, Paper and Scissor are here because they need
 * specific RDN controllers applied to them
 * */
const RDNEBP::EBPName RDNEBP::HQ = {"structures", "lab"};
const RDNEBP::EBPName RDNEBP::Rock = {"units", "rock"};
const RDNEBP::EBPName RDNEBP::Paper = {"units", "paper"};
const RDNEBP::EBPName RDNEBP::Scissor = {"units", "scissor"};

void RDNEBP::Preload()
{
#define LOAD(t) \
	ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);

	LOAD(HQ);
	LOAD(Rock);
	LOAD(Paper);
	LOAD(Scissor);

#undef LOAD

	return;
}

const ControllerBlueprint *RDNEBP::Get(const EBPName &t)
{
	return ModObj::i()->GetEntityFactory()->GetControllerBP(t.folder, t.file);
}
