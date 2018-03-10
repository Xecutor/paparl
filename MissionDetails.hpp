#pragma once

#include "Enemy.hpp"

enum class MissionTime{
  day, night, evening
};

enum class MissionDeviations{
  blackout,
};

class MissionDetails{
public:
  struct EnemyPosition{
    EnemyType et;
    IPos pos;
  };

  MissionTime mt;
  struct EnemyGroup{
    EnemyType et;
    bool scattered;
    int count;
  };
  std::vector<EnemyGroup> enemies;
};
