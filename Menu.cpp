#include "Menu.hpp"


void Menu::onKeyboardEvent(const KeyboardEvent& evt)
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
    case GK_PAGEUP:
    case GK_HOME:selectedIndex=0;break;
    case GK_PAGEDOWN:
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
    case GK_ESCAPE:
    {
      close();
      return;
    }
    default:break;
  }
  selectedIndex=(selectedIndex+items.size()) % items.size();
  if(items[selectedIndex].onSelected)
  {
    items[selectedIndex].onSelected();
  }
}

void Menu::draw()
{
  if(modified)
  {
    updatePosWidth();
  }
  IPos p = rect.pos;
  con.clearRect(rect);
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

