#include "pch.h"
#include "ModGuiInterface.h"

#include <ModInterface/DllInterface.h>
#include <ModInterface/ModSimVis.h>
#include <ModInterface/ModUIEvent.h>
#include <ModInterface/NISletInterface.h>
#include <ModInterface/EntityFilter.h>

#include "ModObj.h"
#include "UI/RDNHUD.h"
#include "UI/RDNUIState.h"
#include "UI/RDNEntityFilter.h"
#include "UI/RDNNISletInterface.h"
#include "Simulation/RDNPlayer.h"

#define LOGIT(x) \
  dbTracef("ModGuiInterface::" #x)

#define TODO(x) \
  dbFatalf("ModGuiInterface::" #x " NOT YET IMPLEMENTED")

#define REVISIT(x) \
  dbTracef("ModGuiInterface::" #x " Needs to be revisited and understood")

ModGuiInterface::ModGuiInterface()
{
}

void ModGuiInterface::InitLuaGui(LuaConfig *lc)
{
  TODO(InitLuaGui);
}

void ModGuiInterface::ShutLuaGui(LuaConfig *)
{
  TODO(ShutLuaGui);
}

void ModGuiInterface::OnEntityCreate(const Entity *)
{
  // could make a call to show/hide ghosts here
  LOGIT(OnEntityCreate);
}

void ModGuiInterface::ChangePlayerArmy(unsigned long PlayerID, const std::vector<long> &soldiers)
{
  TODO(ChangePlayerArmy);
}

// can be NULL
ModSimVis *ModGuiInterface::GetModSimVis()
{
  REVISIT(GetModSimVis);
  if (RDNHUD::IsInitialized())
    return RDNHUD::instance();

  return NULL;
}

// can be NULL
ModUIEvent *ModGuiInterface::GetModUIEvent()
{
  REVISIT(GetModUIEvent);
  return RDNHUD::instance();
}

EntityFilter *ModGuiInterface::GetEntityFilter()
{
  REVISIT(GetEntityFilter);
  return RDNEntityFilter::Instance();
}

NISletInterface *ModGuiInterface::GetNISletInterface()
{
  REVISIT(GetNISletInterface);
  return RDNNISletInterface::Instance();
}

void ModGuiInterface::DoCommand(const EntityGroup &g)
{
  REVISIT(DoCommand);
  RDNHUD::instance()->DoCommand(g);
}

void ModGuiInterface::DoCommand(const Vec3f *v, unsigned long n)
{
  REVISIT(DoCommand);
  RDNHUD::instance()->DoCommand(v, n);
}

bool ModGuiInterface::ProcessInput(const Plat::InputEvent &ie)
{
  REVISIT(ProcessInput);
  return RDNHUD::instance()->Input(ie);
}

const char *ModGuiInterface::GetCursor(const Entity *mouseOverEntity)
{
  REVISIT(GetCursor);
  return RDNHUD::instance()->GetCursor(mouseOverEntity);
}

void ModGuiInterface::CreateHUD(
    const Player *localplayer,
    RTSHud *hud,
    CommandInterface *command,
    UIInterface *ui,
    MessageInterface *message,
    SelectionInterface *sel,
    CameraInterface *cam,
    SoundInterface *sound,
    FXInterface *fx)
{
  REVISIT(CreateHUD);

  // these shouldn't be passed here
  ModObj::i()->SetSoundInterface(sound);
  ModObj::i()->SetFxInterface(fx);

  // these should be sent directly to the trigger system, NOT the ModObj
  ModObj::i()->SetCameraInterface(cam);
  ModObj::i()->SetSelectionInterface(sel);
  ModObj::i()->SetUIInterface(ui);

  RDNHUD::Initialize(
      static_cast<const RDNPlayer *>(localplayer),
      hud,
      command,
      ModObj::i()->GetSelectionInterface(),
      ModObj::i()->GetCameraInterface(),
      ui,
      ModObj::i()->GetSoundInterface(),
      ModObj::i()->GetFxInterface(),
      message);
}

void ModGuiInterface::ShutdownHUD()
{
  REVISIT(ShutdownHUD);
  RDNHUD::Shutdown();
}

void ModGuiInterface::UpdateHUD(float elapsedSeconds)
{
  REVISIT(UpdateHUD);
  RDNHUD::instance()->Update(elapsedSeconds);
}

void ModGuiInterface::UIPause(bool bPause)
{
  REVISIT(UIPause);
  RDNHUD::instance()->UIPause(bPause);
}

void ModGuiInterface::Save(IFF &iff)
{
  REVISIT(Save);
  RDNUIState::i()->Save(iff);
}

void ModGuiInterface::Load(IFF &iff)
{
  REVISIT(Load);
  RDNUIState::i()->Load(iff);
}

void ModGuiInterface::ShowModOptions(void)
{
  REVISIT(ShowModOptions);
  RDNHUD::instance()->ShowModOptions();
}

#undef TODO
#undef LOGIT
#undef REVISIT
