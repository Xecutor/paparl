#pragma once

#include "GameObject.hpp"

class GameLoop;

class GameActor : public GameObject, public std::enable_shared_from_this<GameActor>{
public:
  virtual void beforeTurn(){};
  virtual double makeTurn(GameLoop* game) = 0;
  virtual float getSpeed()const =0;
  float getHp()const
  {
    return hp;
  }
  virtual void takeDamage(GameLoop* game, GameActor* source, float dmg)=0;
  virtual bool isEnemy()const
  {
    return false;
  }
protected:
  float hp=1.0f;
};

using GameActorPtr = std::shared_ptr<GameActor>;
