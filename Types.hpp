#pragma once
#include "Utility.hpp"

using namespace glider;
using IPos = Posi<int>;
using IRect = Recti<int>;

enum class Dir{
  left,
  upLeft,
  up,
  upRight,
  right,
  downRight,
  down,
  downLeft,
};

inline IPos getDir(Dir d)
{
  static IPos dirs[8] = {
    {-1,0},{-1,-1},{0,-1},{1,-1},{0,1},{1,1},{0,1},{-1,1}
  };
  return dirs[(int)d];
}

inline Dir rotateDirCw(Dir d)
{
  return (Dir)(((int)d+1)%8);
}

inline Dir rotateDirCCw(Dir d)
{
  return (Dir)(((int)d-1+8)%8);
}
