#pragma once

#include <map>
#include "GameActor.hpp"

class TimeLine{
public:
  void add(double offset, GameActorPtr obj)
  {
    timeLine.emplace(currentTime + offset, obj);
  }
  bool empty()
  {
    return timeLine.empty();
  }
  GameActorPtr getNext()
  {
    if(timeLine.empty())
    {
      return {};
    }
    auto it = timeLine.begin();
    currentTime=it->first;
    auto rv = it->second;
    timeLine.erase(it);
    return rv;
  }
protected:
  double currentTime;
  std::multimap<double, GameActorPtr> timeLine;
};
