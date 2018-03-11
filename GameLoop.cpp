#include "GameLoop.hpp"
#include "Console.hpp"
#include "EventHandler.hpp"
#include "Enemy.hpp"
#include "MapGenerators.hpp"

GameLoop::GameLoop(Console& argCon, ScreensController* argSCon):GameScreen(argCon, argSCon)
{
  player = std::make_shared<Player>();
}

void GameLoop::addActor(IPos pos, float actDelay, GameActorPtr actor)
{
  timeLine.add(actDelay, actor);
  actor->setPos(pos);
  addActorToMap(actor);
}

IPos GameLoop::mapToScreen(IPos p)
{
  int dx = (con.width()-mapOffsetX)/2;
  int dy = (con.height()-mapOffsetY)/2;
  int mx = player->getPos().x - dx;
  int my = player->getPos().y - dy;
  p-=IPos(mx,my);
  p+=IPos(mapOffsetX, mapOffsetY);
  return p;
}

void GameLoop::addActorToMap(GameActorPtr actor)
{
  map.getUserInfo(actor->getPos()).actor=actor;
  if(!actor->isHidden(player->getPerception()))
  {
    map.setInfo(actor->getPos(), actor->getFg(), map.getBg(actor->getPos()), actor->getSym());
    if(actor->getLightSource())
    {
      map.setLightSource(actor->getPos(), actor->getLightStrength(), actor->getLightTint());
    }
  }
}

void GameLoop::removeActorFromMap(GameActorPtr actor)
{
  map.getUserInfo(actor->getPos()).actor.reset();
  fillTileInfo(map, actor->getPos(), *map.getUserInfo(actor->getPos()).tile);
}


void GameLoop::turn()
{
  while(!stopForPlayerTurn && !timeLine.empty())
  {
    auto actor=timeLine.getNext();
    if(actor->getHp()<=0.0f)
    {
      removeActorFromMap(actor);
      LOGINFO("gameloop", "actor %{} died at %{}", actor->getName(), actor->getPos());
      if(actor==player)
      {
        playerTurn();
        playerDied();
      }
      continue;
    }
    LOGDEBUG("gameloop", "%{} making turn at %{}", actor->getName(), (int)(100.0*timeLine.getCurrentTime()));
    actor->beforeTurn();
    double actCost = actor->makeTurn(this);
    if(actCost>0.0)
    {
      timeLine.add(actCost, actor);
    }
  }
  afterTurn();
  stopForPlayerTurn = false;
}

int GameLoop::canActorMoveTo(GameActor& act, IPos d)
{
  TileObjects& to=map.getUserInfo(act.getPos()+d);
  if(to.actor)
  {
    return -1;
  }
  return to.tile->walkCost;
}

float GameLoop::moveActorBy(GameActorPtr ptr, IPos d)
{
  removeActorFromMap(ptr);
  ptr->moveBy(d);
  addActorToMap(ptr);
  TileObjects& to=map.getUserInfo(ptr->getPos());
  int wc=to.tile->walkCost;
  float moveCost = wc/100.0f;
  moveCost /= ptr->getSpeed();
  return moveCost;
}

void GameLoop::playerTurn()
{
  stopForPlayerTurn = true;
  if(player->getLightSource())
  {
    map.setLightSource(player->getPos(), player->getLightStrength(), player->getLightTint());
  }
  map.clearVisibility();
  auto pp = player->getPos();
  map.calcFov(pp.x, pp.y, 20);
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
  //con.printAt({mapOffsetX+dx,mapOffsetY+dy}, Color::white, {}, Console::pfKeepBackground, "@");
  drawPanel();
  if(runMode)
  {
    if(canActorMoveTo(*player, runDir))
    {
      timeLine.add(moveActorBy(player, runDir), player);
      turn();
    }
  }
}
