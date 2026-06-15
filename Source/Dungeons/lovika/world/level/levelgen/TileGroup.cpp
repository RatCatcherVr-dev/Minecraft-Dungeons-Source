#include "Dungeons.h"
#include "TileGroup.h"
#include "util/Algo.h"

TileGroup::TileGroup(MetaTileVector tiles)
	: mTiles(std::move(tiles))
{}

MetaTilePtr TileGroup::findById(std::string id) const {
	//D11.PS - removed std::move for console compile PS.TODO - come back to this.
	//id = std::move(Util::toLower(std::move(id)));
	id = Util::toLower(std::move(id)); 
	const auto ret = std::find_if(begin(mTiles), end(mTiles), RETLAMBDA(it.lowerId == id));
	return (ret != end(mTiles)) ? &(*ret) : nullptr;
}

TileGroup TileGroup::filter(const MetaTilePredicate& predicate) const {
	return TileGroup(::filter(tiles(), predicate));
}

TileGroup TileGroup::filter(const TilePredicate& predicate) const {
	return TileGroup(::filter(tiles(), predicate));
}

bool TileGroup::isEmpty() const {
	return mTiles.empty();
}

const MetaTileVector& TileGroup::tiles() const {
	return mTiles;
}

//
//
//
MetaTileVector filter(const MetaTileVector& tiles, const MetaTilePredicate& predicate) {
	return algo::copy_if(tiles, predicate);
}

MetaTileVector filter(const MetaTileVector& tiles, const TilePredicate& predicate) {
	return algo::copy_if(tiles, RETLAMBDA(predicate(it.tile())));
}

MetaTilePtr random(const TileGroup& group, LevelGenRandom& rnd) {
	auto& tiles = group.tiles();
	return tiles.empty() ? nullptr : &tiles[rnd.nextInt(tiles.size())];
}

std::vector<MetaTile> createMetaTiles(const TileGroup& tileGroup, const std::vector<io::WeightedTileId>& tiles) {
	std::vector<MetaTile> out;
	for (auto& tile : tiles) {
		if (auto meta = tileGroup.findById(tile.id)) {
			out.emplace_back(*meta);
			out.back().weight = tile.weight;
		}
	}
	return out;
}
