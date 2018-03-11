#include <stdio.h>

#include "Engine.hpp"
#include "ResourcesManager.hpp"
#include "kst/Logger.hpp"
#include "Console.hpp"
#include "GumMap.hpp"
#include "Menu.hpp"
#include "Enemy.hpp"
#include "MissionControl.hpp"
#include "TextMessage.hpp"

using namespace glider;

class MainMenu:public Menu{
public:
  MainMenu(Console& argCon, ScreensController* argScon):Menu(argCon, argScon, {})
  {
    setAutoClose(false);
  }
  void init()override
  {
    const char* menuCaption=MissionControl::saveExists()?"Continue game":"Start game";
    clear();
    add(menuCaption,[this](){
      scon->pushScreen<MissionControl>();
    });
    add("Quit",[this](){
      close();
    });
  }
  void onTopScreen()override
  {
    init();
  }
  void draw()override
  {
    con.curBg=Color::black;
    con.clear();
    con.fillRect(IRect(0,0,con.width(),1), Color::gray);
    con.curFg=Color::white;
    con.printAlignedAt({0,0}, con.width(), Console::pfAlignCenter|Console::pfKeepBackground, "P.A.P.A. roguelike");
    con.curBg=Color::black;
    con.curFg=Color::gray/3;
    con.printAlignedAt({0,5}, con.width(), Console::pfAlignCenter, "Paronormal Activities Prevention Agency");
    con.printAt({0, con.height()-1}, Color::white, Color::black, 0, "Created for 7drl 2018 by Konstantin 'Xecutor' Stupnik");
    Menu::draw();
  }
protected:
};

int GliderAppMain(int argc,char* argv[])
{
  (void)argc;
  (void)argv;
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
//    int tileWidth=13;
//    int tileHeight=15;
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
