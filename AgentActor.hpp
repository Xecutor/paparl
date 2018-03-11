#pragma once

#include "GameActor.hpp"
#include "AgentEquipment.hpp"
#include <array>

class AgentActor:public GameActor{
public:
  enum NrgySlot{
    nsAttack,
    nsDefense,
    nsSpeed,
    nsPerception
  };

  void beforeTurn()override;

  float getStamina()const
  {
    return stamina;
  }
  float getAttackBonus()const;
  float getAttack()const;
  float getDefense()const;
  float getSpeed()const override;
  float getPerception()const;

  const std::array<float,4>& getNrgyDistr()const
  {
    return nrgy;
  }

  void changeNrgySlot(NrgySlot ns, float val)
  {
    if(val<0)
    {
      val=-val;
      val=std::min(nrgy[ns], val);
      nrgy[ns]-=val;
      freeNrgy+=val;
    }
    else
    {
      val=std::min(freeNrgy, val);
      nrgy[ns]+=val;
      freeNrgy-=val;
    }
  }

  void takeDamage(GameLoop* game, GameActor* from, float dmg)override
  {
    (void)game;
    (void)from;
    hp-=std::max(0.0f, dmg-getDefense());
  }

  void addEquipment(AgentEquipmentPrt eq)
  {
    equipment.push_back(eq);
    baseSpeed-=0.05f;
  }

  const std::vector<AgentEquipmentPrt>& getEquipment()
  {
    return equipment;
  }

protected:
  float stamina=1.0;
  float baseAttack=0.05f;
  float baseDefense=0.05f;
  float baseSpeed=1.0f;
  float basePerception=0.5f;
  float freeNrgy=5.0f;
  std::array<float, 4> nrgy = { 0.0f, 0.0f, 0.0f, 0.0f };

  std::vector<AgentEquipmentPrt> equipment;

  static float nrgyLevelToStaminaSpent(float lvl);
};
