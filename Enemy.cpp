#include "Enemy.hpp"
#include "GameLoop.hpp"

EnemyBase::EnemyBase(EnemyType argEt):et(argEt)
{
  using ET=EnemyType;
  switch(et)
  {
    case ET::angryGhost:
      name="Angry ghost";
      sym='G';
      fg=Color::gray;
      lightSource=true;
      lightStrength=300;
      lightTint=fg;
      damage=0.1f;
      break;
  }
}

double EnemyBase::makeTurn(GameLoop* game)
{
  if(!seenPlayer)
  {
    if(game->getMap().getVis(pos))
    {
      seenPlayer=true;
    }
    else
    {
      return 1.0;
    }
  }
  auto player = game->getPlayer();
  IPos pp=player->getPos();
  if(hp>0.5f)
  {
    if(game->getMap().findPath(pos, pp))
    {
      IPos* path;
      if(game->getMap().astar.getSolutionStart() && (path=game->getMap().astar.getSolutionNext()))
      {
        IPos d=*path-pos;
        int wc=game->canActorMoveTo(*this, d);
        if(wc>0)
        {
          return game->moveActorBy(this->shared_from_this(), d);
        }
        else if(wc<0)
        {
          GameActorPtr actor = game->getMap().getUserInfo(pos+d).actor;
          if(actor && !actor->isEnemy())
          {
            actor->takeDamage(game, this, 0.1f);
          }
        }
      }
    }
  }
  else
  {
    IPos d=pos-pp;
    normalizeDir(d);
    int wc=game->canActorMoveTo(*this, d);
    if(wc>0)
    {
      return game->moveActorBy(this->shared_from_this(), d);
    }
  }
  return 1.0;
}

float EnemyBase::getSpeed()const
{
  return 1.5;
}
