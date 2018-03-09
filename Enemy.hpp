#pragma once

#include "GameActor.hpp"

class EnemyBase : public GameActor{
public:
  EnemyBase();
  double makeTurn(GameLoop* game)override;
protected:
};
