#include "Dungeons.h"
#include "IsInRange.h"
#include "game/util/ActorQuery.h"
#include "game/ai/bt/BtEvalState.h"
#include "LocatorPredicates.h"

UIsInRange::UIsInRange(const bt::locator::Provider& locator, FloatRange distanceRange)
	: UIsInRange(locator, distanceRange, distanceRange) {
}

UIsInRange::UIsInRange(const bt::locator::Provider& locator, FloatRange enterDistanceRange, FloatRange exitDistanceRange)
	: inEnterRange(bt::locator::IsInRange(locator, enterDistanceRange))
	, inExitRange(bt::locator::IsInRange(locator, exitDistanceRange)) {
	type = "is-in-range";
}

bool UIsInRange::OnCanRun(bt::StateRef state) {
	return inEnterRange(state);
}

bool UIsInRange::OnCanContinue(bt::StateRef state) {
	return inExitRange(state);
}


Unique<UIsInRange> isInRange(const bt::locator::Provider& provider, FloatRange distanceRange) {
	return make_unique<UIsInRange>(provider, distanceRange);
}

Unique<UIsInRange> isInRange(const bt::locator::Provider& provider, FloatRange enterDistanceRange, FloatRange exitDistanceRange) {
	return make_unique<UIsInRange>(provider, enterDistanceRange, exitDistanceRange);
}


