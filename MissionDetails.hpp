#pragma once

#include "Enemy.hpp"
#include "GeneratorType.hpp"
#include "DifficultyLevel.hpp"

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
  struct EnemyGroup{
    EnemyType et;
    bool scattered;
    int count;
  };

  uint32_t seed;
  DifficultyLevel dl=DifficultyLevel::easy;
  GeneratorType gt=GeneratorType::downtown;
  MissionTime mt;
  std::vector<EnemyGroup> enemies;
  int rp;
};

struct MissionResultInfo{
  enum Result{
    success,
    escaped,
    evacuated,
    kia,
  };
  Result result;
};
