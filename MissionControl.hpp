#pragma once

#include "GameScreen.hpp"
#include "MissionDetails.hpp"
#include "Rand.hpp"
#include <set>
#include "AgentEquipment.hpp"

class MissionControl: public GameScreen{
public:
  MissionControl(Console& argCon, ScreensController* argScon):GameScreen(argCon, argScon){}
  void init()override;

  void onTopScreen()override
  {
    close();
  }

  void onKeyboardEvent(const KeyboardEvent& )override{};
  void draw()override;

  static bool saveExists();

protected:
  int rp=0;

  static const std::string saveFileName;
  static const uint32_t saveVersion=0x0100;

  uint32_t seed;
  RandGen rnd;

  std::set<AgentEquipmentType> equipped;

  void refreshMissions();

  std::vector<MissionDetails> availableMissions;
  int selectedMission=-1;

  GameScreenPtr menu;

  void selectMission();
  void selectEquipment();
  void startMission();

  void onMissionEnd(MissionResultInfo mri);

  void save(bool missionStart=false);
  void load();
};
