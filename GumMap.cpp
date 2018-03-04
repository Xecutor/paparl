#include "GumMap.hpp"

CircularFov<IPos> GumMap::cfov;
GumMap::MyAStar GumMap::astar;

bool GumMap::findPath(int x0,int y0,int x1,int y1)
{
  astar.assignMap(this);
  astar.setStartAndGoalStates(IPos(x0,y0),IPos(x1,y1));
  MyAStar::AStarSearchState st;
  while((st=astar.searchStep())==MyAStar::SEARCH_STATE_SEARCHING)
  {
  }
  return st==MyAStar::SEARCH_STATE_SUCCEEDED;
}
