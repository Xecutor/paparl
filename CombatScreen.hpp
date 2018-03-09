#pragma once

#include "GameLoop.hpp"
#include "MissonDetails.hpp"

class CombatScreen : public GameLoop{
public:
    CombatScreen(Console& argCon, ScreensController* argSCon, MissionDetails argMd) : GameLoop(argCon, argSCon), md(argMd){}
    void init()override;
protected:
    MissionDetails md;
};
