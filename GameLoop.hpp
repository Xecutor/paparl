#pragma once

#include "TimeLine.hpp"
#include "GameActor.hpp"

class GameLoop{
public:
protected:
  TimeLine timeLine;
  struct TileObj{
    GameActorPtr actor;
    std::list<GameItemPtr> items;
    const GameTile* tile;
  };
  GumMap<TileObj> map;
}
