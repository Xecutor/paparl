#pragma once

#include "GameActor.hpp"
#include "DifficultyLevel.hpp"
#include "Rand.hpp"

enum class EnemyType{
  angryGhost,
  shyGhost,
  poltergeist,
  daemon,
  vampire
};

enum class EnemyClass{
  ghost,
  poltergeist,
  daemon,
  vampire,
};

enum class EnemyBehaviour{
  attackWhenDamagedRetreat,
  attackAgressively,
  runsAway,
};

enum EnemyFeatures:uint32_t{
  efChasesPlayer             = 0x01,
  efInitiallyHidden          = 0x02,
  efExplodesWhenDamaged      = 0x04,
  efShielded                 = 0x08,
  efRegenerating             = 0x10,
};

std::string getEnemyNameForType(EnemyType et);
EnemyType getRandomEnemyForDifficulty(RandGen& gen, DifficultyLevel dl);

class EnemyBase : public GameActor{
public:
  EnemyBase(EnemyType agrEt);
  double makeTurn(GameLoop* game)override;
  float getSpeed()const override;
  void takeDamage(GameLoop* game, GameActor* from, float dmg)override;
  bool isEnemy()const override
  {
    return true;
  }

  EnemyClass getEnemyClass()
  {
    return eClass;
  }

  void setStunCount(int argStunCount)
  {
    stunCount=argStunCount;
  }

  bool isHidden(float prcLevel)const override;

protected:
  EnemyType eType;
  EnemyClass eClass;
  uint32_t eFeatures = 0;
  EnemyBehaviour eBehaviour = EnemyBehaviour::attackAgressively;
  float damage=0.1f;
  float shield=0.0f;
  bool seenPlayer = false;
  RandGen rnd;
  int stunCount=0;
  bool hidden=false;
  float hideLevel=0.0f;
};
