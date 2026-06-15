#pragma once

#include "lovika/world/level/levelgen/LevelDef.h"
#include <vector>

namespace generator { struct Tile; }
namespace generator { namespace graph { struct Graph; } }

namespace generator { namespace graph { namespace util {

Graph createGraph(const std::vector<Tile>&);
std::vector<levelgen::TileProgress> createProgress(const Graph&, const std::vector<Tile>&);

int findEndTileIndex(const std::vector<Tile>&); // @attn: This might explode if we have any side-paths on the last stretch
std::vector<size_t> findMainPathTileIndices(const Graph&, const std::vector<Tile>&, size_t goalTileIndex);

}}}
