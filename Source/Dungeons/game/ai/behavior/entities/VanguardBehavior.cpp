#include "Dungeons.h"
#include "VanguardBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "util/Random.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createVanguard(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto speedMultiplier = rnd.nextFloat(.4f, .6f);

	enum class State {
		March,
		Normal
	};

	const auto stage = sharedRef(State::March);
	const auto MarchSpeedMultiplier = options.Get("MarchSpeedMultiplier", .7f);
	const auto MarchDuration = options.Get("MarchDuration", 2.s);
	const auto AttackInDistance = options.Get("AttackInDistance", 250.f);
	const auto AttackOutDistance = options.Get("AttackOutDistance", 400.f);
	const auto AttackDistance = options.Get("AttackDistance", 140.f);

	return BehaviorTuple(
		selector("Vanguard",
			sequence("march",
				predicate(equals(stage, value(State::March))),
				selector(
					behavior::moveTo(move::Forward(value(500.f)), Relative(MarchSpeedMultiplier)),
					set(stage, value(State::Normal))
				),
				alwaysTrue(dropFor(MarchDuration,
					set(stage, value(State::Normal))
				))
			),
			sequence("normal",
				predicate(equals(stage, value(State::Normal))),
				selector(
					sequence("Attack",
						isInRange(
							actor::Target(),
							AttackInDistance,
							AttackOutDistance
						),
						ifElse(isInRange(actor::Target(), AttackDistance),
							sequence(
								behavior::meleeAttack(mob)
							),
							behavior::moveTo(actor::Target(), Relative(speedMultiplier))
						)
					),
					behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(speedMultiplier)),
					behavior::chaseIfAttacked(mob, Relative(1.2f)),
					behavior::chaseIfWarned(mob, Relative(1.2f)),
					behavior::defaultRoam()
				)
			)
		),
		parallel("update-targets",
			every(0.5s, set(actor::Target(), actor::ClosestEnemy(5000.f))),
			every(3s, sequence(
				isInRange(actor::Target(), mob.OffensiveRange),
				set(location::Anchor(), location::Self())
			))
		)
	);
}

}}}
