#pragma once

#include "EventMobTypes.h"
#include "lovika/io/IoMobTypes.h"
#include <vector>

class Random;

namespace game { namespace mobspawn { namespace eventmob {

struct MobType {
	MobType(std::vector<io::MobGroup>, float probability, EventPredicateProvider);

	float currentProbability(const EventState&) const;

	void trigger(const EventState&);

	std::vector<io::MobGroup> groups;
private:
	float mProbability;
	EventPredicateProvider mPredicateProvider;
	mutable EventPredicate mCurrentPredicate;
};

}}}
