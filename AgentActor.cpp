#include "AgentActor.hpp"
#include <numeric>

void AgentActor::beforeTurn()
{
  if(std::accumulate(std::begin(nrgy), std::end(nrgy), 0.0f)==0.0f)
  {
    stamina+=1.0f/50.0f;
    if(stamina>1.0f)
    {
      stamina=1.0f;
    }
  }
  for(int i=0;i<4;++i)
  {
    stamina-=nrgyLevelToStaminaSpent(nrgy[i]);
  }
  if(stamina<=0.0f)
  {
    stamina=0.0f;
    for(int i=0;i<4;++i)
    {
      nrgy[i]=0.0f;
    }
    freeNrgy=5.0f;
  }
}

float AgentActor::getAttackBonus() const
{
  return (1.0f+nrgy[nsAttack]);
}

float AgentActor::getAttack()const
{
  return baseAttack*getAttackBonus();
}

float AgentActor::getDefense()const
{
  return baseDefense*(1.0f+nrgy[nsDefense]);
}

float AgentActor::getSpeed()const
{
  return baseSpeed*(1.0f+nrgy[nsSpeed]);
}

float AgentActor::getPerception()const
{
  return basePerception*(1.0f+nrgy[nsPerception]);
}

float AgentActor::nrgyLevelToStaminaSpent(float lvl)
{
  return ((1.0f+lvl)*(1.0f+lvl)-1.0f)/(10.0f*33.0f);
}
