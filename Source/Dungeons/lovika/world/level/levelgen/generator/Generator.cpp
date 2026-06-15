#include "Dungeons.h"
#include "Generator.h"
#include "lovika/world/level/levelgen/LevelDef.h"
#include "util/Algo.h"
#include <numeric>

namespace generator {

//
// Tile
//
io::Overrides Tile::overrides(const levelgen::LevelDef& level) const {
	// @todo: merge the tile's data here, if we ever get tile specific Overrides
	return level.stretches[stretchId.index].def.overrides;
}

PlaceResultVector tilesToPlaceResults(const std::vector<Tile>& tiles) {
	return algo::map_as<PlaceResultVector>(tiles, RETLAMBDA(it.placeResult));
}

}
