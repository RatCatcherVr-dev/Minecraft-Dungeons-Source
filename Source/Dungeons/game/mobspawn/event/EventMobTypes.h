#pragma once

#include "CommonTypes.h"
#include "game/level/GameTile.h"
#include <functional>
#include <vector>

class Random;

namespace game { namespace mobspawn { namespace eventmob {

struct EventStamp {
	int tiles;
	float seconds;
};

struct EventState {
	Random& rnd;
	TileRef furthest;
	EventStamp current;
	EventStamp globalPassed;
	EventStamp passed;
};

using EventPredicate = Pred<EventState>;

struct ProviderState {
	Random& rnd;
};

using EventPredicateProvider = std::function<EventPredicate(ProviderState)>;

}}}
