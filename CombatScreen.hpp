#pragma once

#include "GameLoop.hpp"
#include "MissionDetails.hpp"
#include "TextPanel.hpp"


class CombatScreen : public GameLoop{
public:
  CombatScreen(Console& argCon, ScreensController* argSCon, MissionDetails argMd,
               const std::set<AgentEquipmentType>& argEquipped,
               std::function<void(MissionResultInfo)> argOnMissionEnd) :
    GameLoop(argCon, argSCon), md(argMd), equipped(argEquipped), onMissionEnd(argOnMissionEnd){}
  void init()override;
  void drawPanel()override;
  void onKeyboardEvent(const KeyboardEvent &evt)override;
  void onMouseEvent(const MouseEvent& evt)override;
  void onTopScreen()override;

  struct Animation{
    virtual ~Animation(){}
    virtual bool isBlocking()
    {
      return true;
    }
    virtual void step()=0;
    virtual bool hasFinished()const=0;
  };

  using AnimationPtr=std::shared_ptr<Animation>;

  void addAnimation(AnimationPtr ani)
  {
    animations.push_back(ani);
  }

  void restoreMapAt(IPos p);

  IPos getTargetPos()
  {
    return targetPos;
  }

protected:
  MissionDetails md;
  std::set<AgentEquipmentType> equipped;
  IPos panelPos;
  IPos lookPos;

  enum TargetFor{
    tfNone,
    tfFire,
    tfThrow,
  };

  bool targetMode=false;
  TargetFor targetFor = tfNone;
  IPos targetPos;

  enum class EscapeMode
  {
    none,
    escape,
    evac
  };
  EscapeMode escapeMode=EscapeMode::none;

  int delayedClose = -1;

  std::function<void(MissionResultInfo)> onMissionEnd;

  struct ScanAnimation;

  void playerDied()override;
  void abortMission(bool evac);

  void afterTurn()override;

  std::vector<AnimationPtr> animations;

  struct ScanItem{
    IPos dst;
    Color clr;
    bool needToFade;
    int fadeCount=20;
  };
  struct RingItem{
    IPos pos;
    AngleRange r;
  };
  bool scanAnimation=false;
  size_t scanCreateIndex = 0;
  std::vector<IPos> scanTargets;
  std::vector<ScanItem> scanItems;
  std::vector<RingItem> scanRing;

  void startScan();
  void progressScan();

  void nrgyChanged();

  void enterTargetMode();

  void printPanelText(Color clr, const char* txt);
  void printBar(Color clr1, Color clr2, float val);

};
