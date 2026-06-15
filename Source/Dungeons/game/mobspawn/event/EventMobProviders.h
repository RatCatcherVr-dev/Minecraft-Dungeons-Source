#pragma once

#include "EventMobTypes.h"

namespace game { namespace mobspawn { namespace eventmob { namespace providers {

const EventPredicateProvider& Always();
      EventPredicateProvider  TilesSince(int min, int max);
      EventPredicateProvider  OnSubLevel(FString);
      EventPredicateProvider  SkipTiles(int);

      EventPredicateProvider  And(EventPredicateProvider, EventPredicateProvider);
      EventPredicateProvider  Or(EventPredicateProvider, EventPredicateProvider);

}}}}
