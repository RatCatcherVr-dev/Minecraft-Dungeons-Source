#include "Dungeons.h"
#include "LevelDef.h"
#include "generator/GeneratorUtil.h"
#include "lovika/BlockPosTransform.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"

namespace levelgen {

//
// TileProgress
//
const generator::Stretch& TileProgress::stretch(const levelgen::LevelDef& level) const {
	return level.stretches[stretchIndex];
}

const io::DungeonDef* TileProgress::dungeon(const levelgen::LevelDef& level) const {
	const int dungeonIndex = stretch(level).def.dungeonIndex;
	return Util::isValidIndex(level.data.dungeons, dungeonIndex) ? &level.data.dungeons[dungeonIndex] : nullptr;
}

//
// Translation of LevelDef
//
void translate(LevelDef& levelDef, BlockPos offset) {
	for (auto& tile : levelDef.tiles) {
		translate(tile.placeResult, offset);
	}
	const auto transform = blockpostransform::offset(offset);
	auto edges = levelDef.graph.edges();
	algo::for_each(edges, [&](generator::graph::Edge& e) {
		e.src.door = transformed(e.src.door, transform);
		e.dst.door = transformed(e.dst.door, transform);
	});
	levelDef.graph = generator::graph::Graph(edges);
}

void translateTo(LevelDef& levelDef, BlockPos position) {
	translate(levelDef, position - generator::util::bounds(levelDef.tiles).minInclusive);
}

void translateToFit(LevelDef& levelDef) {
	translateTo(levelDef, BlockPos(0, 1, 0));
}

}
