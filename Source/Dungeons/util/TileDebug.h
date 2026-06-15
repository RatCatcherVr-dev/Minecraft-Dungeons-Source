#pragma once

#include <vector>
#include <string>
#include "game/level/GameTile.h"
#include "game/level/GameTiles.h"
#include "lovika/BlockCuboid.h"

namespace Util {

	struct TileDebugData {
		std::string name;
		BlockCuboid location;
		std::vector<TileDebugData> children;
	};

	TileDebugData traverse(const game::Tiles&, std::unordered_set<std::string>, const game::Tile&);

	TileDebugData collectTiles(const game::Tiles&);

}
