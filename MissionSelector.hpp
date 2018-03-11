#pragma once

#include "GameScreen.hpp"
#include "MissionDetails.hpp"
#include "TextPanel.hpp"
#include <functional>

class MissionSelector: public GameScreen{
public:
  MissionSelector(Console& argCon, ScreensController* argScon, std::vector<MissionDetails>& argMissions, int argSelectedMission, std::function<void(int)> argOnSelectMission):
    GameScreen(argCon, argScon),missions(argMissions), selectedMission(argSelectedMission),onSelectMission(argOnSelectMission), details(argCon){}
  void init()override;
  void onTopScreen()override
  {
    close();
  }
  void draw()override;
  void onKeyboardEvent(const KeyboardEvent&)override{};
protected:
  std::vector<MissionDetails>& missions;
  int selectedMission=0;
  std::function<void(int)> onSelectMission;
  TextPanel details;
};
