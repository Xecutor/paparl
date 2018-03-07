#pragma once

struct GameTile{
  std::string sym;
  Color bg;
  Color fg;
  uint16_t walkCost;
  bool blocksLight;
};
