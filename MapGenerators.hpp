#pragma once

#include <stdint.h>
#include "Types.hpp"

template <class T>
class GumMap;
struct GameTile;
struct TileObjects;
using GameMap=GumMap<TileObjects>;

class MissionDetails;

enum class GeneratorType{
  downtown,
  suburbs,
  warehouse,
  seaport,
};


IPos generateLevel(GeneratorType gt, uint32_t seed, const MissionDetails& md, GameMap& map);
void fillTileInfo(GameMap& map, IPos p, const GameTile& tile);
