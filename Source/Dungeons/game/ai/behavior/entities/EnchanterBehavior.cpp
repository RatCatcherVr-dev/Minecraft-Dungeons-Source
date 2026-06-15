#include "Dungeons.h"
#include "EnchanterBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/component/GrowComponent.h"
#include "game/ai/task/attack/Enchant.h"
#include "game/component/GrowAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/action/TransformActions.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/util/Tags.h"

namespace bt { namespace behavior { namespace entities {

	bool isBuffable(AActor* candidate) {
		if (candidate->ActorHasTag(tags::inLove)) {
			return false;
		}

		if (
			candidate->FindComponentByClass<UGrowComponent>() ||
			candidate->FindComponentByClass<UGrowAttackComponent>()
		) {
			return false;
		}

		static const TSet<EntityType> excludedTypes {
			EntityType::PiggyBank,
			EntityType::Necromancer,
			EntityType::RedstoneGolem,
			EntityType::RedstoneMonstrosity,
			EntityType::SlimeSmall,
			EntityType::SlimeMedium,
			EntityType::SlimeLarge,
			EntityType::OrdinaryHorse,
			EntityType::SkeletonHorseman,
			EntityType::SlimeCauldron,
			EntityType::CauldronBoss,
			EntityType::ArchIllager,
			EntityType::ArchVessel,
			EntityType::Cow,
			EntityType::Chicken,
			EntityType::Sheep,
			EntityType::Pig,
			EntityType::PiggyBank,
			EntityType::Wolf,
			EntityType::IronGolem,
			EntityType::Llama,
            EntityType::Ocelot,
            EntityType::PlayfulPanda,
            EntityType::LazyPanda,
            EntityType::BabyPanda,
            EntityType::BrownPanda,
			EntityType::QuickGrowingVine,
			EntityType::QuickGrowingVineSimple,
			EntityType::PoisonQuillVine,
			EntityType::PoisonQuillVineSimple,
			EntityType::EntangleVine,
			EntityType::SoulWizard,
			EntityType::Bee,			
			EntityType::SheepFireRed,
			EntityType::SheepPoisonGreen,
			EntityType::SheepSpeedBlue,
		};
		const auto mobCandidate = Cast<AMobCharacter>(candidate);
		return mobCandidate != nullptr &&
			mobCandidate->IsTargetable() &&
			!excludedTypes.Contains(mobCandidate->EntityType);
	}

	BehaviorTuple createEnchanter(AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
		selector("Enchanter",
			sequence(
				predicate(actor::HasTag(tags::inLove)),
				selector(
					sequence("Attack",
						isInRange(
							actor::Target(),
							options.Get("AttackInDistance", 250.f),
							options.Get("AttackOutDistance", 400.f)
						),
						ifElse(isInRange(actor::Target(), options.Get("AttackDistance", 140.f)),
							sequence(
								transform::LookAtYaw(actor::Target()),
								behavior::meleeAttack(mob)
							),
							behavior::moveTo(actor::Target(), Relative(1.1f))
						)
					),
					behavior::chaseInOffensiveRange(mob, actor::Target(), Relative(.9f))
				)
			),

			sequence(
				predicate(!actor::HasTag(tags::inLove)),
				selector(
					sequence("Attack",
						isInRange(actor::Target(), 250, 300),
						ifElse(isInRange(actor::Target(), 140),
							behavior::meleeAttack(mob),
							behavior::moveTo(actor::Target(), Relative(0.5f))
						)
					),
					sequence("enchant",
						startCondition(isInRange(actor::Target(), options.Get("EnchantRange", 1200.f))),
						make_unique<UEnchant>(actor::TargetSecondary())
					),
					sequence("flee-state",
						startCondition(isInRange(actor::Target(), options.Get("FleeDistance", 600.f))),
						behavior::defaultFleeFrom(locator::Target())
					)
				)
			),
			
			behavior::defaultRoam()
		),
		every(0.5s, selector(
			sequence(
				predicate(actor::HasTag(tags::inLove)),
				set(actor::Target(), actor::ClosestEnemy())
			),
			sequence(
				predicate(!actor::HasTag(tags::inLove)),
				parallel(
					set(actor::Target(), actor::ClosestEnemy()),
					set(actor::TargetSecondary(), actor::ClosestMob(options.Get("EnchantTargetSelectionRange", 1600.f), isBuffable))
				)
			)
		))
	); }

}}}
