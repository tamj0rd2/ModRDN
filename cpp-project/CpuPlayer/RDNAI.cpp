/////////////////////////////////////////////////////////////////////
// File  : RDNAI.h
// Author: Shelby
// Date  : 2001-1-3
//      (c) relic entertainment inc.2001
//
//

#include "pch.h"
#include "RDNAI.h"

#include "../ModObj.h"
#include "../RDNDLLSetup.h"

#include "../Simulation/RDNWorld.h"
#include "../Simulation/RDNPlayer.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityGroup.h>
#include <SimEngine/SimHelperFuncs.h>
#include <SimEngine/TerrainHMBase.h>
#include <SimEngine/Pathfinding/ImpassMap.h>
#include <SimEngine/Pathfinding/Pathfinding.h>
#include <SimEngine/Pathfinding/PathfinderQuery.h>
#include <SimEngine/GroundDynamics.h>

#include <EngineAPI/CommandInterface.h>
#include <EngineAPI/EntityFactory.h>
#include <EngineAPI/ControllerBlueprint.h>

#include <ModInterface/ECStaticInfo.h>

#include <Filesystem/ByteStream.h>
#include <Filesystem/FilePath.h>

#include <Util/DebugRender.h>
#include <Util/Logfile.h>
#include <Util/Iff.h>

#include <Platform/Platform.h>

#define VERBOSELOG 1

#if !(defined(shubick) || defined(ddunlop))
#undef FIXME
#define FIXME(x) comment(user, #x)
#endif

/////////////////////////////////////////////////////////////////////
// RDNAI

RDNAI::RDNAI(CommandInterface *)
		: m_player(NULL)
{
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

RDNAI::~RDNAI()
{
}

//------------------------------------------------------------------------------------------------
// This is when the RDNPlayer has been initialized
//------------------------------------------------------------------------------------------------

void RDNAI::AIInit(unsigned long PlayerID, const char *script)
{
	UNREF_P(script);

	dbAssert(m_player == NULL);
	m_player = static_cast<RDNPlayer *>(ModObj::i()->GetWorld()->GetPlayerFromID(PlayerID));
	dbAssert(m_player != NULL);

	return;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

unsigned long RDNAI::GetPlayerID() const
{
	return m_player->GetID();
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void RDNAI::Think(float)
{
	return;
}

void RDNAI::Save(IFF &iff) const
{
	iff.PushChunk(Type_NormalVers, 'AISV', 1000);
	iff.PopChunk();

	return;
}

void RDNAI::Load(IFF &iff)
{
	//	Add parse handlers, let the caller perform the Parse()
	iff.AddParseHandler(&HandleAISV, Type_Form, 'AISV', this, 0);
}

unsigned long RDNAI::HandleAISV(IFF &, ChunkNode *, void *, void *)
{
	return 0;
}
