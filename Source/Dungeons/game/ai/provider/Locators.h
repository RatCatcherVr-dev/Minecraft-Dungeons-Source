#pragma once

#include "game/Locator.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/ai/bt/BtTypes.h"
#include <AITypes.h>

namespace bt {
namespace move { using Provider = bt::Provider<FAIMoveRequest>; }
namespace locator {

using Provider = bt::Provider<FLocator>;

struct Target {
	FLocator operator()(StateRef) const;
	void operator()(StateRef, FLocator) const;
};

struct LastValid {
	LastValid(const Provider&);
	FLocator operator()(StateRef) const;
private:
	mutable Provider provider;
	mutable FLocator lastValidLocationLocator;
};

Provider RandomReachablePointAround(const Provider&, float);

Provider RandomReachablePointAroundSmooth(const Provider&, const bt::Provider<AActor*>&, float);

Provider RandomReachablePointAround(
	const Provider&,
	float,
	const std::function<bool(const FVector&, const FVector&)>&
);

Provider RandomReachablePointAround(const Provider&, float radiusMin, float radiusMax);

Provider Forward(float distance);

Provider Forward(const bt::Provider<AActor*>& provider, float offset);

Provider Side(const Provider& provider, float radius, bool clockwise);

Provider Away(const Provider& provider, float distance = 500.f);

Provider From(const move::Provider&);

}}
