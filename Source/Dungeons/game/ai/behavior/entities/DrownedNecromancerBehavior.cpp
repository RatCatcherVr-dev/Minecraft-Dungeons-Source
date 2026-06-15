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
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/bt/BtDelay.h"

namespace bt { namespace behavior { namespace entities {

io::MobGroup createDrownedMobGroup() {
	return game::mobspawn::sampleWithProbabilities(
		{ 
			EntityType::TridentDrowned,
			EntityType::Drowned 
		},
		std::initializer_list<float>({ 0.5f, 0.5f })
	);
}

BehaviorTuple createDrownedNecromancer(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;
	const auto optMarkerAttackCooldown = options.Get("TridentBombSummonCooldown", 4.f);

	return BehaviorTuple(root(
	selector("DrownedNecromancer",
		sequence("flee-state",
			startCondition(isInRange(actor::Target(), 500)),
			behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(1))
		),
		sequence("summon-if-close-enough",
			predicate(!actor::HasTag(tags::inLove)),
			isInRange(actor::Target(), 1250),
			onStart(focus::Set(actor::Target())),
			make_unique<USummonTask>(
				rnd.nextInt(options.Get("SummonCapLow", 5), 1 + options.Get("SummonCapHigh", 10)),
				createDrownedMobGroup(),
				options.Get("SummonDuration", 0.5s),
				options.Get("SummonCooldown", 4s),
				bt::value(rnd.nextInt(options.Get("SummonCountAtATimeLow", 2), 1 + options.Get("SummonCountAtATimeHigh", 2))),
				bt::locator::RandomReachablePointAround(bt::location::Self(), options.Get("SummonRadius", 350))
			)
		),
		uninterruptible(sequence("FormationAoeAttack",
			minTimeBetweenStopAndStart(optMarkerAttackCooldown),
			sequence(
				std::make_unique<UAoeAttack>(actor::Target(), "FormationAoeAttack"),
				delay(optMarkerAttackCooldown)
			)
		)),
		sequence(
			onStart(focus::Set(actor::Target())),		
			behavior::rangedAttackAndMoveCloser(mob, 0, options.Get("RangedAcquireTargetRangeUnits", 1000.f), options.Get("RangedLoseTargetRangeUnits", 1200.f), options.Get("RangedAcquireTargetRangeUnits", 1000.f)*0.8f, actor::Target(), Relative(0.33f), 5)
		),
		behavior::chaseIfAttacked(mob, Relative(options.Get("ChaseIfAttackedSpeedMultiplier", .6f))),
		behavior::chaseInOffensiveRange(mob, move::withSettings(actor::Target(), move::defaultRequest().SetAcceptanceRadius(900)), Relative(0.5f)),
		behavior::chaseIfWarned(mob, Relative(options.Get("ChaseIfAttackedSpeedMultiplier", .6f)))
	)),
	every(0.5s,
		set(actor::Target(), actor::ClosestEnemy())
	));
}

}}}
