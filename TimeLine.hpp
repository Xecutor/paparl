#pragma once

#include <map>
#include "GameObject.hpp"

class TimeLine{
public:
  add(double offset, GameObjectPtr obj)
  {
    timeLine.emplace(currentTime + offset, obj);
  }
  bool empty()
  {
    return timeLine.empty();
  }
  GameObjectPtr getNext()
  {
    if(timeLine.empty())
    {
      return {};
    }
    auto rv = timeLine.begin()->second;
    timeLine.erase(timeLine.begin());
    return rv;
  }
protected:
  double currentTime;
  std::multimap<double, GameObjectPtr> timeLine;
};
