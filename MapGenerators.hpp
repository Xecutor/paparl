#pragma once

#include <stdint.h>
#include "Types.hpp"
#include "MissionDetails.hpp"
#include "GeneratorType.hpp"

template <class T>
class GumMap;
struct GameTile;
struct TileObjects;
using GameMap=GumMap<TileObjects>;


IPos generateLevel(GeneratorType gt, uint32_t seed, const MissionDetails& md, std::vector<MissionDetails::EnemyPosition>& enemies, GameMap& map);
void fillTileInfo(GameMap& map, IPos p, const GameTile& tile);
