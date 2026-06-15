#include "Dungeons.h"
#include "Picks.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "util/RandomUtil.h"

namespace levelgen { namespace score { namespace pick {


const Picker& First() {
	static const Picker picker = [](const PickState& state) {
		return state.current().seed;
	};
	return picker;
}

Picker FirstAtLeast(float minimum, bool orBest) {
	return [minimum, orBest](const PickState& state) -> PickResult {
		if (state.current().score >= minimum) {
			return state.current().seed;
		}
		if (state.isFinalPickChance) {
			return orBest? algo::max_element(state.scores)->seed : state.scores.front().seed;
		}
		return {};
	};
}

Picker Best(int successfulCount) {
	return Max(successfulCount);
}

Picker Random(int successfulCount) {
	return [successfulCount](const PickState& state) -> PickResult {
		if (state.scores.size() >= successfulCount || state.isFinalPickChance) {
			return Util::randomChoiceOrEmpty(state.scores, &state.rnd)->seed;
		}
		return {};
	};
}

Picker Max(int successfulCount) {
	return [successfulCount](const PickState& state) -> PickResult {
		if (state.scores.size() >= successfulCount || state.isFinalPickChance) {
			return algo::max_element(state.scores)->seed;
		}
		return {};
	};
}

Picker Min(int successfulCount) {
	return [successfulCount](const PickState& state) -> PickResult {
		if (state.scores.size() >= successfulCount || state.isFinalPickChance) {
			return algo::min_element(state.scores)->seed;
		}
		return {};
	};
}

}}}
