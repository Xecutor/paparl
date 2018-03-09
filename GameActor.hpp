#pragma once

#include "GameObject.hpp"

class GameLoop;

class GameActor : public GameObject, public std::enable_shared_from_this<GameActor>{
public:
  virtual double makeTurn(GameLoop* game) = 0;
protected:
};

using GameActorPtr = std::shared_ptr<GameActor>;
