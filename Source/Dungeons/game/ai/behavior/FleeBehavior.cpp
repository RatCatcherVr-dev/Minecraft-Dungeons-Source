#include "Dungeons.h"
#include "FleeBehavior.h"
#include "MoveBehavior.h"
#include "NodeFactory.h"
#include "game/ai/provider/Move.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/TimeLimited.h"
#include "game/ai/bt/BtLogic.h"
#include "game/actor/character/mob/MobParams.h"

namespace bt { namespace behavior {

Unique<UBtGroup> defaultFleeFrom(const locator::Provider& provider, float distance/* = 1000.f*/, const speed::Speed& speed /*= Relative(1)*/) {
	return sequence("flee",
		startCondition(isInRange(provider, distance)),
		greedySequence("run-away",
			behavior::moveTo(move::LocationAwayFrom(provider, 800, 850), speed),
			maxRunTime(6.5s)
		)
	);
}

Unique<UBtGroup> defaultFleeFromAfterNumAttacks(const FMobParams& params, const locator::Provider& provider, const Provider<int>& afterNumAttacks, float distance /*= 1000.f*/, const speed::Speed& speed /*= Relative(1)*/) {
	auto currentAttackCounter = ref(params.successfulAttacks.all);
	auto canFleeAttackCounter = sharedRef(params.successfulAttacks.all + 1);

	return sequence("flee-from-after-num-attacks",
		predicate(greaterThanOrEquals(currentAttackCounter, canFleeAttackCounter)),
		defaultFleeFrom(provider, distance, speed),
		onStop(set(canFleeAttackCounter, plus(currentAttackCounter, afterNumAttacks)))
	);
}

}}
