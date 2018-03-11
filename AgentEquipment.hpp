#pragma once

#include <string>
#include <memory>
#include <vector>

enum class AgentEquipmentType{
  ectoplasmicGun,
  silverSword,
  stunGrenade,
};

std::string getEquipmentNameByType(AgentEquipmentType aet);

inline std::vector<AgentEquipmentType> getAllEquipmentTypes()
{
  using AET=AgentEquipmentType;
  return {
    AET::ectoplasmicGun,
    AET::silverSword,
    AET::stunGrenade,
  };
}

class CombatScreen;
class EnemyBase;

class AgentEquipment{
public:
  virtual ~AgentEquipment(){}

  virtual void fireThis(CombatScreen*){};
  virtual void throwThis(CombatScreen*){};
  virtual void useThis(CombatScreen*){};

  virtual float onMeleeAttack(EnemyBase*, float baseDmg)
  {
    return baseDmg;
  }

  std::string getName()
  {
    return getEquipmentNameByType(aet);
  }

  AgentEquipmentType aet;

  int count=0;

  bool canBeFired=false;
  bool canBeThrown=false;
  bool canBeUsed=false;
protected:
};

using AgentEquipmentPrt = std::shared_ptr<AgentEquipment>;

AgentEquipmentPrt createAgentEquipment(AgentEquipmentType aet);
