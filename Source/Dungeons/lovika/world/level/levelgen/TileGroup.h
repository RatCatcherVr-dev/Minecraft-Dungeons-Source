#pragma once

#include "lovika/tile/TileDef.h"
#include "lovika/world/level/LevelGenRandom.h"

class TileGroup {
public:
	TileGroup() {}
	TileGroup(MetaTileVector);

	MetaTilePtr findById(std::string id) const;
	TileGroup   filter(const MetaTilePredicate&) const;
	TileGroup   filter(const TilePredicate&) const;

	bool isEmpty() const;

	explicit operator bool() const {
		return !isEmpty();
	}

	const MetaTileVector& tiles() const;
private:
	MetaTileVector mTiles;
};

MetaTileVector filter(const MetaTileVector&, const MetaTilePredicate&);
MetaTileVector filter(const MetaTileVector&, const TilePredicate&);

MetaTilePtr random(const TileGroup&, LevelGenRandom&);

std::vector<MetaTile> createMetaTiles(const TileGroup&, const std::vector<io::WeightedTileId>&);
