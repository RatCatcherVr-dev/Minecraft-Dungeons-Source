#pragma once

#include "lovika/BlockCuboid.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/world/level/levelgen/generator/Generator.h"

namespace generator { struct Stretch; }
namespace io { struct DungeonDef; }

namespace levelgen {

struct LevelDef;

struct TileProgress {
	int stretchIndex;
	int stretchTileIndex;
	int globalTileIndex;
	int strayPathSubId;
	int strayPathIndex;
	int strayPathLength;

	const generator::Stretch& stretch(const levelgen::LevelDef&) const;
	const io::DungeonDef* dungeon(const levelgen::LevelDef&) const;
};

struct LevelDef {
	std::string id;
	std::vector<generator::Stretch> stretches;
	std::vector<generator::Tile> tiles;
	std::vector<TileProgress> progress; //@progress
	generator::graph::Graph graph;
	io::Level::PassThrough data;
};

//BlockCuboid calculateBounds(const LevelDef&);
void translate(LevelDef&, BlockPos offset);
void translateTo(LevelDef&, BlockPos position);
void translateToFit(LevelDef&);

}