#include "Dungeons.h"
#include "SquallGolemBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/action/StateActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/RedstoneFormations.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/component/MobActivationComponent.h"
#include "util/CharacterQuery.h"
#include "game/ai/provider/Actors.h"


namespace bt { namespace behavior { namespace entities {

BehaviorTuple createSquallGolem(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

	const auto optAttackDistance = options.Get("AttackDistance", 400.0f);
	const auto optPostAttackSlowdown = options.Get("PostAttackSlowdown", 2.5s);
	const auto optAttackDuration = options.Get("AttackDuration", 1.5s);

	const auto bIsActive = makeSharedRef<bool>(true);

	auto MakeInvulnerable = [=]() -> bt::Action {
		return[=](bt::StateRef state) {
			auto mob = state.owner;
			mob->ApplyInvulnerability(-1.0f, false);
			return true;
		};
	};

	auto DisableInteraction = [=]() -> bt::Action {
		return[=](bt::StateRef state) {
			auto mob = state.owner;
			mob->SetTargetable(false);
		};
	};

	auto MakeVulnerable = [=]() -> bt::Action {
		return[=](bt::StateRef state) {
			auto mob = state.owner;
			mob->RemoveInvulnerability();
			return true;
		};
	};

	auto EnableInteraction = [=]() -> bt::Action {
		return[=](bt::StateRef state) {
			auto mob = state.owner;
			mob->SetTargetable(true);
		};
	};

	return BehaviorTuple(root(
		selector("SquallGolem",
			uninterruptibleSequence("Inactivate",
				predicate(!actor::IsActivated(actor::Self()) && equals(bIsActive, value(true))),
				parallel(
					focus::Clear(EAIFocusPriority::Attacking),
					MakeInvulnerable(),
					DisableInteraction(),
					set(bIsActive, value(false)),
					bt::state::SetHealthBarEnabled(false)
				)
			),
			uninterruptibleSequence("Activate",
				predicate(actor::IsActivated(actor::Self()) && equals(bIsActive, value(false))),
					chain(
						playAnimation(options.Get("ActivateAnimation")),
						delay(0.2s),
						parallel(
							set(bIsActive, value(true)),
							MakeVulnerable(),
							EnableInteraction(),
							bt::state::SetHealthBarEnabled(true)
						)
					)
			),
			sequence(
				chain(
					sequence(
						predicate(equals(bIsActive, value(true))),
						ifElse(locator::IsInRange(actor::Target(), optAttackDistance),
							sequence(
								onStop(focus::Clear(EAIFocusPriority::Attacking)),
								uninterruptibleSequence(
									focus::Set(actor::Target(), EAIFocusPriority::Attacking),
									behavior::aoeAttack(mob),
									delay(optAttackDuration)
								)
							),
							sequence(
								ifElse(actor::HasRecentlyAttacked(optPostAttackSlowdown),
									behavior::moveTo(actor::Target(), Relative(0.5f)),
									behavior::moveTo(actor::Target(), Relative(1.2f))
								)
							)
						)
					)
				)
			)
		)
	),
		every(0.5s,
			set(actor::Target(), actor::ClosestEnemy())
		)
	); }

}}}
