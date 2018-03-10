#pragma once

#include "GameLoop.hpp"
#include "MissionDetails.hpp"

class CombatScreen : public GameLoop{
public:
    CombatScreen(Console& argCon, ScreensController* argSCon, MissionDetails argMd) : GameLoop(argCon, argSCon), md(argMd){}
    void init()override;
    void drawPanel()override;
    void onKeyboardEvent(const KeyboardEvent &evt)override;
    void onMouseEvent(const MouseEvent& evt)override;
protected:
    MissionDetails md;
    IPos panelPos;
    IPos lookPos;

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
    size_t scanCreateIndex;
    std::vector<IPos> scanTargets;
    std::vector<ScanItem> scanItems;
    std::vector<RingItem> scanRing;

    void startScan();
    void progressScan();

    void nrgyChanged();

    void printPanelText(Color clr, const char* txt);
    void printBar(Color clr1, Color clr2, float val);

};
