#pragma once

#include "GameActor.hpp"

class Player:public GameActor{
public:
    Player();
    double makeTurn(GameLoop* game);
protected:
};

using PlayerPtr = std::shared_ptr<Player>;
