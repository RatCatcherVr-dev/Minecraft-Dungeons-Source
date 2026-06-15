#include "Dungeons.h"
#include "EventMobProviders.h"
#include "util/Random.h"
#include "util/BooleanOperatorMacros.h"

namespace game { namespace mobspawn { namespace eventmob { namespace providers {

const EventPredicateProvider& Always() {
	static const EventPredicate pred = [](auto state) { return true; };
	static const EventPredicateProvider provider = [](auto state) { return pred; };
	return provider;
}

EventPredicateProvider TilesSince(int min, int max) {
	return [=](auto state) {
		return [tilesBetween = state.rnd.nextInt(min, max + 1)](auto state) {
			return state.passed.tiles >= tilesBetween;
		};
	};
}

EventPredicateProvider OnSubLevel(FString level) {
	return [level = std::move(level)](ProviderState) {
		return [=](EventState state) {
			return state.furthest.dungeon().def().level.id.Equals(level, ESearchCase::IgnoreCase);
		};
	};
}

EventPredicateProvider SkipTiles(int maxSkipCount) {
	return [maxSkipCount, seenTiles = std::make_shared<std::unordered_set<int>>()](ProviderState) mutable {
		return [=](EventState state) {
			seenTiles->insert(state.furthest.progress().global().index());
			return seenTiles->size() > maxSkipCount;
		};
	};
}

GENERATE_BOOLEAN_PRED_OPERATORS(EventState);

EventPredicateProvider And(EventPredicateProvider prov1, EventPredicateProvider prov2) {
	return [prov1 = std::move(prov1), prov2 = std::move(prov2)](ProviderState providerState) {
		return prov1(providerState) && prov2(providerState);
	};
}

EventPredicateProvider Or(EventPredicateProvider prov1, EventPredicateProvider prov2) {
	return [prov1 = std::move(prov1), prov2 = std::move(prov2)](ProviderState providerState) {
		return prov1(providerState) || prov2(providerState);
	};
}

}}}}
