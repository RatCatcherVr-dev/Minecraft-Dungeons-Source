#pragma once

#include "lovika/world/level/levelgen/LevelDef.h"
#include "lovika/tile/TileArea.h"
#include "lovika/tile/PlacedTiles.h"

namespace game {

struct LevelDef {
	LevelDef(const levelgen::LevelDef&, RandomSeed);

	levelgen::LevelDef levelDef;
	RandomSeed finalSeed;
	PlacedTiles placedTiles;
	std::vector<TileArea> tileAreas;
	int levelLength;
};

}
