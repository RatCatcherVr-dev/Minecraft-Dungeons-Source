#include "Dungeons.h"
#include "SlimeCauldronBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/ai/task/attack/formations/EvokerFormations.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/task/HealTask.h"
#include "game/ai/task/attack/RangedAttack.h"
#include "game/ai/task/attack/formations/CauldronFormations.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/task/attack/Enchant.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/task/attack/formations/ArchIllagerFormations.h"
#include "game/ai/provider/Transform.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/component/GrowAttackComponent.h"
#include "game/util/Tags.h"
#include "EnchanterBehavior.h"
#include "../../action/AnimActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createArchIllager(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	
	enum class State {		
		Waking,
		Active
	};

	const auto stage = sharedRef(State::Waking);


	return BehaviorTuple(
		selector("ArchIllager",	root(selector(
			sequence(
				predicate(equals(stage, value(State::Waking))),
				alwaysTrue(playAnimation(options.Get("Waking"), true)),
				alwaysTrue(sequence(
					dropFor(options.Get("WakeupDuration", 0.8s),
						set(stage, value(State::Active)))
				))
			),
			uninterruptible(sequence(
				startPredicate(locator::IsInRange(actor::Target(), options.Get("TeleportAwayTriggerRange", 300.f))),
				sequence(
					onStart(anim::ClearLookAtTarget()),
					onStart([](StateRef state) {
						if (auto growAttackComponent = state.owner->FindComponentByClass<UGrowAttackComponent>()) {
							growAttackComponent->Disenchant();
						}
					}),
					chain(
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", 2.s),
							options.Get("TeleportInDuration", 2.s),
							value(0.s),
							locator::RandomReachablePointAround(
								location::StartPos(),
								options.Get("TeleportAwayDistance", 900.f) - 200.f,
								options.Get("TeleportAwayDistance", 900.f)
							),
							actor::Target()
						),
						rangedAttackInOrientation(mob, actor::Target()),
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", 2.s),
							options.Get("TeleportInDuration", 2.s),
							value(0.s),
							locator::RandomReachablePointAround(
								actor::Target(),
								options.Get("TeleportAwayDistance", 900.f) - 200.f,
								options.Get("TeleportAwayDistance", 900.f)
							),
							actor::Target()
						),
						rangedAttackInOrientation(mob, actor::Target()),
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", 2.s),
							options.Get("TeleportInDuration", 2.s),
							value(0.s),
							locator::RandomReachablePointAround(
								actor::Target(),
								options.Get("TeleportAwayDistance", 900.f) - 200.f,
								options.Get("TeleportAwayDistance", 900.f)
							),
							actor::Target()
						),
						rangedAttackInOrientation(mob, actor::Target()),
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", 2.s),
							options.Get("TeleportInDuration", 2.s),
							value(0.s),
							locator::RandomReachablePointAround(
								actor::Target(),
								options.Get("TeleportAwayDistance", 900.f) - 200.f,
								options.Get("TeleportAwayDistance", 900.f)
							),
							actor::Target()
						),
						rangedAttackInOrientation(mob, actor::Target()),
						make_unique<UTeleportTask>(
							options.Get("TeleportOutDuration", 2.s),
							options.Get("TeleportInDuration", 2.s),
							value(0.s),
							locator::RandomReachablePointAround(
								actor::Target(),
								options.Get("TeleportAwayDistance", 900.f) - 200.f,
								options.Get("TeleportAwayDistance", 900.f)
							),
							actor::Target()
						)
					)
				)
			)),

			sequence(
				minTimeBetweenStopAndStart(options.Get("SummonCommonCooldown", 8.s)),
				selector(

			uninterruptible(sequence("summon-line",
				startPredicate(
					lessThan(
						actor::CountInRange(actor::Self(), 2000.f, actorquery::is::mob()),
						value(10)
					) &&
					(
						(locator::IsInRange(actor::Target(), { 900.f, 3300.f }) && actor::IsRecentlyDamagedFromAttack(2.s)) ||
						locator::IsInRange(actor::Target(), { 900.f, 1300.f })
					)
				),
				make_unique<USummonFormationTask>(
					options.Get("LineSequence"),
					options.Get("LineAnimationDuration", 2.4s),
					options.Get("LineSummonDelay", 2.s),
					options.Get("LineSummonCooldown", 5.s),
					0.s,
					0.s,
					formation::makeArchLine(
						options.Get("LineHalfCountMin", 5),
						options.Get("LineSpacing", 600.f),
						options.Get("LineOffsetFront", 1200.f)
					),
					USummonFormationTask::SummonMobUnderling,
					options.Get("LineEmergeDuration", 1.s)
				)
			)),
			uninterruptible(sequence("summon-circle",
				startPredicate(
					lessThan(
						actor::CountInRange(actor::Self(), 2000.f, actorquery::is::mob()),
						value(10)
					) &&
					locator::IsInRange(actor::Target(), { 0.f, 900.f })
				),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				make_unique<USummonFormationTask>(
					options.Get("CircleSequence"),
					options.Get("CircleAnimationDuration", 2.4s),
					options.Get("CircleSummonStart", 1.4s),
					options.Get("CircleSummonCooldown", 6.s),
					0.s,
					0.s,
					bt::formation::makeArchCircle(options.Get("CircleCountMin", 8)),
					USummonFormationTask::SummonMobUnderling,
					options.Get("CircleEmergeDuration", 1.s)
				)
			)),				
			uninterruptible(sequence("summon-single",
				startPredicate(
					lessThan(
						actor::CountInRange(actor::Self(), 2000.f, actorquery::is::mob()),
						value(10)
					) &&
					lessThan(
						actor::CountInRange(actor::Self(), 2000.f, actorquery::is::entityType(EntityType::RedstoneGolem)),
						value(2)
					) &&
					locator::IsInRange(actor::Target(), { 0.f, 600.f })
				),
				onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
				onStop(focus::Clear(EAIFocusPriority::Attacking)),
				make_unique<USummonFormationTask>(
					options.Get("SingleSequence"),
					options.Get("SingleAnimationDuration", 2.4s),
					options.Get("SingleSummonStart", 1.4s),
					options.Get("SingleSummonCooldown", 6s),
					0.s,
					0.s,
					bt::formation::makeArchSingle(),
					USummonFormationTask::SummonMobUnderling,
					options.Get("SingleEmergeDuration", 1.s)
				)
			))
			)),

			uninterruptible(sequence(
				isInRange(actor::Target(), 250.f, 300.f),
				ifElse(isInRange(actor::Target(), 140.f),
					behavior::meleeAttack(mob),
					behavior::moveTo(actor::Target(), Relative(.5f))
				)
			)),

			uninterruptible(sequence(
				startPredicate(
					locator::IsInRange(actor::Target(), options.Get("RangedTriggerRange", 1500.f))
				),
				behavior::moveTo(actor::Target(), Relative(.01f)),
				rangedAttackInOrientation(mob, actor::Target())
			)),

			uninterruptible(sequence("flee-state",
				isInRange(actor::Target(), 600.f),
				behavior::defaultFleeFrom(locator::Target())
			)),			
			uninterruptible(sequence("enchant",
				startCondition(isInRange(actor::Target(), 1200.f)),
				make_unique<UEnchant>(actor::TargetSecondary())
			)),
			behavior::defaultRoam()
		))),
		every(0.5s, selector(
			sequence(
				predicate(actor::HasTag(tags::inLove)),
				set(actor::Target(), actor::ClosestEnemy())
			),
			sequence(
				predicate(!actor::HasTag(tags::inLove)),
				parallel(
					set(actor::Target(), actor::ClosestPlayer()),
					set(actor::TargetSecondary(), actor::ClosestMob(1600.f, isBuffable))
				)
			)
		))
	);
}

}}}
