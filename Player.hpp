#pragma once

#include "AgentActor.hpp"

class Player:public AgentActor{
public:
  Player();
  double makeTurn(GameLoop* game);
  void toggleFlashLight()
  {
    if(lightStrength==300)
    {
      lightStrength=1000;
    }
    else
    {
      lightStrength=300;
    }
  }
protected:
};

using PlayerPtr = std::shared_ptr<Player>;
