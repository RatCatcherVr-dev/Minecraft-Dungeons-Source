#include "Dungeons.h"
#include "ChillagerBehavior.h"
#include "game/actor/FallingIceActor.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
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
#include "game/ai/action/StateActions.h"
#include "game/actor/TrackingFallingIceActor.h"
#include "game/ai/action/AttackActions.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createChillager(AMobCharacter& mob, const UBehaviorOptionsComponent& options)
	{
		const auto optFallingIceClass = options.GetClass("falling-ice");

		const auto optAttackRange = options.Get("attack-range", 600.f);
		const auto optAttackCooldown = options.Get("attack-cooldown", 8s);

		const auto optChaseSpeed = options.Get("chase-speed", 1.2f);
		const auto optChaseRange = options.Get("chase-range", 1000.f);

		const auto optFleeRange = options.Get("attack-flee-range", 300.f);

		const auto optSummoningAnim = options.Get("ice-summoning-anim");

		// ----- Summon Ice Action ----- //
		auto ChillagerSummonIceBehaviour = [=, &mob]() -> Action{
			return[=, &mob](StateRef state) {

				// Try and spawn our attack cube
				if (auto target = Cast<ABaseCharacter>(actor::Target()(state))) {
					if (UWorld* world = target->GetWorld()) {
						if (ATrackingFallingIceActor* iceCube = world->SpawnActor<ATrackingFallingIceActor>(optFallingIceClass)) {
							iceCube->SetOwner(&mob);

							FVector currentLocation = target->GetActorLocation();
							currentLocation.Z += 400;
							iceCube->SetActorLocation(currentLocation);

							iceCube->SetTarget(target);

							// Set attack time
							state.params().lastAttackTime = TimeStamp::Now(state);
							return true;
						}
					}
				}

				return false;
			};
		};

		// ----- Attack Behaviour Leaf ----- //
		// ----- Calls ChillagerSummonIcePredicate ----- //
		auto ChillagerAttackBehaviorNode = [=]() {
			return sequence("chillager-summon-ice",
				startPredicate(
					!actor::HasRecentlyAttacked(optAttackCooldown) &&
					locator::IsInRange(actor::Target(), optAttackRange) &&
					actor::IsAnyPlayersInCone(PI * 0.2f)
				),
				onStart({ focus::Set(actor::Target(), EAIFocusPriority::Attacking), attack::OnBeforeCast() }),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				uninterruptible(playAnimation(optSummoningAnim)),
				ChillagerSummonIceBehaviour()
			);
		};

		// ----- Behaviour Tree ----- //
		return BehaviorTuple(
			root(selector("Chillager",
				sequence("passive",
					predicate(!locator::IsInRange(actor::Target(), optChaseRange)),
					defaultRoam()
				),
				sequence("flee",
					startCondition(isInRange(actor::Target(), optFleeRange)),
					defaultFleeFrom(locator::Target())
				),
				selector("agro",
					ChillagerAttackBehaviorNode(),
					chase(actor::Target(), FloatRange(optAttackRange, optChaseRange), Relative(optChaseSpeed))
				)
			)),
			parallel("update-targets",
				every(3s, sequence(
					locator::IsInRange(actor::Target(), optAttackRange),
					set(location::Anchor(), location::Self())
				)),
				every(0.5s, set(actor::Target(), actor::ClosestEnemy()))
			)
		);
	}

} } }
