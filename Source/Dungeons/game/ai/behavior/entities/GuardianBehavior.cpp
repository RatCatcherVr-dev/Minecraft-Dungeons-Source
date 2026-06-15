#include "Dungeons.h"
#include "GuardianBehavior.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtProbability.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/IsInRange.h"
#include "util/Random.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/action/AnimActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/task/attack/BeamAttack.h"
#include "game/component/BeamAttackComponent.h"
#include "game/ai/behavior/BeamAttackBehaviour.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/component/AoeAttackComponent.h"

namespace bt {
	namespace behavior {
		namespace entities {
			BehaviorTuple createGuardian(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

				enum class State {
					Normal,
					Fleeing,
					Spiking,
					Beaming,
				};

				enum class GuardianType
				{
					Normal,
					Elder,
				};

				const auto stage = sharedRef(State::Normal);
				const auto Type = sharedRef(options.Get("GuardianType", 1) == 1 ? GuardianType::Normal : GuardianType::Elder);

				return BehaviorTuple(
					root(selector("Guardian",
						sequence("Melee",
							predicate(equals(stage, value(State::Normal))),
							isInRange(
								actor::Target(),
								options.Get("SpikeMinAttackDistance", 300.f)
							),
							uninterruptibleSequence("Melee",
								onStart({
									[](StateRef state) {
									const auto beamComponent = state.owner->FindComponentByClass<UBeamAttackComponent>();
									beamComponent->Stop();
								} }),
								behavior::aoeAttack(mob),
								delay(1.7s)
							)
						),
						sequence("Chase",
							predicate(equals(stage, value(State::Normal))),
							predicate(equals(Type, value(GuardianType::Elder))),
							isInRange(
								actor::Target(),
								options.Get("ChaseDistance", 500.f)
							),
							onStart({
								[](StateRef state) {
								const auto beamComponent = state.owner->FindComponentByClass<UBeamAttackComponent>();
								beamComponent->Stop();
							} }),
							behavior::chase(actor::Target(), options.Get("ChaseDistance", 500.f), Relative(options.Get("ChaseSpeedMultiplier", 1.2f))),
							delay(2.0s)
						),
						sequence("flee-state",
							predicate(equals(Type, value(GuardianType::Normal))),
							behavior::defaultFleeFromAfterNumAttacks(mob.MobParams, locator::Target(), value(options.Get("FleeAfterNumAttacks", 1))),
							behavior::defaultFleeFrom(actor::Target(), 700.0f),
							anim::ClearLookAtTarget(),
							focus::Clear(EAIFocusPriority::Attacking),
							onStart({
								[](StateRef state) {
								const auto beamComponent = state.owner->FindComponentByClass<UBeamAttackComponent>();
								beamComponent->Stop();
							} }),
							set(stage, value(State::Fleeing)),
							delay(2.0s),
							set(stage, value(State::Normal))
						),
						greedySequence(
							isInRange(actor::Target(), options.Get("AquireTargetRange", 1000.0f)),
							predicate(equals(stage, value(State::Normal))),
							minTimeBetweenStopAndStart(options.Get("TotalBeamLifetime", 3.2s)),
							onStart({
								anim::SetLookAtTarget(actor::Target()),
								focus::Set(actor::Target(), EAIFocusPriority::Attacking),
								[](StateRef state) {
								const auto beamComponent = state.owner->FindComponentByClass<UBeamAttackComponent>();
								beamComponent->AttackLocal(actor::Target()(state));
							} }),
							set(stage, value(State::Beaming)),
							onStop(set(stage, value(State::Normal)))
						),
						sequence("Movement",
							predicate(equals(stage, value(State::Normal))),
							selector(
								behavior::chase(actor::Target(), FloatRange(options.Get("SpikeMinAttackDistance", options.Get("AcquireTargetRange", 1000.0f))), Relative(options.Get("ChaseSpeedMultiplier", 0.5f))),
								behavior::chaseIfAttacked(mob, Relative(0.5f)),
								behavior::chaseIfWarned(mob, Relative(0.5f)),
								behavior::defaultRoam()
							)
						)
					)),
					every(0.5s,
						set(actor::Target(), actor::ClosestEnemy())
					)
				);
			}
		}
	}
}
