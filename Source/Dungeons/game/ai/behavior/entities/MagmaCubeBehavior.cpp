#include "Dungeons.h"
#include "MagmaCubeBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/component/LeapComponent.h"
#include "util/Random.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createMagmaCube(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto forwardMultiplier = options.Get("ForwardMultiplier", 0.f);
	const auto offsetRadiusMax = options.Get("OffsetRadiusMax", 0.f);

	return BehaviorTuple(dropFor(options.Get("InitialDelay", .3s) + rnd.nextFloat(-.25f, .25f), selector(
		sequence(
			predicate(locator::IsInRange(actor::Target(), options.Get("AttackInDistance", 1000.f))),
			alwaysTrue(sequence(
				predicate([&](StateRef state) { return mob.FindComponentByClass<ULeapComponent>()->CanLeap(); }),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				dropFor(options.Get("JumpDelay", .75s),
					common::Exec([&, forwardMultiplier, offsetRadiusMax](StateRef state) {
						if (const auto target = actor::Target()(state)) {
							const auto leapComponent = mob.FindComponentByClass<ULeapComponent>();
							if (AActor* targetActor = state.params().target) {
								leapComponent->Leap(Cast<ABaseCharacter>(targetActor), forwardMultiplier, offsetRadiusMax);
							}
						}
					})
				)
			))			
		),
		sequence(
			onStart(focus::Clear(EAIFocusPriority::Attacking)),
			selector(
				behavior::chaseIfAttacked(mob, Relative(1.2f)),
				behavior::chaseIfWarned(mob, Relative(1.2f)),
				behavior::defaultRoam()
			)
		)
	)),
	parallel("update-targets",
		every(0.5s, sequence(
			set(actor::Target(), actor::ClosestEnemy(3000.f))
		))
	)
); }

}}}
