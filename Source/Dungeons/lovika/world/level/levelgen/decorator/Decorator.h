#pragma once

#include "lovika/tile/TilePlacement.h"
#include "lovika/world/level/LevelGenRandom.h"

struct WeightedId;

namespace levelgen {
struct LevelDef;
}

namespace decorator {
struct State {
	const TilePlacement& tp;
	PlacementVector& decors;
	LevelGenRandom& rnd;
	float decorDensity;
	std::string stretchId;
	const std::vector<io::PropId>& decorGroups;
};

class TileDecorator {
public:
	virtual ~TileDecorator() {}

	virtual void decorate(State) const = 0;
};

void decorate(levelgen::LevelDef&, const TileDecorator&, RandomSeed);
}
