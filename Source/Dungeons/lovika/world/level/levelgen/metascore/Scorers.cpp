#include "Dungeons.h"
#include "Scorers.h"
#include "Picks.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "util/Algo.h"
#include <unordered_set>

namespace levelgen { namespace score {

Scorer Constant(float value) {
	return [value](const ScoreState&) { return value; };
}

Scorer Required(std::initializer_list<Scorer> s) {
	return [scorers = std::vector<Scorer>(s.begin(), s.end())](const ScoreState& state)->TOptional<float> {
		return algo::all_of(scorers, RETLAMBDA(it(state).Get(0) > 0)) ? 1 : TOptional<float>{};
	};
}

Scorer Required(Scorer s) {
	return [s = std::move(s)](const ScoreState& state) -> TOptional<float> {
		return s(state).Get(0) > 0 ? 1 : TOptional<float>{};
	};
}

Scorer Equals(Scorer scorer, float value, float epsilon) {
	return IsInRange(std::move(scorer), value - FMath::Abs(epsilon), value + FMath::Abs(epsilon));
}

Scorer IsInRange(Scorer scorer, float min, float max) {
	return [scorer = std::move(scorer), min, max](const ScoreState& state) {
		const auto value = scorer(state);
		return value && value.GetValue() >= min && value.GetValue() <= max;
	};
}

const Scorer& SubDungeons() {
	static const Scorer scorer = [](const ScoreState& state) -> TOptional<float> {
		const float numDungeonsIncludingMain = algo::map_as<std::unordered_set<int>>(state.range.stretches(), RETLAMBDA(it->dungeonInstance)).size();
		return std::max(0.f, numDungeonsIncludingMain - 1);
	};
	return scorer;
}
const levelgen::score::Scorer& BlocksArea()
{
	static const Scorer scorer = [](const ScoreState& state) -> TOptional<float> {

		return algo::sum(state.result.tileInfos, RETLAMBDA(it.metaTile.tile().size().productXz()));
	};
	return scorer;
}

const MetaScorer& JustMetaCodePath() {
	static const MetaScorer scorer({ Constant(1), pick::First() });
	return scorer;
}

namespace subrange {

Scorer SubRangeScorer(Scorer s, std::function<SubRange::Indices(const ScoreState&)> f) {
	return [s = std::move(s), f = std::move(f)](const ScoreState& state) {
		const ScoreState newState { state.result, SubRange(state.result, f(state)), state.rnd };
		return s(newState);
	};
}

Scorer MainDungeon(Scorer s) {
	return SubRangeScorer(std::move(s), [](const ScoreState& state) {
		return algo::copy_if(state.range.tileIndices, RETLAMBDA(state.result.tileInfos[it].stretchId.dungeonInstance == generator::MainDungeonInstanceId));
	});
}

Scorer SubDungeons(Scorer s) {
	return SubRangeScorer(std::move(s), [](const ScoreState& state) {
		return algo::copy_if(state.range.tileIndices, RETLAMBDA(state.result.tileInfos[it].stretchId.dungeonInstance != generator::MainDungeonInstanceId));
	});
}

}

}}
