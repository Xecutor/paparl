#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <stdint.h>

#include "kst/Throw.hpp"
#include "kst/Logger.hpp"

#include "Engine.hpp"
#include "Drawable.hpp"
#include "EventHandler.hpp"
#include "ResourcesManager.hpp"
#include "Types.hpp"
#include "VertexBuffer.hpp"
#include "Text.hpp"
#include "Image.hpp"
#include "GLState.hpp"
#include "Timer.hpp"

using namespace glider;

class Console:public EventHandler,public Drawable{
public:
  void init(const std::string& fontFile, int fontSize, int argTileWidth, int argTileHeight, int argW,int argH);
  virtual ~Console(){}
  Color curFg,curBg;
  bool curGlow = false;
  void clear();
  enum PrintFlags{
    pfKeepBackground,
    pfKeepForeground,
    pfMakeBackgroundDarker,
  };
  void printAt(IPos pos, uint32_t flags, const char* str);
  void printAt(IPos pos, const Color& fg, const Color& bg, uint32_t flags, const char* str);

  void fillRect(const IRect& rect, const Color& bg);

  int width()const
  {
    return w;
  }
  int height()const
  {
    return h;
  }
  void setDrawCb(std::function<void()> argDrawCb)
  {
    drawCb = argDrawCb;
  }
  void setKeyCb(std::function<void(const KeyboardEvent&)> argKeyCb)
  {
    keyCb = argKeyCb;
  }
protected:
  virtual void onActiveChange(bool active){}
  virtual void onResize(){}
  virtual void onQuit()
  {
    engine.exitApp();
  }
  
  void draw()override;
  void onFrameUpdate(int mcsec)override
  {
    if(drawCb) {
      drawCb();
    }
  }
  
  virtual void onUserEvent(void* data1,void* data2){}
  
  void onKeyboardEvent(KeyboardEvent& argEvent)override
  {
    if(keyCb) {
      keyCb(argEvent);
    }
  }

  using ColorVector=std::vector<uint32_t>;
  using ColorMatrix=std::vector<ColorVector>;
  using SymbolVector=std::vector<ushort>;
  using SymbolMatrix=std::vector<SymbolVector>;
  void printEx(int x,int y,const ColorVector& argFg,const ColorVector& argBg,const SymbolVector& argSym);

  FontRef fnt, fntGlow;
  Text txt, txtGlow;
  Image* img;
  VertexBuffer vb;
  VertexBuffer bg;
  int w,h;
  int tileWidth, tileHeight;
  typedef std::unordered_map<uint32_t,Pos> CharMap;
  CharMap cm;
  int fillX=0;
  int fillY=0;
  float ch,cw;

  ColorMatrix bgm,bgmOld;
  ColorMatrix fgm,fgmOld;
  SymbolMatrix sym,symOld;

  static const uint32_t glowBit = 0x10000;
  
  std::function<void()> drawCb;
  std::function<void(const KeyboardEvent&)> keyCb;
  std::function<void(const MouseEvent&)> mouseCb;

  uint32_t getCharCode(ushort c)
  {
    return curGlow?c | glowBit : c;
  }

  CharMap::iterator initChar(ushort c, bool glow=false);
  friend class Snapshot;
};

class Snapshot{
public:
  Snapshot(Console* argCon, const IPos& pos,const IPos& argSize):con(argCon)
  {
    IPos size=argSize;
    if(pos.x+size.x>con->width())
    {
      size.x=con->width()-pos.x;
    }
    if(pos.y+size.y>con->height())
    {
      size.y=con->height()-pos.y;
    }
    for(int y=0;y<size.y;++y)
    {
      fg.push_back(Console::ColorVector());
      fg.back().insert(fg.back().begin(),con->fgm[pos.y+y].begin()+pos.x,con->fgm[pos.y+y].begin()+pos.x+size.x);
      bg.push_back(Console::ColorVector());
      bg.back().insert(bg.back().begin(),con->bgm[pos.y+y].begin()+pos.x,con->bgm[pos.y+y].begin()+pos.x+size.x);
      sym.push_back(Console::SymbolVector());
      sym.back().insert(sym.back().begin(),con->sym[pos.y+y].begin()+pos.x,con->sym[pos.y+y].begin()+pos.x+size.x);
    }
  }
  void draw(const IPos& pos)
  {
    if(fg.empty())
    {
      return;
    }
    for(size_t y=0;y<fg.size();++y)
    {
      con->printEx(pos.x,pos.y+y,fg[y],bg[y],sym[y]);
    }
  }
protected:
  Console::ColorMatrix fg,bg;
  Console::SymbolMatrix sym;
  Console* con;
};
