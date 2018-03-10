#pragma once

#include <list>

#include "GameScreen.hpp"
#include "TimeLine.hpp"
#include "GameActor.hpp"
#include "GameItem.hpp"
#include "GameTile.hpp"
#include "GumMap.hpp"
#include "GameWorldObject.hpp"
#include "Player.hpp"

struct TileObjects{
  GameActorPtr actor;
  std::list<GameItemPtr> items;
  GameWorldObjectPtr worldObj;
  const GameTile* tile;
};

using GameMap = GumMap<TileObjects>;

class GameLoop: public GameScreen {
public:
  GameLoop(Console& argCon, ScreensController* argSCon);
  void turn();
  void draw();
  void playerTurn();

  virtual void drawPanel()=0;

  //>0 move cost
  //==0 cannot move
  //-1 occupied by another actor
  int canActorMoveTo(GameActor& ptr, IPos d);
  float moveActorBy(GameActorPtr ptr, IPos d);


  void addActor(IPos pos, float actDelay, GameActorPtr actor);

  PlayerPtr getPlayer()
  {
    return player;
  }

  GameMap& getMap()
  {
    return map;
  }

  IRect getMapArea()const
  {
    return {{mapOffsetX,mapOffsetY}, {con.width()-mapOffsetX, con.height()-mapOffsetY}};
  }

protected:
  const int mapOffsetX=40;
  const int mapOffsetY=0;
  TimeLine timeLine;
  GameMap map;
  PlayerPtr player;
  bool stopForPlayerTurn = false;
  std::vector<GameActorPtr> actors;

  void addActorToMap(GameActorPtr actor);
  void removeActorFromMap(GameActorPtr actor);

};
