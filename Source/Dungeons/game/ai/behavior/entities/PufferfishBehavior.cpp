#include "Dungeons.h"
#include "PufferfishBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/provider/Move.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/bt/BtDelay.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/task/attack/MeleeAttack.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/animinstance/PufferfishAnimInstance.h"

namespace bt {	namespace behavior { namespace entities {
	BehaviorTuple createPufferfish(AMobCharacter& mob, const
		UBehaviorOptionsComponent& options) {
		
		enum class State
		{
			Normal,
			Inflated,
		};

		auto SetPufferfishInflate = [=, &mob](bool bInflate) -> bt::Action {
			return[=, &mob](StateRef state) {
				if (UPufferfishAnimInstance* PufferfishAnim = Cast<UPufferfishAnimInstance>(mob.GetMesh()->GetAnimInstance()))
				{
					PufferfishAnim->bInflate = bInflate;
				};
			};
		};

		const auto stage = sharedRef(State::Normal);
		
		return BehaviorTuple(

			selector("pufferfish",
				sequence(
					predicate((equals(stage, value(State::Inflated)) && locator::IsInRange(actor::Target(), options.Get("AttackRange", 110.f)) && !actor::HasRecentlyAttacked(2.0s))),
					make_unique<UMeleeAttack>(actor::Target(), false)
				),
				sequence(
					predicate(actor::IsRecentlyDamagedFromAttack(2s)),
					make_unique<UMeleeAttack>(actor::LastAttacker(), false)
				),
				behavior::defaultRoam()
				),
			parallel("UpdateTargetsAndAnims",
				every(.5s,
					sequence(
						set(actor::Target(), actor::ClosestEnemy(3000, false)),
						sequence("Animation",
							ifElse(predicate(locator::IsInRange(actor::Target(), options.Get("MinInflateDistance", 500.f))),
								sequence(
									set(stage, value(State::Inflated)),
									SetPufferfishInflate(true)
								),
								sequence(
									set(stage, value(State::Normal)),
									SetPufferfishInflate(false)
								)
							)
						)
					)
				)
			)
		);
	}
}}}
