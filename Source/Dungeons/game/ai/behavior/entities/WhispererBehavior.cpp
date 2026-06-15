#include "Dungeons.h"
#include "WhispererBehavior.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/BtRandom.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/BtInvert.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/task/attack/formations/WhispererFormations.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/TurnTask.h"
#include "game/util/Tags.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "util/Random.h"

#include "EngineUtils.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createWhisperer(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { 

		const auto optHealthFraction = options.Get("health-fraction", 0.3f);
		const auto optMeleeRange = options.Get("melee-range", 300.0f);
		const auto optFleeRange = options.Get("flee-range", 600.0f);
		const auto optAttackRange = options.Get("attack-range", 1000.0f);
		const auto optApproachRange = options.Get("approach-range", 1500.0f);

		const auto optApproachSpeed = options.Get("approach-speed", 1.2f);
		const auto optFleeSpeed = options.Get("flee-speed", 1.6f);

		const auto optAttackDelayA = options.Get("attack-delay-A", 1.0s);
		const auto optAttackDelayB = options.Get("attack-delay-B", 1.0s);
		const auto optAttackDelayC = options.Get("attack-delay-C", 1.0s);

		const auto optAnimDurationA = options.Get("anim-duration-A", 1.0s);
		const auto optAnimDurationB = options.Get("anim-duration-B", 1.0s);
		const auto optAnimDurationC = options.Get("anim-duration-C", 1.0s);

		const auto optAttackBModifier = options.Get("attackB-modifier", 1.0s);

		const auto optAttackCooldown = options.Get("attack-cooldown", 3.0s);
		const auto optAttackCooldown2 = options.Get("attack-cooldown2", 5.0s);

		const auto optAttackProbabilityA1 = options.Get("attack-probability-A1", 0.4f);
		const auto optAttackProbabilityB1 = options.Get("attack-probability-B1", 0.3f);
		const auto optAttackProbabilityC1 = options.Get("attack-probability-C1", 0.2f);

		const auto optAttackProbabilityA2 = options.Get("attack-probability-A2", 0.0f);
		const auto optAttackProbabilityB2 = options.Get("attack-probability-B2", 0.0f);
		const auto optAttackProbabilityC2 = options.Get("attack-probability-C2", 1.0f);

		const auto lowHealth = makeSharedRef<bool>(false);
		const auto nextAttack = makeSharedRef<int>(0);

		const auto getAttack = [=]() {
			return [=](StateRef state) {
				static Random rnd;
				std::vector<float> probabilities = {
					*lowHealth ? optAttackProbabilityA2 : optAttackProbabilityA1,
					*lowHealth ? optAttackProbabilityB2 : optAttackProbabilityB1,
					*lowHealth ? optAttackProbabilityC2 : optAttackProbabilityC1,
				};

				float totalProbability = 0.0f;
				for (auto prob : probabilities) {
					totalProbability += prob;
				}

				float randomProbability = rnd.nextFloat(totalProbability);
				float currentProbability = 0.0f;
				int index = 0;
				for (auto prob : probabilities) {
					currentProbability += prob;
					if (randomProbability < currentProbability) {
						return index;
					}
					++index;
				}

				return 0;
			};
		};

		auto canPerformMagic = [=]() {
			return [=](StateRef state) {
				const auto& lastAttackTime = state.params().lastAttackTime;
				return lastAttackTime.IsPassedOrZero(state, *lowHealth ? optAttackCooldown2 : optAttackCooldown);
			};
		};


		return BehaviorTuple(root(selector("Whisperer",

			sequence("melee-attack",
				predicate(locator::IsInRange(actor::Target(), optMeleeRange)),
				meleeAttack(mob)
			),

			uninterruptible(sequence("flee",
				startPredicate(locator::IsInRange(actor::Target(), optFleeRange)),
				behavior::moveTo(move::LocationAwayFrom(actor::Target(), optFleeRange), Relative(optFleeSpeed)),
				maxRunTime(6.0s)
			)),

			sequence("flee-condition",
				predicate(actor::IsRecentlyDamagedFromAttack(2s)),
				behavior::defaultFleeFrom(actor::LastAttacker())
			),

			sequence("attack",
				predicate( locator::IsInRange(actor::Target(), optAttackRange) && canPerformMagic() ),
				onStart(set(nextAttack, getAttack())),
				chain(
					delay(0.1s),
					selector(

						uninterruptible(sequence("attackA",
							startPredicate(equals(nextAttack, value(0))),
							onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
							onStop(focus::Clear(EAIFocusPriority::Attacking)),
							make_unique<USummonFormationTask>(
								options.Get("QuickGrowingVineSequence"),
								optAnimDurationA,
								0.s,
								0.s,
								0.s,
								0.s,
								formation::makeWhispererWall(options.Get("wall-type", EntityType::QuickGrowingVineSimple)),
								USummonFormationTask::SummonMob,
								optAttackDelayA
							)
						)),

						uninterruptible(sequence("attackB",
							startPredicate(equals(nextAttack, value(1))),
							onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
							onStop(focus::Clear(EAIFocusPriority::Attacking)),
							chain(
								playAnimation(options.Get("WebSequence"), false, true, true),
								delay( optAttackBModifier ),
								sequence(
									focus::Clear(EAIFocusPriority::Attacking),
									make_unique<USummonFormationTask>(
										nullptr,
										optAnimDurationB - optAttackBModifier,
										0.s,
										0.s,
										0.s,
										0.s,
										formation::makeWhispererWeb(options.Get("web-type", EntityType::EntangleVine)),
										USummonFormationTask::SummonMob,
										optAttackDelayB
									),
									delay(optAnimDurationB - optAttackBModifier)
								)
							)
						)),

						uninterruptible(sequence( "attackC",
							startPredicate( equals( nextAttack, value( 2 )) ),
							onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
							onStop(focus::Clear(EAIFocusPriority::Attacking)),
							make_unique<USummonFormationTask>(
								options.Get("PoisonVineSequence"),
								optAnimDurationC,
								0.s,
								0.s,
								0.s,
								0.s,
								formation::makeWhispererPoison(options.Get("poison-type", EntityType::PoisonQuillVineSimple)),
								USummonFormationTask::SummonMob,
								optAttackDelayC
							)
						))
					)
				)
			),

			uninterruptible(sequence( "approach",
				predicate( 
					locator::IsInRange( actor::Target(), optApproachRange ) &&
					!locator::IsInRange( actor::Target(), optAttackRange )
				),
				behavior::moveTo( move::withSettings( actor::Target(),
						move::defaultRequest().SetAcceptanceRadius( optAttackRange-100.0f )),
					Relative(optApproachSpeed)
				),
				maxRunTime(6.0s)
			)),

			sequence( "idle",
				predicate(!locator::IsInRange(actor::Target(), optApproachRange )),
				defaultRoam()
			)

		)),

		parallel( "update-targets",
			every(0.1s, set(actor::Target(), actor::ClosestEnemy())),
			every(0.1s, 
				sequence(
					predicate( lessThan( actorStats::healthFraction(), value(optHealthFraction) ) ),
					set( lowHealth, value( true ) )
				)
			)
		)

	);}

}}}