#include "Dungeons.h"
#include "RedstoneMonstrosityBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/task/attack/formations/RedstoneFormations.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "world/entity/EntityTypes.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createRedstoneMonstrosity(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { 
	enum class State {
		Sleeping,
		Waking,
		Active
	};
	
	const auto stage = sharedRef(State::Sleeping);

	return BehaviorTuple(
		selector("RedstoneMonstrosity",
			sequence(
				predicate(equals(stage, value(State::Sleeping))),
				alwaysTrue(playAnimation(options.Get("Sleeping"), true)),
				alwaysTrue(sequence(
					predicate(actor::IsAnyPlayersInRange(options.Get("WakeupRange", 1000.f)) || actor::IsRecentlyDamagedFromAttack(10s)),
					set(stage, value(State::Waking))
				))
			),
			sequence(
				predicate(equals(stage, value(State::Waking))),
				playAnimation(options.Get("Waking")),
				alwaysTrue(dropFor(options.Get("WakeupDuration", 2.7s),
					set(stage, value(State::Active))
				))
			),
			sequence(
				predicate(equals(stage, value(State::Active))),
				root(selector("main-loop",
					uninterruptible(sequence("melee-attack",
						startPredicate(random::probability(actorStats::healthFraction())),
						behavior::meleeAttack(mob)
					)),
					uninterruptible(sequence("summon-state",
						startPredicate(
							!random::probability(actorStats::healthFraction()) &&
							lessThan(
								actor::CountInRange(actor::Self(), 3000.f, actorquery::is::entityType(options.Get("SummonEntity", EntityType::RedstoneCube))),
								value(options.Get("NumberOfMobsLeftToResummon", 4)
							))
						),
						make_unique<USummonFormationTask>(
							options.Get("SummonSequence"),
							options.Get("SummonDuration", 5.2s),
							options.Get("SummonStart", 1.1s),
							options.Get("SummonCooldown", 4s),
							0.s,
							0.s,
							formation::makeRedstoneCubeRandom(
								options.Get("RadiusMin", 2),
								options.Get("RadiusMax", 7),
								options.Get("CubeCount", 7),
								options.Get("SummonEntity", EntityType::RedstoneCube)
							),
							USummonFormationTask::SummonMob,
							options.Get("EmergeDuration", 1.5s)
						)
					)),
					uninterruptible(sequence("ranged-attack",
						startPredicate(
							!random::probability(actorStats::healthFraction())							
						),
						rangedAttack(
							mob,
							options.Get("RangedMinAttackDistance", 400.f),
							options.Get("RangedAcquireTargetRangeUnits", 1800.f),
							options.Get("RangedLoseTargetRangeUnits", 2300.f),
							actor::Target()
						)
					)),
					sequence("chase",
						isInRange(actor::Target(), 3500),
						behavior::moveTo(actor::Target(), Relative(0.8f))
					),
					behavior::chaseIfAttacked(mob, Relative(0.95f)),
					behavior::defaultRoam()
				))
			)
		),
		parallel(
			every(0.5s, set(actor::Target(), actor::ClosestEnemy()))
		)
	); 
}

}}}
