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
  double getCurrentTime()const
  {
    return currentTime;
  }
  std::vector<GameActorPtr> getEnemies()const
  {
    std::vector<GameActorPtr> rv;
    for(auto p:timeLine)
    {
      if(p.second->isEnemy())
      {
        rv.push_back(p.second);
      }
    }
    return rv;
  }
protected:
  double currentTime = 0.0;
  std::multimap<double, GameActorPtr> timeLine;
};
