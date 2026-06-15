#include "Dungeons.h"
#include "MetaScoreTypes.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "util/Algo.h"

namespace levelgen { namespace score {

//
// Iterations
//
int Iterations::total() const {
	return successful + failedGeneration + failedScoring;
}

int Iterations::totalGeneratedLevels() const {
	return total() - failedGeneration;
}

bool SeedScore::operator<(const SeedScore& rhs) const {
	return score < rhs.score;
}

const SeedScore& PickState::current() const {
	return scores.back();
}

//
// SubRange (for working on a range of tiles and stretches)
//
SubRange::SubRange(const generator::Result& result)
	: generatorResult(result)
	, tileIndices(algo::iota_vector<int>(result.tileInfos.size()))
	, stretchIndices(algo::iota_vector<int>(result.stretches.size())) {
}

SubRange::SubRange(const generator::Result& result, Indices tileIndices)
	: generatorResult(result)
	, tileIndices(std::move(tileIndices))
{
	std::basic_string<bool> markedStretches(result.stretches.size(), false);
	for (auto i : this->tileIndices) {
		markedStretches[result.tileInfos[i].stretchId.index] = true;
	}
	for (int i = 0; i < markedStretches.size(); ++i) {
		if (markedStretches[i]) {
			stretchIndices.push_back(i);
		}
	}
}

std::vector<const generator::Tile*> SubRange::tiles() const {
	return algo::map_vector(tileIndices, RETLAMBDA(&generatorResult.tileInfos[it]));
}

std::vector<const generator::Stretch*> SubRange::stretches() const {
	return algo::map_vector(stretchIndices, RETLAMBDA(&generatorResult.stretches[it]));
}

//
// Scorer
//
MetaScorer::MetaScorer(Scorer scorer, Picker picker)
	: scorer(std::move(scorer))
	, picker(std::move(picker)) {
}

}}
