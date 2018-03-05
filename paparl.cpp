#include <cstdint>
#include <list>
#include <map>
#include <vector>
#include <math.h>

#include <stdio.h>
#include "Engine.hpp"
#include "ResourcesManager.hpp"
#include "Rectangle.hpp"
#include "kst/Logger.hpp"
#include "Console.hpp"
#include "GumMap.hpp"

using namespace glider;

struct ConTest{
  void init(Console* argCon)
  {
    con=argCon;
    con->setDrawCb([this]() {onDraw();});
    con->setKeyCb([this](auto ke){onKeyPress(ke);});
    Color::red.toHSL(h,s,l);
    int mw=30;
    int mh=30;
    IRect r(0,0,mw,mw);
    for(auto p:r) {
        map.setBg(p.x, p.y, Color::white/10);
        map.setFg(p.x, p.y, Color::white);
        if(r.onBorder(p))
        {
          map.setBlock(p.x,p.y, true);
          map.setSym(p.x,p.y,"#");
        }
        else {
          map.setSym(p.x,p.y,".");
        }
    }
    map.setMemColors(Color(0.0f,0.0f,0.1f), Color(0.05f,0.0f,0.05f));
    map.setLightSource(10,10,1000, Color::white);
    map.calcFov(px, py, 12);
  }
  void onKeyPress(const KeyboardEvent& ke)
  {
    if(ke.eventType==ketPress) {
      if(ke.keySym==keyboard::GK_LEFT) {
        --px;
      }
      else if(ke.keySym==keyboard::GK_RIGHT) {
        ++px;
      }
      else if(ke.keySym==keyboard::GK_UP) {
        --py;
      }
      else if(ke.keySym==keyboard::GK_DOWN) {
        ++py;
      }
      map.clearVisibility();
      map.calcFov(px, py, 12 );
    }
  }
  void onDraw()
  {
    //con->clear();
    map.draw(con, 0, 0, 0, 0, 40,40);
    con->curFg=Color::white;
    con->printAt({px, py},0, "@");
    con->fillRect({{40,0},{10,10}}, Color::blue);
    con->printCenteredAt({40,0},10, Color::red, Color::black, Console::pfMakeBackgroundDarker, FORMAT("hello %{}", "world"));
  }
  GumMap map;
  Console* con;
  int px=10;
  int py=10;
  float h,s,l;
};

int GliderAppMain(int argc,char* argv[])
{
#ifdef DEBUG
  //dpFile=fopen("zorro.log","wb+");
#endif
  try{
    //╔════╗
    //║    ║
    //╚════╝
    srand((unsigned int)time(nullptr));
    kst::Logger::Init("paparl.log");
    GumMap::cfov.prepare(20);
    engine.setVSync(false);
    int tileWidth=8;
    int tileHeight=15;
    int conWidth=120;
    int conHeight=40;

    engine.selectResolution(tileWidth*conWidth,tileHeight*conHeight,false);
    //engine.selectResolution(800,480,false);
    engine.setResolution();
    engine.setFpsLimit(60);
    ReferenceTmpl<Console> con = new Console();
    con->init("FSEX300.ttf", 16, tileWidth, tileHeight, conWidth,conHeight);
    
    ConTest t;

    t.init(con.get());
    
    engine.assignHandler(con.get());
    engine.loop(con.get());
  }catch(std::exception& e)
  {
    fprintf(stderr,"exception:%s\n",e.what());
    LOGERROR(kst::Logger::getLogger("main"),"exception:%{}",e.what());
  }
  return 0;
}
