#include "Dungeons.h"
#include "WolfBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/util/Tags.h"
#include "game/mobspawn/MobGroupUtil.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createMobSpawner(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto group = game::mobspawn::sampleWithProbabilities(
		std::vector<EntityType> {
			EntityType::Zombie,
			EntityType::BabyZombie,
			EntityType::Husk,
			EntityType::ChickenJockey,
			EntityType::Skeleton
		},
		{ 1.f }
	);

	return BehaviorTuple(
		sequence("MobSpawner",
			sequence("summon-if-close-enough",
				predicate(!actor::HasTag(tags::inLove)),
				isInRange(actor::Target(), options.Get("SpawnTriggerRange", 1250.f)),
				make_unique<USummonTask>(
					rnd.nextInt(options.Get("SpawnCountMin", 5), options.Get("SpawnCountMax", 10) + 1),
					group,
					0.5s,
					4.s
				)
			)
		),
		every(0.5s, set(actor::Target(), actor::ClosestEnemy()))
	);
}

}}}
