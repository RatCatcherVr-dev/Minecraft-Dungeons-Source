#include "Dungeons.h"
#include "NecromancerBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/util/Tags.h"
#include "util/Random.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt { namespace behavior { namespace entities {

io::MobGroup createMobGroup() {
	static const std::vector<EntityType> necromancerTypes{
		EntityType::Zombie,
		EntityType::BabyZombie,
		EntityType::Husk,
		EntityType::ChickenJockey,
		EntityType::Skeleton
	};
	return game::mobspawn::sampleWithProbabilities(necromancerTypes, { 1 });
}

BehaviorTuple createNecromancer(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto SummonCapLow = options.Get("SummonCapLow", 5);
	const auto SummonCapHigh = options.Get("SummonCapHigh", 10);
	const auto SummonDuration = options.Get("SummonDuration", 0.5s);
	const auto SummonCooldown = options.Get("SummonCooldown", 4s);
	const auto SummonCountAtATimeLow = options.Get("SummonCountAtATimeLow", 2);
	const auto SummonCountAtATimeHigh = options.Get("SummonCountAtATimeHigh", 2);
	const auto SummonRadius = options.Get("SummonRadius", 350);
	const auto RangedAcquireTargetRangeUnits = options.Get("RangedAcquireTargetRangeUnits", 1000.f);
	const auto RangedLoseTargetRangeUnits = options.Get("RangedLoseTargetRangeUnits", 1200.f);
	const auto ChaseIfAttackedSpeedMultiplier = options.Get("ChaseIfAttackedSpeedMultiplier", .6f);

	return BehaviorTuple(
	selector("Necromancer",
		sequence("flee-state",
			startCondition(isInRange(actor::Target(), 500.f)),
			behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(1))
		),
		sequence("summon-if-close-enough",
			predicate(!actor::HasTag(tags::inLove)),
			isInRange(actor::Target(), 1250.f),
			onStart(focus::Set(actor::Target())),
			make_unique<USummonTask>(
				rnd.nextInt(SummonCapLow, 1 + SummonCapHigh),
				createMobGroup(),
				SummonDuration,
				SummonCooldown,
				bt::value(rnd.nextInt(SummonCountAtATimeLow, 1 + SummonCountAtATimeHigh)),
				bt::locator::RandomReachablePointAround(bt::location::Self(), SummonRadius)
			)
		),
		sequence(
			onStart(focus::Set(actor::Target())),		
			behavior::rangedAttackAndMoveCloser(mob, 0, RangedAcquireTargetRangeUnits, RangedLoseTargetRangeUnits, RangedAcquireTargetRangeUnits * 0.8f, actor::Target(), Relative(0.33f), 5)
		),
		behavior::chaseIfAttacked(mob, Relative(ChaseIfAttackedSpeedMultiplier)),
		behavior::chaseInOffensiveRange(mob, move::withSettings(actor::Target(), move::defaultRequest().SetAcceptanceRadius(900)), Relative(0.5f)),
		behavior::chaseIfWarned(mob, Relative(ChaseIfAttackedSpeedMultiplier))
	),
	every(0.5s,
		set(actor::Target(), actor::ClosestEnemy())
	));
}

}}}
