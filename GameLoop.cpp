#include "GameLoop.hpp"
#include "Console.hpp"
#include "EventHandler.hpp"
#include "Enemy.hpp"

GameLoop::GameLoop(Console& argCon, ScreensController* argSCon):GameScreen(argCon, argSCon)
{
  player = std::make_shared<Player>();
  timeLine.add(0.0, player);
  turn();
}

void GameLoop::turn()
{
  while(!stopForPlayerTurn && !timeLine.empty())
  {
    auto actor=timeLine.getNext();
    double actCost = actor->makeTurn(this);
    if(actCost>0.0)
    {
      timeLine.add(actCost, actor);
    }
  }
  stopForPlayerTurn = false;
}

void GameLoop::moveActorBy(GameActorPtr ptr, IPos d)
{
  map.getUserInfo(ptr->getPos()).actor.reset();
  ptr->moveBy(d);
  map.getUserInfo(ptr->getPos()).actor=ptr;
}

void GameLoop::onKeyboardEvent(const KeyboardEvent &evt)
{
  if(evt.eventType!=ketPress)
  {
    return;
  }
  using namespace keyboard;
  IPos d;
  switch(evt.keySym)
  {
    case GK_LEFT:
    case GK_KP_4:
      d={-1,0};
      break;
    case GK_RIGHT:
    case GK_KP_6:
      d={1,0};
      break;
    case GK_UP:
    case GK_KP_8:
      d={0,-1};
      break;
    case GK_DOWN:
    case GK_KP_2:
      d={0,1};
      break;
    case GK_KP_7:
      d={-1,-1};
      break;
    case GK_KP_9:
      d={1,-1};
      break;
    case GK_KP_1:
      d={-1,1};
      break;
    case GK_KP_3:
      d={1,1};
      break;
    default:
      break;
  }
  moveActorBy(player, d);
  timeLine.add(1.0, player);
  turn();
}

void GameLoop::playerTurn()
{
  stopForPlayerTurn = true;
  map.clearVisibility();
  auto pp = player->getPos();
  map.calcFov(pp.x, pp.y, 40);
}

void GameLoop::draw()
{
  con.curBg=Color::black;
  con.clear();
  int dx = (con.width()-mapOffsetX)/2;
  int dy = (con.height()-mapOffsetY)/2;
  int mx = player->getPos().x - dx;
  int my = player->getPos().y - dy;
  int mw=con.width()-mapOffsetX;
  int mh=con.height()-mapOffsetY;
  map.draw(&con, mx, my, mapOffsetX, mapOffsetY, mw, mh);
  con.printAt({mapOffsetX+dx,mapOffsetY+dy}, Color::white, {}, Console::pfKeepBackground, "@");
  IRect visRect(mx, my, mw, mh);
  for(auto a:actors)
  {
    if(visRect.isInside(a->getPos()))
    {
      con.printAt({mapOffsetX+a->getPos().x-mx,mapOffsetY+a->getPos().y-my},a->getFg(), {}, Console::pfKeepBackground, a->getSym().c_str());
    }
  }
}
