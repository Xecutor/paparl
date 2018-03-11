#include "AgentEquipment.hpp"
#include "CombatScreen.hpp"

std::string getEquipmentNameByType(AgentEquipmentType aet)
{
  using AET=AgentEquipmentType;
  switch(aet)
  {
    case AET::ectoplasmicGun:return "Ectoplasmic gun";
    case AET::silverSword:return "Silver sword";
    case AET::stunGrenade:return "Stun grenade";
  }
  return {};
}

struct EctoplasmicGunShotAni:CombatScreen::Animation{
  EctoplasmicGunShotAni(CombatScreen* argGame,IPos argSrc, IPos argDst):game(argGame),src(argSrc), dst(argDst){}
  CombatScreen* game;
  IPos src,dst;
  Color clr=0x00e9ff;
  int stepsLeft=10;
  void step()override
  {
    auto& map=game->getMap();
    for(auto p:ILine(src,dst))
    {
      if(stepsLeft==1)
      {
        game->restoreMapAt(p);
      }
      else
      {
        map.setInfo(p, clr, clr, " ");
        map.setLightSource(p, stepsLeft*40, clr);
      }
    }
    clr.changeValue(0.9f);
    --stepsLeft;
  }
  bool hasFinished()const override
  {
    return stepsLeft==0;
  }
};

class EctoplasmicGun:public AgentEquipment{
public:
  EctoplasmicGun()
  {
    canBeFired=true;
    count=5;
  }
  void fireThis(CombatScreen* game)
  {
    --count;
    IPos src=game->getPlayer()->getPos();
    IPos dst=game->getTargetPos();

    auto& map=game->getMap();
    for(auto p:ILine(src,dst))
    {
      auto& to=map.getUserInfo(p);
      if(to.actor && to.actor->isEnemy())
      {
        float dmg=0.2f*game->getPlayer()->getAttackBonus();
        EnemyBase* enemy=dynamic_cast<EnemyBase*>(to.actor.get());
        if(enemy->getEnemyClass()==EnemyClass::ghost)
        {
          dmg*=1.5;
        }
        enemy->takeDamage(game, game->getPlayer().get(), dmg);
      }
    }

    game->addAnimation(std::make_shared<EctoplasmicGunShotAni>(game, src, dst));

  }
};

class SilverSword:public AgentEquipment{
public:
  SilverSword()
  {
  }
  float onMeleeAttack(EnemyBase* enemy, float baseDmg)
  {
    if(enemy->getEnemyClass()==EnemyClass::vampire || enemy->getEnemyClass()==EnemyClass::daemon)
    {
      return baseDmg*3.0f;
    }
    return baseDmg;
  }
};

struct ThrowAnimation:CombatScreen::Animation{
  ThrowAnimation(CombatScreen* argGame, const std::string& argSym, std::function<void()> argOnAniEnd):
    game(argGame),sym(argSym),onAniEnd(argOnAniEnd),throwLine(game->getPlayer()->getPos(), game->getTargetPos()),
    pos(throwLine.begin())
  {
  }
  void step()
  {
    IPos p=*pos;
    auto& map=game->getMap();
    if(!firstStep)
    {
      game->restoreMapAt(prevPos);
    }
    else
    {
      firstStep=false;
    }
    map.setInfo(p, Color::white, map.getBg(p), sym);
    prevPos=p;
    ++pos;
    if(pos==throwLine.end())
    {
      onAniEnd();
    }
  }
  bool hasFinished()const
  {
    return pos==throwLine.end();
  }
  CombatScreen* game;
  std::string sym;
  std::function<void()> onAniEnd;
  bool firstStep=true;
  IPos prevPos;
  ILine throwLine;
  ILine::Iterator pos;
};

struct ExplosionAni:CombatScreen::Animation{
  ExplosionAni(CombatScreen* argGame):game(argGame){}

  void step()override
  {
    int r=5;
    for(auto p:IRect(IPos(-r,-r), {r*2+1,r*2+1}))
    {
      if(p.x*p.x+p.y*p.y>r*r)
      {
        continue;
      }
      p+=game->getTargetPos();
      if(aniCount==1)
      {
        game->restoreMapAt(p);
      }
      else
      {
        if((aniCount/5)%2)
        {
          game->getMap().setInfo(p, Color::black, Color::black, " ");
          game->getMap().setLightSource(p, 0, Color::black);
        }
        else
        {
          game->getMap().setInfo(p, Color::white*0.75f, Color::white*0.75f, " ");
          game->getMap().setLightSource(p, 500, Color::white);
        }
      }
    }
    --aniCount;
  }
  bool hasFinished()const override
  {
    return aniCount==0;
  }
  CombatScreen* game;
  int aniCount=30;
};

class StunGrenade:public AgentEquipment{
public:
  StunGrenade()
  {
    canBeThrown=true;
    count=4;
  }
  void throwThis(CombatScreen* game)override
  {
    --count;
    game->addAnimation(std::make_shared<ThrowAnimation>(game, "*", [this,game](){startExplosion(game);}));
  };
  void startExplosion(CombatScreen* game)
  {
    game->addAnimation(std::make_shared<ExplosionAni>(game));
    int r=5;
    for(auto p:IRect(IPos(-r,-r), {r*2+1,r*2+1}))
    {
      if(p.x*p.x+p.y*p.y>r*r)
      {
        continue;
      }
      p+=game->getTargetPos();
      auto& to=game->getMap().getUserInfo(p);
      if(to.actor && to.actor->isEnemy())
      {
        EnemyBase* enemy=dynamic_cast<EnemyBase*>(to.actor.get());
        if(enemy)
        {
          enemy->setStunCount(8);
        }
      }
    }
  }
};

AgentEquipmentPrt createAgentEquipment(AgentEquipmentType aet)
{
  using AET=AgentEquipmentType;
  AgentEquipmentPrt rv;
  switch(aet)
  {
    case AET::ectoplasmicGun:rv=std::make_shared<EctoplasmicGun>();break;
    case AET::silverSword:rv=std::make_shared<SilverSword>();break;
    case AET::stunGrenade:rv=std::make_shared<StunGrenade>();break;
  }
  rv->aet=aet;
  return rv;
}
