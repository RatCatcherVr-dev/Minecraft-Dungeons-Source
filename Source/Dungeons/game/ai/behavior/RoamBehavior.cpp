#include "Dungeons.h"
#include "RoamBehavior.h"
#include "NodeFactory.h"
#include "MoveBehavior.h"
#include "game/ai/action/BtAction.h"
#include "game/ai/condition/TimeLimited.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/provider/Move.h"

namespace bt { namespace behavior {

Unique<UBtGroup> defaultRoam(Duration minIntervalSeconds/*= 3s*/, const speed::Speed& speed/*= speed::Speed(EMovementState::Walking)*/) {
	return sequence("roam",
		minTimeBetweenStopAndStart(minIntervalSeconds),
		onStart(focus::Clear()),
		behavior::moveTo(move::RandomLocationAround(location::Anchor(), 1000), speed)
	);
}

Unique<UBtGroup> smoothRoam(Duration minIntervalSeconds/*= 3s*/, const speed::Speed& speed/*= speed::Speed(EMovementState::Walking)*/) {
	return sequence("roam",
		minTimeBetweenStopAndStart(minIntervalSeconds),
		onStart(focus::Clear()),
		behavior::moveTo(move::From(locator::RandomReachablePointAroundSmooth(location::Anchor(), actor::Self(), 1000.f)), speed)
	);
}

}}
