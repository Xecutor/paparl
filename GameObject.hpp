#pragma once

#include <memory>
#include <string>

#include "Types.hpp"

class GameObject {
public:
  IPos getPos()const
  {
    return pos;
  }
  void setPos(IPos argPos)
  {
    pos=argPos;
  }
  void moveBy(IPos d)
  {
    pos+=d;
  }
  const std::string& getSym()const
  {
    return sym;
  }
  bool getLightSource()const
  {
    return lightSource;
  }
  uint16_t getLightStrength()const
  {
    return lightStrength;
  }
  const Color& getLightTint()const
  {
    return lightTint;
  }
  const Color getFg()const
  {
    return fg;
  }
  const Color getBg()const
  {
    return fg;
  }
  const std::string& getName()const
  {
    return name;
  }

protected:
  IPos pos;
  std::string name;
  std::string sym;
  Color fg;
  Color bg;
  bool lightSource = false;
  uint16_t lightStrength = 0;
  Color lightTint;

};


using GameObjectPtr = std::shared_ptr<GameObject>;
