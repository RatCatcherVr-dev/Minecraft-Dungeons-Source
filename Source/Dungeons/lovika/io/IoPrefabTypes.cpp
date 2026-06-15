#include "Dungeons.h"
#include "IoPrefabTypes.h"
#include "util/Algo.hpp"

namespace io {

//
// Teleport
//
bool Teleport::isJumpToExistingPoint() const {
	return target && !isJumpToNewDungeon();
}

bool Teleport::isJumpToNewDungeon() const {
	return dungeons && !dungeons->empty();
}

bool Teleport::hasDungeonId(const CaseInsensitiveId& id) const {
	return dungeons && algo::contains(dungeons.GetValue(), id);
}


//
// Tile
//
Tile::Tile(const WeightedId& id)
	: WeightedId(id)
	, objectId(id.id) {
}

Tile::Tile(const std::string& id, float weight)
	: WeightedId(id, weight)
	, objectId(id) {
}

bool Tile::operator<(const Tile& rhs) const {
	return lowerId < rhs.lowerId;
}

}
