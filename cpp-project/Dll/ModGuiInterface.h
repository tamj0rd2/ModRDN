#pragma once

#include <ModInterface/DllInterface.h>

class ModGuiInterface : public DLLGuiInterface
{

public:
  virtual void InitLuaGui(LuaConfig *);
  virtual void ShutLuaGui(LuaConfig *);

  virtual void OnEntityCreate(const Entity *);

  virtual void ChangePlayerArmy(unsigned long PlayerID, const std::vector<long> &soldiers);

  // can be NULL
  virtual ModSimVis *GetModSimVis();

  // can be NULL
  virtual ModUIEvent *GetModUIEvent();

  virtual EntityFilter *GetEntityFilter();

  virtual NISletInterface *GetNISletInterface();

  virtual void DoCommand(const EntityGroup &);

  virtual void DoCommand(const Vec3f *, unsigned long numVecs);

  virtual bool ProcessInput(const Plat::InputEvent &ie);

  virtual const char *GetCursor(const Entity *mouseOverEntity);

  virtual void CreateHUD(
      const Player *localplayer,
      RTSHud *hud,
      CommandInterface *command,
      UIInterface *ui,
      MessageInterface *message,
      SelectionInterface *sel,
      CameraInterface *cam,
      SoundInterface *sound,
      FXInterface *fx);

  virtual void ShutdownHUD();

  virtual void UpdateHUD(float elapsedSeconds);

  virtual void UIPause(bool bPause);

  virtual void Save(IFF &);
  virtual void Load(IFF &);

  virtual void ShowModOptions(void);
};
