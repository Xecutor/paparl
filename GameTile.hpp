#pragma once

#include <string>

#include "Types.hpp"

struct GameTile{
  std::string sym;
  Color bg;
  Color fg;
  bool lightSource;
  int  lightStrength;
  Color lightTint;
  uint16_t walkCost;
  bool blocksLight;
  std::string desc;
};

enum class GameTileType{
  aslphalt,
  aslphaltHLine,
  aslphaltVLine,
  concrete,
  tilepath,
  dirt,
  bush,
  concreteWall,
  streetLight,
  streetLightOff,
  brokenStreetLight,
  glassWall,
  ceilingLight,
  interiorFloor1,
  interiorFloor2,
  carTL, carH, carTR,carV,carX, carSP, carB
};

const GameTile& getTile(GameTileType tile);
