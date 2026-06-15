#pragma once
#include "CommonTypes.h"
#include "Graph.h"
#include "lovika/io/LevelFile.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/tile/DoorDef.h"
#include "world/level/BlockPos.h"
#include "util/Validation.h"
#include <functional>
#include <initializer_list>

class BlockRegion;
class TileGroup;

namespace levelgen {
struct LevelDef;
}

namespace generator {
struct Result;
struct ValidationState;
using GeneratorFunc = std::function<Result(const TileGroup&, const io::Level&, RandomSeed, int maxTries)>;
using SeedGenerator = std::function<Result(RandomSeed)>;
using SoftValidator = std::function<float(ValidationState)>;

struct Stretch {
	io::StretchDef def;
	int index;
	int length;
	int dungeonInstance;
};

constexpr int MainDungeonInstanceId = 1;

// @note: This might change. Rationale: It can be inferred from the
// "PlacedTiles" structure, if we know the stretch a placed tile belongs to.
struct Tile {
	io::Overrides overrides(const levelgen::LevelDef&) const;

	MetaTile metaTile;
	PlaceResult placeResult;
	StretchId stretchId;
};

PlaceResultVector tilesToPlaceResults(const std::vector<Tile>&);

struct Result {
	Result()
		: mSuccess(true) {}

	Result(ValidationIssues issues)
		: issues(issues)
		, mSuccess(false) {}

	RandomSeed seed;
	std::vector<Stretch> stretches;
	std::vector<Tile> tileInfos;
	graph::Graph graph;

	ValidationIssues issues;

	bool success() const { return mSuccess; }
private:
	bool mSuccess;
};

}
