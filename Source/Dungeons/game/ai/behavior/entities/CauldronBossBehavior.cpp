#include "Dungeons.h"
#include "CauldronBossBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/attack/formations/CauldronFormations.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "util/Algo.hpp"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createCauldronBoss(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	root(selector(
		uninterruptible(sequence(
			startPredicate(
				lessThan(
					actor::CountInRange(actor::Self(), 2000.f, [owner = &mob](const AActor* actor) {
						const auto mob = Cast<AMobCharacter>(actor);

						return mob != owner
							&& actorquery::is::alive(mob)
							&& mob->EntityType != EntityType::SlimeCauldron 
							&& !mob->IsFriendlyTowardsPlayers();
					}),
					value(options.Get("SpawnTreshold", 5))
				)
			),
			make_unique<USummonFormationTask>(
				options.Get("SpawnSequence"),
				options.Get("SpawnAnimationDuration", 4.s),
				options.Get("SpawnSummonDelay", 2.s),
				options.Get("SpawnSummonCooldown", 5.s),
				0.s,
				0.s,
				formation::makeCauldronSpawn(
					options.Get("SpawnCount", 5),
					options.Get("SpawnRadiusMin", 600.f),
					options.Get("SpawnRadiusMax", 1200.f)
				),
				USummonFormationTask::SummonMob,
				options.Get("EmergeDuration", 1.s)
			)
		)),
		uninterruptible(sequence(
			startPredicate([
				poisonRadiusTriggerSquared = FMath::Square(options.Get("PoisonRadiusTrigger", 500.f)),
				poisonRatioTrigger = options.Get("PoisonRatioTrigger", .5f)
			](StateRef state) {
					const auto ownerLocation { state.owner->GetActorLocation() };
					const auto& players = InstanceTracker<APlayerCharacter>::GetList(&state.world());
					const auto insideCount = algo::count_if(players, RETLAMBDA(FVector::DistSquared2D(it->GetActorLocation(), ownerLocation) < poisonRadiusTriggerSquared));
					return players.Num()? static_cast<float>(insideCount) / static_cast<float>(players.Num()) >= poisonRatioTrigger : false;
				}
			),
			make_unique<USummonFormationTask>(
				options.Get("PoisonSequence"),
				options.Get("PosionAnimationDuration", 4.s),
				options.Get("PosionSummonDelay", 4.s),
				options.Get("PosionSummonCooldown", 10.s),
				0.s,
				0.s,
				formation::makeCauldronPoison(options.GetClass("PosionClass")),
				USummonFormationTask::SummonActor
			)
		)),
		uninterruptible(
			rangedAttackInOrientation(mob, actor::Target())
		)
	)),
	parallel("update-targets",
		every(0.5s, set(actor::Target(), actor::ClosestEnemy(3000.f)))
	)
); }

}}}
