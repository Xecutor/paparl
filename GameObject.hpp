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
  const Color getFg()const
  {
    return fg;
  }
  const Color getBg()const
  {
    return fg;
  }
protected:
  IPos pos;
  std::string sym;
  Color fg;
  Color bg;
};


using GameObjectPtr = std::shared_ptr<GameObject>;
