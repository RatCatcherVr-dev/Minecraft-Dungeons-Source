#pragma once

#include "TileSet.h"
#include "lovika/tile/PlacedTiles.h"
#include "generator/Generator.h"

struct FreezeResult {
	TileSet tiles;
	PlaceResultVector placeResults;
};

class TilePlacementFreezer {
public:
	TilePlacementFreezer();

	PlaceResult freeze(PlaceResult);

	FreezeResult& result();
	const FreezeResult& result() const;
private:
	int mIndex;
	FreezeResult mFrozen;
};

FreezeResult freezeAll(const std::vector<generator::Tile>&);
FreezeResult freezeAllInPlace(std::vector<generator::Tile>&);
