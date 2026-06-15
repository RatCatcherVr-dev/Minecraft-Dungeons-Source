#include "Dungeons.h"
#include "AnimalBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Move.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "util/Random.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPanda(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { 
	static Random rnd;

	auto optSitTime = options.Get("sit-time", 8.0f);
	auto optSitVariation = options.Get("sit-variation", 2.0f);
	auto optRollChance = options.Get("roll-chance", 1.0f);
	auto optStartRoll = options.Get("roll-distance", 500.0f);
	auto optEndRoll = optStartRoll - 50.0f;

	const auto targetPosition = makeSharedRef<FAIMoveRequest>();
	const auto rollCheck = makeSharedRef<bool>(true);

	auto getTargetPosition = [=]() -> move::Provider {
		return [=](StateRef state) {
			return *targetPosition;
		};
	};

	auto optPauseTime = bt::Duration(bt::Seconds(rnd.nextFloat(
		optSitTime - optSitVariation,
		optSitTime + optSitVariation
	)));

	return BehaviorTuple(
		selector("panda",

			sequence("flee-condition",
				startPredicate(actor::IsRecentlyDamagedFromAttack(10s)),
				behavior::defaultFleeFrom(actor::LastAttacker())
			),

			sequence("roam",
				minTimeBetweenStopAndStart( optPauseTime ),
				onStart(set( targetPosition, move::RandomLocationAround(location::Anchor(), 1000.0f ) ) ),
				onStart( set(rollCheck, value( true ) ) ),
				chain(
					delay(0.1s),
					parallel(
						behavior::moveTo(getTargetPosition(), Relative( 1.0f ) ),
						sequence(
							predicate( 
								locator::IsInRange( locator::From(getTargetPosition()), optStartRoll ) &&
								!locator::IsInRange( locator::From(getTargetPosition()), optEndRoll )
							),
							sequence(
								predicate( random::probability( optRollChance ) && equals( rollCheck, value( true ) ) ),
								onStart( set(rollCheck, value(false) ) ),
								playAnimation( options.Get( "Roll" ), false, true, true )
							)
						)
					)
				)
			)

		)
	);

}

}}}
