#include "Dungeons.h"
#include "VindicatorBehavior.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/MeleeTicketProvider.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "util/Random.h"

class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createVindicator(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		enum class State {
			Locked,
			Aggresive,
			Standby,
			Idle,
			Init
		};

		const auto stage = makeSharedRef<State>(State::Init);

		const auto angleSign = rnd.nextFloat() < .5f ? 1 : -1;

		const auto ticketProvider = sharedObjectRef<AMeleeTicketProvider>();
		const auto canAttackTarget = [=, &options](const actor::Provider& targetProvider) {
			return [=, duration = options.Get("MeleeTicketDuration", 1.25f), tickets = options.Get("MeleeTickets", 2.0f)](StateRef state) {
				if (const auto target = targetProvider(state)) {
					if (ticketProvider) {
						return ticketProvider->CanAttack(state.owner, target, duration, tickets);
					} else {
						ticketProvider(state, actorquery::getFirstActorTracked<AMeleeTicketProvider>(&state.world()));
					}
				}
				// not attacking is worse than attacking out of turn
				return true;
			};
		};		

		const auto speedMultiplier = rnd.nextFloat(options.Get("WalkSpeedFactorMin", 0.4f), options.Get("WalkSpeedFactorMax", 0.6f));
		const auto moveAroundRadius = rnd.nextFloat(options.Get("MoveAroundRadiusMin", 700.0f), options.Get("MoveAroundRadiusMax", 900.0f));
		const auto clockwise = rnd.nextBoolean();

		return BehaviorTuple(
			selector("Vindicator",
				sequence(
					predicate(locator::IsInRange(actor::Target(), 1100.f) || !equals(stage, value(State::Init))),
					parallel(
						sequence(
							predicate(!locator::IsInRange(actor::Target(), options.Get("ReturnToInitStateRange", 3000.0f))),
							set(stage, value(State::Init))
						),
						root(selector(
							uninterruptible(sequence(
								predicate(equals(stage, value(State::Locked))),
								parallel(
									behavior::meleeAttack(mob),
									behavior::moveTo(actor::Target(), Relative(speedMultiplier * 1.55f))
								)
							)),
							uninterruptible(sequence(
								predicate(equals(stage, value(State::Aggresive))),
								parallel(
									alwaysTrue(sequence(
										predicate(!locator::IsInRange(actor::Target(), { options.Get("AutoLockRange", 1500.0f) })),
										set(stage, value(State::Locked))
									)),
									chain(
										behavior::meleeAttack(mob),
										set(stage, [=, &options](StateRef state) {
											if (AActor* target = state.params().target) {
												return State::Standby;
											} else {
												return State::Idle;
											}
										})
									),
									behavior::moveTo(actor::Target(), Relative(speedMultiplier * 1.3f))
								)
							)),
							uninterruptible(sequence(
								predicate(equals(stage, value(State::Standby))),
								sequence(
									onStart(focus::Set(actor::Target(), EAIFocusPriority::Default)),
									onStop(focus::Clear(EAIFocusPriority::Default)),
									parallel(
										alwaysTrue(sequence(
											predicate(!locator::IsInRange(actor::Target(), { options.Get("AutoLockRange", 1500.0f) })),
											set(stage, value(State::Locked))
										)),
										chain(
											behavior::moveTo(
												move::From(
													locator::RandomReachablePointAround(
														locator::Side(actor::Target(), moveAroundRadius, clockwise),
														200.f
													)
												),
												Relative(options.Get("MoveAroundMultiplier", speedMultiplier * 1.1f))
											),
											ifElse(
												actor::IsRecentlyDamagedFromAttack(2s),
												set(stage, value(State::Aggresive)),
												parallel(
													playAnimation(options.Get("Standby"), true),
													alwaysTrue(sequence(
														predicate(canAttackTarget(actor::Target())),
														set(stage, value(State::Aggresive))
													))
												)
											)
										)
									)
								)
							)),
							uninterruptible(sequence(
								predicate(equals(stage, value(State::Idle))),
								parallel(
									sequence(
										predicate(locator::IsInRange(actor::Target(), { options.Get("MoveAroundRadiusMax", 900.0f) })),
										set(stage, choice(canAttackTarget(actor::Target()), value(State::Aggresive), value(State::Standby)))
									),
									selector(
										behavior::chaseIfAttacked(mob, Relative(speedMultiplier * 1.2f)),
										behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(speedMultiplier)),
										behavior::chaseIfWarned(mob, Relative(speedMultiplier * 1.2f)),
										behavior::defaultRoam()
									)
								)
							)),
							uninterruptible(sequence(
								predicate(equals(stage, value(State::Init))),
								[=](StateRef state) { ticketProvider(state, actorquery::getFirstActorTracked<AMeleeTicketProvider>(&state.world())); },
								set(stage, value(State::Idle))
							))
						))
					)
				),
				behavior::chaseIfAttacked(mob, Relative(speedMultiplier * 1.2f)),
				behavior::chaseIfWarned(mob, Relative(speedMultiplier * 1.2f)),
				behavior::defaultRoam()
			),
			parallel("update-targets",
				every(.8s * rnd.nextFloat(.7f, 1.3f),
					set(actor::Target(), actor::ClosestEnemy())
				),
				every(3s, sequence(
					isInRange(actor::Target(), mob.OffensiveRange),
					set(location::Anchor(), location::Self())
				))
			)
	);
}

}}}
