#pragma once

#include "GameScreen.hpp"
#include "Console.hpp"

class Menu : public GameScreen {
public:
  enum MenuOrigin{
    moTopLeft,
    moTopRight,
    moBottonLeft,
    moBottomRight,
    moTopCenter,
    moLeftCenter,
    moRightCenter,
    moBottomCenter,
    moCenter,
    moScreenCenter
  };
  Menu(Console& argCon, ScreensController* argScon, IPos argOrgPos):GameScreen(argCon, argScon), orgPos(argOrgPos){}
  Menu& add(const std::string& item, std::function<void()> action)
  {
    modified = true;
    items.emplace_back(item, action);
    return *this;
  }

  void onKeyboardEvent(const KeyboardEvent& evt)override
  {
    //
  }

  void draw()override
  {
    if(modified)
    {
      updatePosWidth();
    }
    IPos p = pos;
    for(auto& item:items)
    {
      con.printAt(p, Color::white, Color::blue, 0, item.item.c_str());
      ++p.y;
    }
  }
protected:
  IPos orgPos;
  IPos pos;
  int width;
  MenuOrigin menuOrigin = moScreenCenter;
  bool modified = true;

  void updatePosWidth()
  {
    modified = true;
    width = 0;
    for(auto& item:items)
    {
      width=std::max(width, item.text.length());
    }
    width+=2;
    switch(menuOrigin)
    {
      case moTopLeft:
        pos = orgPos;
        break;
      case moTopRight:
        pos = orgPos - IPos(width, 0);
        break;
      case moBottonLeft:
        pos = orgPos - IPos(0, items.size());
        break;
      case moBottomRight:
      case moTopCenter:
      case moLeftCenter:
      case moRightCenter:
      case moBottomCenter:
      case moCenter:
      case moScreenCenter:
    }
  }

  struct MenuItem{
    std::string text;
    std::function<void()> action;
    MenuItem()=default;
    MenuItem(const MenuItem&)=default;
    MenuItem(MenuItem&&)=default;
    MenuItem(const std::string& argText, std::function<void()> argAction):text(argText), action(argAction){}
  };
  std::vector<MenuItem> items;
};
