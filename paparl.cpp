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
#include "Menu.hpp"
#include "CombatScreen.hpp"

using namespace glider;

class MainMenu:public Menu{
public:
  MainMenu(Console& argCon, ScreensController* argScon):Menu(argCon, argScon, {})
  {
    setAutoClose(false);
    add("Start Game",[this](){
      MissionDetails md;
      md.mt=MissionTime::day;
      scon->pushScreen<CombatScreen>(md);
    });
    add("Quit",[this](){
      close();
    });
  }
  void draw()override
  {
    con.curBg=Color::black;
    con.clear();
    Menu::draw();
  }
protected:
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
    GumMap<int>::cfov.prepare(20);
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
    //con->init("Topaz-8.ttf", 14, tileWidth, tileHeight, conWidth,conHeight);

    ScreensController scon(*con);
    scon.pushScreen<MainMenu>();
    
    engine.assignHandler(con.get());
    engine.loop(con.get());
  }catch(std::exception& e)
  {
    fprintf(stderr,"exception:%s\n",e.what());
    LOGERROR(kst::Logger::getLogger("main"),"exception:%{}",e.what());
  }
  return 0;
}
