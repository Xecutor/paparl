#include "Enemy.hpp"
#include "GameLoop.hpp"
#include "Rand.hpp"

EnemyBase::EnemyBase(EnemyType argEt):eType(argEt)
{
  using ET=EnemyType;
  name=getEnemyNameForType(eType);
  switch(eType)
  {
    case ET::angryGhost:
      sym='G';
      eClass=EnemyClass::ghost;
      eBehaviour=EnemyBehaviour::attackWhenDamagedRetreat;
      fg=Color::gray;
      lightSource=true;
      lightStrength=300;
      lightTint=fg;
      damage=0.1f;
      break;
    case ET::shyGhost:
      sym='G';
      eClass=EnemyClass::ghost;
      eBehaviour=EnemyBehaviour::runsAway;
      fg=Color::gray;
      lightSource=true;
      lightStrength=300;
      lightTint=fg;
      damage=0.1f;
      break;
    case ET::poltergeist:
      sym='P';
      eClass=EnemyClass::poltergeist;
      eBehaviour=EnemyBehaviour::attackAgressively;
      eFeatures=efInitiallyHidden;
      hidden=true;
      hideLevel=1.5f;
      fg=Color::blue;
      lightStrength=0;
      damage=0.1f;
      break;
    case ET::vampire:
      sym='V';
      eBehaviour=EnemyBehaviour::attackAgressively;
      eFeatures=efRegenerating;
      fg=Color::red;
      lightStrength=0;
      damage=0.2f;
      break;
    case ET::daemon:
      sym='D';
      eBehaviour=EnemyBehaviour::attackAgressively;
      fg=0x751aa5;
      lightStrength=0;
      damage=0.3f;
      break;
  }
}

double EnemyBase::makeTurn(GameLoop* game)
{
  if(eFeatures&efRegenerating)
  {
    hp+=0.01f;
    if(hp>1.0f)
    {
      hp=1.0f;
    }
  }
  if(stunCount)
  {
    --stunCount;
    return 1.0;
  }
  if(hidden)
  {
    return 1.0;
  }
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
  if(eBehaviour==EnemyBehaviour::attackAgressively || (eBehaviour==EnemyBehaviour::attackWhenDamagedRetreat && hp>0.5f))
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
    else
    {
      d=getDir((Dir)intRand(rnd, 0, 8));
      wc=game->canActorMoveTo(*this, d);
      if(wc>0)
      {
        return game->moveActorBy(this->shared_from_this(), d);
      }
    }
  }
  return 1.0;
}

float EnemyBase::getSpeed()const
{
  return 1.5;
}

void EnemyBase::takeDamage(GameLoop* game, GameActor* from, float dmg)
{
  (void)game;
  (void)from;
  hp-=dmg;
  if(hidden)
  {
    hidden=false;
  }
}

bool EnemyBase::isHidden(float prcLevel) const
{
  return hidden && hideLevel>prcLevel;
}

std::string getEnemyNameForType(EnemyType et)
{
  using ET=EnemyType;
  switch(et)
  {
    case ET::angryGhost:return "Angry ghost";
    case ET::shyGhost:return "Shy ghost";
    case ET::poltergeist:return "Poltergeist";
    case ET::vampire:return "Vampire";
    case ET::daemon:return "Daemon";
  }
  return {};
}

EnemyType getRandomEnemyForDifficulty(RandGen& rnd,DifficultyLevel dl)
{
  using ET=EnemyType;
  switch(dl)
  {
    case DifficultyLevel::easy:
    {
      static std::vector<EnemyType> easyMonsters={ET::shyGhost, ET::angryGhost};
      return vectorRand(rnd, easyMonsters);
    }
    case DifficultyLevel::average:
    {
      static std::vector<EnemyType> avgMonsters={ET::poltergeist, ET::angryGhost};
      return vectorRand(rnd, avgMonsters);
    }
    case DifficultyLevel::hard:
    {
      static std::vector<EnemyType> hardMonsters={ET::vampire};
      return vectorRand(rnd, hardMonsters);
    }
    case DifficultyLevel::epic:
    {
      static std::vector<EnemyType> epicMonsters={ET::daemon};
      return vectorRand(rnd,epicMonsters);
    }
  }
  return EnemyType::angryGhost;
}
