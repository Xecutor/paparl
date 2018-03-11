#pragma once

#include "GameScreen.hpp"
#include "TextPanel.hpp"

class TextMessage:public GameScreen{
public:
  TextMessage(Console& argCon, ScreensController* argScon, const std::string& argText):
    GameScreen(argCon, argScon), textPanel(argCon)
  {
    TextPanel::splitTextToLines(argText, [this](std::string line){lines.push_back(std::move(line));});
  }
  void init()override;
  void draw()override;
  void onKeyboardEvent(const KeyboardEvent& evt)override;

  void setFg(const Color& argFg)
  {
    fg=argFg;
  }
  void setBg(const Color& argBg)
  {
    bg=argBg;
  }
protected:
  std::vector<std::string> lines;
  TextPanel textPanel;
  Color bg{0x1a468c};
  Color fg=Color::black;
};
