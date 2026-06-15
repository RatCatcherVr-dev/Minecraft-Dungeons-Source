#pragma once

#include "lovika/world/level/levelgen/LevelGen.h"
#include "game/level/GameTiles.h"

namespace generatortest { namespace util {

struct TilesData {
	TilesData(game::LevelDef);
	game::LevelDef levelDef;
	game::Tiles tiles;
};

Unique<TilesData> createTiles(const levelgen::Result&);

}}

