#pragma once

#include "EventMobType.h"
#include "EventMobProviders.h"
#include "lovika/io/IoMobTypes.h"
#include <vector>

class Random;

namespace game { namespace mobspawn { namespace eventmob {

class EventMobSpawner;

//
// Spawner Track -- Tracks are generally independent. One track might be "bonus mobs"
//                  like PiggyBanks, and one track might be the dangerous event mobs
//                  for the level. Since they use io::MobGroups/Types, we can get
//                  multiple mobs to spawn on the same time using min/max values.
//
struct Track {
	Track& addMobs(std::vector<io::MobGroup>, float probability = 1, EventPredicateProvider = providers::Always());
private:
	Track(EventPredicateProvider, EventStamp);
	bool canRun(const EventState&);
	void trigger(const EventState&);
	MobType* randomChoice(const EventState&);

	friend EventMobSpawner;
	EventPredicateProvider mPredicateProvider;
	EventPredicate mCurrentPredicate;
	EventStamp mLastSpawned;
	std::vector<MobType> mTypes;
};

}}}
