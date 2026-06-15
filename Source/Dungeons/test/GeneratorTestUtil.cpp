#include "Dungeons.h"
#include "GeneratorTestUtil.h"
#include "util/Algo.h"

namespace generatortest { namespace util {

TilesData::TilesData(game::LevelDef def)
	: levelDef(std::move(def))
	, tiles(levelDef) {
}

Unique<TilesData> createTiles(const levelgen::Result& result) {
	game::LevelDef levelDef(result.levelDef, result.finalSeed);

	algo::map_to(result.levelDef.tiles, [](const generator::Tile& genTile) {
		const auto& size = genTile.metaTile.tile().size();
		const auto surfaceSize = static_cast<uint32>(size.productXz());
		return TileArea{ surfaceSize, surfaceSize, surfaceSize, {size.x, size.z} };
	},
	levelDef.tileAreas);

	return std::make_unique<TilesData>(std::move(levelDef));
}

}}
