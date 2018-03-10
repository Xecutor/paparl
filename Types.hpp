#pragma once
#include "Utility.hpp"
#include "Keyboard.hpp"

using namespace glider;
using IPos = Posi<int>;
using IRect = Recti<int>;
using ILine = Linei<int>;

enum class Dir{
  left,
  upLeft,
  up,
  upRight,
  right,
  downRight,
  down,
  downLeft,
  none
};

inline IPos getDir(Dir d)
{
  static IPos dirs[9] = {
    {-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{0,0}
  };
  return dirs[(int)d];
}

inline Dir getDir4FromInt(int idx)
{
  switch(idx)
  {
    case 0:return Dir::left;
    case 1:return Dir::up;
    case 2:return Dir::right;
    case 3:return Dir::down;
  }
  return Dir::none;
}

inline Dir rotateDirCw(Dir d)
{
  return (Dir)(((int)d+1)%8);
}

inline Dir rotateDirCCw(Dir d)
{
  return (Dir)(((int)d-1+8)%8);
}

inline bool getDirForKey(keyboard::KeySymbol key, IPos& dir, Dir& d)
{
  using namespace keyboard;
  switch(key)
  {
    case GK_LEFT:
    case GK_KP_4:
      d=Dir::left;
      break;
    case GK_RIGHT:
    case GK_KP_6:
      d=Dir::right;
      break;
    case GK_UP:
    case GK_KP_8:
      d=Dir::up;
      break;
    case GK_DOWN:
    case GK_KP_2:
      d=Dir::down;
      break;
    case GK_KP_7:
      d=Dir::upLeft;
      break;
    case GK_KP_9:
      d=Dir::upRight;
      break;
    case GK_KP_1:
      d=Dir::downLeft;
      break;
    case GK_KP_3:
      d=Dir::downRight;
      break;
    default:
      return false;
  }
  dir=getDir(d);
  return true;
}

inline ILine getRectSide(const IRect& rect, Dir dir)
{
  switch(dir)
  {
    case Dir::up:return {rect.tle(), rect.tre()};
    case Dir::right:return {rect.tre(), rect.bre()};
    case Dir::down:return {rect.bre(), rect.ble()};
    case Dir::left:return {rect.ble(), rect.tle()};
    default:break;
  }
  return {{},{}};
}

inline Dir normalizeDir(IPos& dir)
{
  Dir rv;
  if(dir.x<0)
  {
    if(dir.y<0)
    {
      if(-dir.x>-dir.y*2)
      {
        rv=Dir::left;
      }
      else
      {
        if(-dir.y>-dir.x*2)
        {
          rv=Dir::up;
        }
        else
        {
          rv=Dir::upLeft;
        }
      }
    }
    else
    {
      if(-dir.x>dir.y*2)
      {
        rv=Dir::left;
      }
      else
      {
        if(dir.y>-dir.x*2)
        {
          rv=Dir::down;
        }
        else
        {
          rv=Dir::downLeft;
        }
      }
    }
  }
  else
  {
    if(dir.y<0)
    {
      if(dir.x>-dir.y*2)
      {
        rv=Dir::right;
      }
      else
      {
        if(-dir.y>dir.x*2)
        {
          rv=Dir::up;
        }
        else
        {
          rv=Dir::upRight;
        }
      }
    }
    else
    {
      if(dir.x>dir.y*2)
      {
        rv=Dir::right;
      }
      else
      {
        if(dir.y>dir.x*2)
        {
          rv=Dir::down;
        }
        else
        {
          rv=Dir::downRight;
        }
      }
    }
  }
  dir=getDir(rv);
  return rv;
}
