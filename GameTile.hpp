#pragma once

#include <string>

#include "Types.hpp"

enum class GameTileType{
  barrier,
  aslphalt,
  aslphaltHLine,
  aslphaltVLine,
  concrete,
  tilepath,
  dirt,
  bush,
  bench,
  concreteWall,
  streetLight,
  streetLightOff,
  brokenStreetLight,
  glassWall,
  ceilingLight,
  interiorFloor1,
  interiorFloor2,
  carTL, carH, carTR,carV,carX, carSP, carB,
  grass,
  woodenWall,
  fence,
  dirtRoad,
  crate,
  shelf,
};

struct GameTile{
  GameTileType gtt;
  std::string sym;
  Color bg;
  Color fg;
  bool lightSource;
  int  lightStrength;
  Color lightTint;
  uint16_t walkCost;
  bool blocksLight;
  std::string desc;
  GameTile(const GameTile&)=delete;
};

const GameTile& getTile(GameTileType tile);
