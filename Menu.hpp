#pragma once

#include "GameScreen.hpp"
#include "Console.hpp"

class Menu : public GameScreen {
public:
  Menu(Console& argCon, ScreensController* argScon, IPos argOrgPos):GameScreen(argCon, argScon), orgPos(argOrgPos){}
  Menu& add(const std::string& item, std::function<void()> action)
  {
    modified = true;
    items.emplace_back(item, action);
    return *this;
  }

  enum class MenuTextAlignment{
    left,
    right,
    center
  };

  Menu& setOrigin(RectOrigin argOrigin)
  {
    menuOrigin=argOrigin;
    modified=true;
    return *this;
  }

  Menu& setOriginPos(IPos argPos)
  {
    orgPos = argPos;
    modified = true;
    return *this;
  }

  Menu& setTextAlignment(MenuTextAlignment newTextAlignment)
  {
    menuTextAlignment=newTextAlignment;
    return *this;
  }


  Menu& setBgColor(Color argBgColor)
  {
    bgColor=argBgColor;
    return *this;
  }

  Menu& setFgColor(Color argFgColor)
  {
    fgColor=argFgColor;
    return *this;
  }

  Menu& setSelBgColor(Color argSelBgColor)
  {
    selBgColor=argSelBgColor;
    return *this;
  }

  Menu& setAutoClose(bool argAutoClose)
  {
    autoClose=argAutoClose;
    return *this;
  }

  Menu& setTitle(const std::string& argTitle)
  {
    title=argTitle;
    modified=true;
    return *this;
  }

  void onKeyboardEvent(const KeyboardEvent& evt)override
  {
    if(evt.eventType!=ketPress)
    {
      return;
    }
    using namespace glider::keyboard;
    switch(evt.keySym)
    {
      case GK_UP:--selectedIndex;break;
      case GK_DOWN:++selectedIndex;break;
      case GK_HOME:selectedIndex=0;break;
      case GK_END:selectedIndex=items.size()-1;break;
      case GK_RETURN:
      case GK_RETURN2:
      case GK_SPACE:
      case GK_KP_ENTER:
      {
        if(selectedIndex<(int)items.size())
        {
          items[selectedIndex].action();
          if(autoClose)
          {
            close();
          }
        }
        return;
      }
    }
    selectedIndex=(selectedIndex+items.size()) % items.size();
  }

  void draw()override
  {
    if(modified)
    {
      updatePosWidth();
    }
    IPos p = rect.pos;
    con.fillRect(rect, bgColor);
    if(!title.empty())
    {
      con.printAlignedAt(p, rect.size.x, fgColor, {}, con.pfKeepBackground|con.pfAlignCenter, title.c_str());
      ++p.y;
    }
    con.fillRect({rect.pos+IPos{0, selectedIndex+(title.empty()?0:1)}, {rect.size.x,1}}, selBgColor);
    if(menuTextAlignment == MenuTextAlignment::left)
    {
      ++p.x;
    }
    int idx = 0;
    for(auto& item:items)
    {
      if(menuTextAlignment == MenuTextAlignment::right)
      {
        p.x = rect.pos.x - 1 - item.text.length();
      }
      else if(menuTextAlignment == MenuTextAlignment::center)
      {
        p.x = rect.pos.x + (rect.size.x-item.text.length())/2;
      }
      if(idx++==selectedIndex)
      {
        con.printAt({rect.pos.x, p.y}, fgColor, {}, Console::pfKeepBackground, ">");
        con.printAt({rect.pos.x+rect.size.x-1, p.y}, fgColor, {}, Console::pfKeepBackground, "<");
        con.printAt(p, fgColor, {}, Console::pfKeepBackground, item.text.c_str());
      }
      else
      {
        con.printAt({rect.pos.x, p.y}, fgColor, {}, Console::pfKeepBackground, " ");
        con.printAt({rect.pos.x+rect.size.x-1, p.y}, fgColor, {}, Console::pfKeepBackground, " ");
        con.printAt(p, fgColor, {}, Console::pfKeepBackground, item.text.c_str());
      }
      ++p.y;
    }
  }
protected:
  IPos orgPos;
  IRect rect;
  int width;
  RectOrigin menuOrigin = roScreenCenter;
  MenuTextAlignment menuTextAlignment = MenuTextAlignment::center;
  bool modified = true;

  int selectedIndex = 0;

  bool autoClose = true;

  std::string title;

  Color bgColor = Color::blue / 2;
  Color fgColor = Color::white;
  Color selBgColor = Color::red / 2;

  void updatePosWidth()
  {
    modified = false;
    width = title.length();
    for(auto& item:items)
    {
      width=std::max(width, (int)item.text.length());
    }
    width+=2;
    rect = calcRect(orgPos, {width, (int)items.size()+(title.empty()?0:1)}, menuOrigin);
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
