#include "Dungeons.h"
#include "RedstoneGolemBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/action/StateActions.h"
#include "game/ai/bt/BtLogic.h"
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
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/RedstoneFormations.h"
#include "util/CharacterQuery.h"


namespace bt { namespace behavior { namespace entities {

bool isPlayer(AActor* actor) {
	return actor->IsA<APlayerCharacter>();
}

BehaviorTuple createRedstoneGolem(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { 
	
	static auto isPlayer = [](AActor* actor) -> bool {		
		return actor && actor->IsA<APlayerCharacter>() && actorquery::is::alive(actor);
	};

	return BehaviorTuple(root(selector("RedstoneGolem",
		//Removed this attack because it makes the golem weaker.
		/*uninterruptible(sequence("attack-aoe",
			startPredicate(
				greaterThanOrEquals(
					actor::CountInRange(actor::Self(), options.Get("AoeMinInRange", 500.f), hostile),
					value(options.Get("AoeMinHostileCount", 3))
				)
			),
			std::make_unique<UAoeAttack>()
		)),*/
		uninterruptible(sequence(
			startPredicate(
				greaterThanOrEquals(
					actor::CountInRange(actor::Self(), options.Get("SummonMinInRange", 700.f), isPlayer),
					value(options.Get("SummonMinHostileCount", 1))
				)
			),
			make_unique<USummonFormationTask>(
				options.Get("SummonSequence"),
				options.Get("SummonDuration", 5.2s),
				options.Get("SummonStart", 1.1s),
				options.Get("SummonCooldown", 4s),
				0.s,
				0.s,
				formation::makeRedstoneDotRandom(
					options.GetClass("DotClass"),
					options.Get("RadiusMin", 2),
					options.Get("RadiusMax", 7),
					options.Get("DotRadius", 1),
					options.Get("DotDelay", 0.05),
					options.Get("SectionRadius", 2),
					options.Get("SectionDelay", 1),
					options.Get("DotCount", 7)
				),
				USummonFormationTask::SummonActor,
				options.Get("EmergeDuration", 1.5s)
			)
		)),
		sequence("Attack",
			startPredicate(
				locator::IsInRange(
					actor::Target(),
					{ options.Get("AttackInDistance", 250.f), options.Get("AttackOutDistance", 400.f) }
				)
			),
			selector(
				meleeAttack(mob, actor::Target()),
				behavior::moveTo(actor::Target(), Relative(1.f))
			)
		),
		sequence("chase",
			isInRange(actor::Target(), 2000),
			behavior::moveTo(actor::Target(), Relative(0.8f))
		),
		sequence("no-target",
			selector(
				behavior::chaseIfAttacked(mob, Relative(0.9f)),
				behavior::defaultRoam()
			)
		)
	)),
	every(0.5s,
		set(actor::Target(), actor::ClosestEnemy())
	)
); }

}}}
