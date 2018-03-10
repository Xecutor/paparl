#pragma once

#include "GameActor.hpp"

enum class EnemyType{
  angryGhost,
};

class EnemyBase : public GameActor{
public:
  EnemyBase(EnemyType agrEt);
  double makeTurn(GameLoop* game)override;
  float getSpeed()const override;
  void takeDamage(GameLoop* game, GameActor* from, float dmg)override
  {
    (void)game;
    (void)from;
    hp-=dmg;
  }
  bool isEnemy()const override
  {
    return true;
  }

protected:
  EnemyType et;
  float damage=0.1f;
  bool seenPlayer = false;
};
