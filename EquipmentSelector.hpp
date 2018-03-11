#pragma once

#include "GameScreen.hpp"
#include "AgentEquipment.hpp"
#include <set>
#include <functional>

class EquipmentSelector: public GameScreen{
public:
  EquipmentSelector(Console& argCon, ScreensController* argScon,const std::set<AgentEquipmentType>& argEquipped):
    GameScreen(argCon, argScon),equipped(argEquipped){}
  void init()override;
  void onTopScreen()override;
  void draw()override;

  void setCompletionHandler(std::function<void(std::set<AgentEquipmentType>)> argHandler)
  {
    handler=argHandler;
  }

protected:
  void updateMenu();
  bool updated=false;
  int selectionIdx=0;
  std::set<AgentEquipmentType> equipped;
  std::function<void(std::set<AgentEquipmentType>)> handler;
};
