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
  void onKeyboardEvent(const KeyboardEvent &evt);
  void playerTurn();

  void moveActorBy(GameActorPtr ptr, IPos d);

protected:
  const int mapOffsetX=40;
  const int mapOffsetY=0;
  TimeLine timeLine;
  GameMap map;
  PlayerPtr player;
  bool stopForPlayerTurn = false;
  std::vector<GameActorPtr> actors;
};
