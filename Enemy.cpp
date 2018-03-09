#include "Enemy.hpp"
#include "GameLoop.hpp"

EnemyBase::EnemyBase()
{
  sym='X';
  fg=Color::red;
}

double EnemyBase::makeTurn(GameLoop* game)
{
  game->moveActorBy(this->shared_from_this(), IPos(1,1));
  return 1.0;
}
