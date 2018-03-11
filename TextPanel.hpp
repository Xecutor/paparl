#pragma once

#include "Console.hpp"

class TextPanel{
public:
  TextPanel(Console& argCon, IRect argRect={}):con(argCon), rect(argRect), pos(argRect.pos)
  {}
  void setPrintFlags(uint32_t argPrintFlags)
  {
    printFlags = argPrintFlags;
  }
  void addLine(const std::string& line);
  void addText(const std::string& text);

  static void splitTextToLines(const std::string& text, std::function<void(std::string)> cb);

  void reset()
  {
    pos=rect.pos;
  }

  void advance()
  {
    pos.y+=dir;
  }

  void setRect(IRect argRect)
  {
    rect=argRect;
  }
  const IRect& getRect()const
  {
    return rect;
  }
  void setPrintPos(IPos argPos)
  {
    pos=argPos;
  }
  void setPrintDir(int argDir)
  {
    dir=argDir;
  }
protected:
  Console& con;
  IRect rect;
  IPos pos;
  int dir=1;
  Color fg=Color::white;
  uint32_t printFlags=Console::pfKeepBackground;
};
