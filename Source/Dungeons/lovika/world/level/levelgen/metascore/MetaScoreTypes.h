#pragma once

#include "lovika/world/level/LevelGenRandom.h"
#include <Optional.h>
#include <vector>
#include <functional>

typedef uint32_t RandomSeed;

namespace generator {
	struct Tile;
	struct Stretch;
	struct Result;
}

namespace levelgen { namespace score {

struct Iterations {
	int successful = 0;
	int failedGeneration = 0;
	int failedScoring = 0;

	int total() const;
	int totalGeneratedLevels() const;
};

struct SeedScore {
	RandomSeed seed;
	float score;

	bool operator<(const SeedScore&) const;
};

struct PickState {
	const SeedScore& current() const;

	Iterations iterations;
	const std::vector<SeedScore>& scores;
	bool isFinalPickChance;
	LevelGenRandom& rnd;
};

using PickResult = TOptional<RandomSeed>;
using Picker = std::function<PickResult(const PickState&)>;

struct SubRange {
	using Indices = std::vector<int>;

	SubRange(const generator::Result&);
	SubRange(const generator::Result&, Indices tileIndices);

	std::vector<const generator::Tile*> tiles() const;
	std::vector<const generator::Stretch*> stretches() const;

	Indices tileIndices;
	Indices stretchIndices;
private:
	const generator::Result& generatorResult;
};

struct ScoreState {
	const generator::Result& result;
	const SubRange& range;
	LevelGenRandom& rnd;
};

using Scorer = std::function<TOptional<float>(const ScoreState&)>;

struct MetaScorer {
	MetaScorer(Scorer, Picker);

	Scorer scorer;
	Picker picker;
};

}}
