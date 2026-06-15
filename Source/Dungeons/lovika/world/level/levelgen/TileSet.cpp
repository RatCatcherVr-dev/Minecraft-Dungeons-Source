#include "Dungeons.h"
#include "TileSet.h"
#include "TileGroup.h"
#include "CommonTypes.h"

void TileSet::feed(Unique<TileDef> tile) {
	mTiles.emplace(tile->lowerId(), std::move(tile));
}

void TileSet::feed(std::vector<Unique<TileDef>>& tiles) {
	for (auto& tile : tiles) {
		feed(std::move(tile));
	}
}

void TileSet::feed(TileSet&& tiles) {
	for (auto&& e : tiles.mTiles) {
		feed(std::move(e.second));
	}
	tiles.mTiles.clear();
}

TileGroup TileSet::createGroup(const std::vector<io::Tile>& tileDefs) const {
	MetaTileVector out;

	std::unordered_set<std::string> added;

	for (auto&& tileDef : tileDefs) {
		auto it = mTiles.find(tileDef.objectId);
		if (it == mTiles.end()) {
			continue; // Invalid json reference, but the validation should be able to catch it
		}
		out.emplace_back(tileDef, *it->second, tileDef.metadata);
		added.insert(tileDef.id);
	}
	for (auto&& kv : mTiles) {
		if (!added.count(kv.first)) {
			out.emplace_back(io::WeightedTileId{kv.first}, *kv.second);
		}
	}
	return out;
}

bool TileSet::isEmpty() const {
	return mTiles.empty();
}
