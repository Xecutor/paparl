#pragma once

#include "GameObject.hpp"

class GameWorldObject: public GameObject{
public:
protected:
};


using GameWorldObjectPtr = std::shared_ptr<GameWorldObject>();
