#pragma once

#include "GameScreen.hpp"
#include "Console.hpp"

class Menu : public GameScreen {
public:
  Menu(Console& argCon, ScreensController* argScon, IPos argOrgPos={}):GameScreen(argCon, argScon), orgPos(argOrgPos){}
  void clear()
  {
    items.clear();
    modified=true;
  }
  Menu& add(const std::string& item, std::function<void()> action, std::function<void()> onSelected={})
  {
    modified = true;
    items.emplace_back(item, action, onSelected);
    return *this;
  }

  Menu& setSelectedIndex(int argSelectedIndex)
  {
    selectedIndex=argSelectedIndex;
    return *this;
  }

  bool isFullScreen()const override
  {
    return false;
  }

  bool dimBackground()const override
  {
    return doDimBackground;
  }

  Menu& setDimBackground(bool value)
  {
    doDimBackground=value;
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

  void onKeyboardEvent(const KeyboardEvent& evt)override;

  void draw()override;
protected:
  IPos orgPos;
  IRect rect;
  int width;
  RectOrigin menuOrigin = roScreenCenter;
  MenuTextAlignment menuTextAlignment = MenuTextAlignment::center;
  bool modified = true;

  int selectedIndex = 0;

  bool autoClose = true;
  bool doDimBackground = false;

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
    std::function<void()> onSelected;
    MenuItem()=default;
    MenuItem(const MenuItem&)=default;
    MenuItem(MenuItem&&)=default;
    MenuItem(const std::string& argText, std::function<void()> argAction, std::function<void()> argOnSelected):
      text(argText), action(argAction),onSelected(argOnSelected){}
  };
  std::vector<MenuItem> items;
};
