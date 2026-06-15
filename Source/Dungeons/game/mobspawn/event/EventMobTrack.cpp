#include "Dungeons.h"
#include "EventMobTrack.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "game/mobspawn/MobGroupUtil.h"

namespace game { namespace mobspawn { namespace eventmob {

Track::Track(EventPredicateProvider provider, EventStamp lastSpawned)
	: mPredicateProvider(std::move(provider))
	, mLastSpawned(lastSpawned) {
}

Track& Track::addMobs(std::vector<io::MobGroup> groups, float probability /*= 1 */, EventPredicateProvider predicateProvider /* = Always()*/) {
	for (auto& group : groups) {
		prepareMobGroup(group);
	}
	mTypes.push_back({
		std::move(groups),
		probability,
		std::move(predicateProvider)
	});
	return *this;
}

bool Track::canRun(const EventState& state) {
	if (!mCurrentPredicate) {
		trigger(state);
	}
	return mCurrentPredicate(state);
}

void Track::trigger(const EventState& state) {
	mLastSpawned = state.current;
	mCurrentPredicate = mPredicateProvider({ state.rnd });
}

MobType* Track::randomChoice(const EventState& state) {
	const auto probabilities = algo::map_tarray(mTypes, RETLAMBDA(it.currentProbability(state)));
	const int index = Util::randomWeightedItemIndex(probabilities, &state.rnd);
	return Util::isValidIndex(mTypes, index)? &mTypes[index] : nullptr;
}

}}}
