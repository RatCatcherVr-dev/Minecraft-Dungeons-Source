#include "Dungeons.h"
#include "EventMobType.h"

namespace game { namespace mobspawn { namespace eventmob {

MobType::MobType(std::vector<io::MobGroup> groups, float probability, EventPredicateProvider provider)
	: groups(std::move(groups))
	, mProbability(probability)
	, mPredicateProvider(std::move(provider)) {
}

float MobType::currentProbability(const EventState& state) const {
	if (!mCurrentPredicate) {
		mCurrentPredicate = mPredicateProvider({ state.rnd });
	}
	return mCurrentPredicate(state) ? mProbability : 0;
}

void MobType::trigger(const EventState& state) {
	mCurrentPredicate = mPredicateProvider({ state.rnd });
}

}}}
