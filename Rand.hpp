#pragma once

#include <random>
#include "Types.hpp"

using RandGen=std::mt19937;

inline int intRand(RandGen& rnd, int minVal, int maxValEx)
{
  double v = rnd();
  v-=rnd.min();
  v/=((double)rnd.max())+1.0;
  v*=(maxValEx-minVal);
  v+=minVal;
  return (int)v;
}

inline bool boolRand(RandGen& rnd)
{
  auto v = rnd();
  v-=rnd.min();
  return v>=(rnd.max()-rnd.min())/2;
}

inline double dblRand(RandGen& rnd)
{
  double v = rnd();
  v-=rnd.min();
  v/=((double)rnd.max())+1.0;
  return v;
}

inline bool chanceRand(RandGen& rnd, double chance)
{
  return dblRand(rnd)<chance;
}

inline IPos rectRand(RandGen& rnd, IRect rect)
{
    return {intRand(rnd, rect.pos.x, rect.pos.x+rect.size.x), intRand(rnd, rect.pos.y, rect.pos.y+rect.size.y)};
}

template <class T>
inline const T& vectorRand(RandGen& rnd,const std::vector<T>& v)
{
  return v[intRand(rnd, 0, v.size())];
}
