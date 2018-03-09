#include "CombatScreen.hpp"
#include "MapGenerators.hpp"
#include "MissonDetails.hpp"
#include "Enemy.hpp"

void CombatScreen::init()
{
  if(md.mt==MissionTime::day)
  {
    map.setGlobalLight(800, 0x2d7596);
  }
  else if(md.mt==MissionTime::night)
  {
    map.setGlobalLight(0, 0);
  }
  else if(md.mt==MissionTime::evening)
  {
    map.setGlobalLight(200, 0x453370);
  }
  auto ent=generateLevel(GeneratorType::downtown, 0, md, map);
  map.setMemColors(Color(0.0f,0.0f,0.15f), Color(0.05f,0.0f,0.05f));
  player->setPos(ent);
  actors.push_back(std::make_shared<EnemyBase>());
  actors.back()->setPos(ent-IPos(3,3));
  timeLine.add(1.0, actors.back());
  playerTurn();
}
