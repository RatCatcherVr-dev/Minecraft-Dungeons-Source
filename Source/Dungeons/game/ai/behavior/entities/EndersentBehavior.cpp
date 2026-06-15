#include "Dungeons.h"
#include "EndersentBehavior.h"
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
#include "game/ai/bt/BtDelay.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/task/attack/AoeAttack.h"
#include "game/ai/task/move/TeleportTask.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/provider/ActorStats.h"
#include "game/util/LocationQuery.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/action/StateActions.h"
#include "util/Random.h"
#include <AIController.h>
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/task/attack/SummonTask.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/VoidTouched.h"
#include "game/Enchantments/ProjectileCounter.h"
#include "game/Enchantments/DamageCounter.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createEndersent(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		static Random rnd;

		const auto optRelativeSpeed = options.Get("RelativeSpeed", 0.5f);

		const auto optTeleportChance = options.Get("TeleportChance", 0.2f);
		const auto optMinDisappearDuration = options.Get("MinDisappearDuration", 4.0f);
		const auto optMaxDisappearDuration = options.Get("MaxDisappearDuration", 8.0f);
		const auto optReappearBlocksAway = options.Get("ReappearBlocksAway", 8);

		const auto optTeleportSpawnChance = options.Get("TeleportSpawnChance", 0.35f);
		const auto optTeleportMinSpawn = options.Get("TeleportMinSpawn", 2);
		const auto optTeleportMaxSpawn = options.Get("TeleportMaxSpawn", 5);

		const auto optDeadlyEscapeHealthFractionTrigger = options.Get("DeadlyEscapeHealthFractionTrigger", 0.7f);
		const auto optDeadlyEscapeTimeWithin = options.Get("DeadlyEscapeTimeWithin", 1.5f);
		const auto optDeadlyEscapeTeleportDistance = options.Get("DeadlyEscapeTeleportDistance", 15);

		const auto optTeleportSmashDisappearDuration = options.Get("TeleportSmashDisappearDuration", 2.0s);

		const auto optVoidTouchDuration = options.Get("VoidTouchDuration", 10.0s);
		const auto optVoidTouchMultiplier = options.Get("VoidTouchMultiplier", 3.0s);

		const auto bDoTeleportSmash = makeSharedRef<bool>(false);
		const auto bDoDeadlyEscape = makeSharedRef<bool>(false);


		auto projectileEnchantment = mob.FindComponentByClass<UProjectileCounter>();
		auto damageEnchantment = mob.FindComponentByClass<UDamageCounter>();

		const auto teleportDurationProvider = [=] (StateRef state) {
			float teleportDuration = rnd.nextFloat(optMinDisappearDuration, optMaxDisappearDuration);
			bt::Seconds secs = bt::Seconds(teleportDuration);
			bt::Duration dur = bt::Duration(secs);
			return dur;
		};

		const auto randomFloatProvider = [=](StateRef state) {
			return rnd.nextFloat();
		};

		mob.SetEventMobRevealed(true);

		const auto group = game::mobspawn::sampleWithProbabilities(
			std::vector<EntityType> {
				EntityType::Endling,
				EntityType::Blastling,
				EntityType::Snareling
			},
			{ 1.f }
		);

		auto SummonMobs = [=]() {
			return sequence("SummonMobs",
				make_unique<USummonTask>(
					rnd.nextInt(optTeleportMinSpawn, optTeleportMaxSpawn),
					group,
					0.0s,
					1.0s,
					value(optTeleportMaxSpawn),
					bt::locator::RandomReachablePointAround(bt::location::Self(), 350.f)
					)
			);
		};

		auto TeleportOutCue = [=]() -> Action {
			return[=](StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					FGameplayCueParameters parameters;
					parameters.Location = mob->GetActorLocation();
					abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TeleportOut"), parameters);
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location);
				}
			};
		};

		auto TeleportInCue = [=]() -> Action {
			return[=](StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					FGameplayCueParameters parameters;
					parameters.Location = mob->GetActorLocation();
					abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.TeleportIn"), parameters);
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location);
				}
			};
		};

		const auto GetAnchorFloorLocation = []() -> locator::Provider {
			return [=](StateRef state) {
				FVector goal = state.params().startPos;
				if (const auto maybeGround = locationquery::findGround(state.world(), state.params().startPos, true)) {
					goal.Z = maybeGround.GetValue() + 500.0f;
				}
				return goal;
			};
		};

		const auto Teleport = [&]() {
			return uninterruptibleSequence("Teleport",
				startPredicate(lessThan(randomFloatProvider, value(optTeleportChance))),
				uninterruptibleSequence("Teleport",					
					ifElse(predicate(lessThan(randomFloatProvider, value(optTeleportSpawnChance))),
						chain(
							parallel(
								SummonMobs(),
								delay(0.2s)
							),
							uninterruptibleSequence(
								onStart(TeleportOutCue()),
								onStop(TeleportInCue()),
								make_unique<UTeleportTask>(
									0.0s,
									0.0s,
									teleportDurationProvider,
									locator::RandomReachablePointAround(GetAnchorFloorLocation(), optReappearBlocksAway * 100.0f),
									actor::Target()
									)
							)
						),
						uninterruptibleSequence(
							onStart(TeleportOutCue()),
							onStop(TeleportInCue()),
							make_unique<UTeleportTask>(
								0.0s,
								0.0s,
								teleportDurationProvider,
								locator::RandomReachablePointAround(GetAnchorFloorLocation(), optReappearBlocksAway * 100.0f),
								actor::Target()
								)
						)						
					)
				)
			);
		};

		auto ShouldTeleportSmash = [&projectileEnchantment]() -> bt::Pred {
			return[=](StateRef state) {
				return projectileEnchantment->HasReachedTarget();
			};
		};

		auto FinishTeleportSmash = [&projectileEnchantment]() -> bt::Action {
			return[=](StateRef state) {
				projectileEnchantment->RemoveStack();
			};
		};

		auto ShouldDeadlyEscape = [&damageEnchantment]() -> bt::Pred {
			return[=](StateRef state) {
				return damageEnchantment->HasReachedTarget();
			};
		};

		auto FinishDeadlyEscape = [&damageEnchantment]() -> bt::Action {
			return[=](StateRef state) {
				damageEnchantment->RemoveStack();
			};
		};

		auto ShakeScreen = [=]() -> bt::Action {
			return[=](StateRef state) {
				FGameplayCueParameters params;
				auto mob = state.owner;
				params.Location = mob->GetActorLocation();
				mob->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enderman.DeadlyEscape"), params);
			};
		};

		auto GetClosestPlayerToAnchor = [=]() -> actor::Provider {
			return[=](StateRef state) {
				return actor::ClosestEnemyToLocation(location::Anchor())(state);
			};
		};

		return BehaviorTuple(
			root(selector("Endersent",

				parallel("DeadlyEscape",
					sequence(
						predicate(ShouldDeadlyEscape()),
						set(bDoDeadlyEscape, value(true))
					),
					uninterruptibleSequence("DeadlyEscape",
						predicate(equals(bDoDeadlyEscape, value(true))),
						onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
						onStop(focus::Clear(EAIFocusPriority::Attacking)),
						onStop(TeleportInCue()),
						chain(
							sequence(
								std::make_unique<UAoeAttack>(actor::Self(), "DeadlyEscape"),
								delay(2.15s)
							),
							sequence(
								SummonMobs(),
								TeleportOutCue()
							),
							make_unique<UTeleportTask>(
								0.0s,
								0.0s,
								teleportDurationProvider,
								locator::RandomReachablePointAround(GetAnchorFloorLocation(), (optDeadlyEscapeTeleportDistance - 1) * 100.0f, optDeadlyEscapeTeleportDistance * 100.0f),
								actor::Target()
							),
							dropFor(2.0s,
								parallel(
									FinishDeadlyEscape(),
									set(bDoDeadlyEscape, value(false))
								)
							)
						)
					)
				),

				parallel("TeleportSmash",
					sequence(
						predicate(ShouldTeleportSmash()),
						set(bDoTeleportSmash, value(true))
					),
					uninterruptibleSequence("TeleportSmash",
						predicate(equals(bDoTeleportSmash, value(true))),
						onStart(focus::Set(actor::Target(), EAIFocusPriority::Attacking)),
						onStop(focus::Clear(EAIFocusPriority::Attacking)),
						chain(
							sequence(
								playAnimation(options.Get("TeleportSmashOut")),
								TeleportOutCue()
							),
							make_unique<UTeleportTask>(
								0.0s,
								0.0s,
								value(optTeleportSmashDisappearDuration),
								locator::RandomReachablePointAround(GetClosestPlayerToAnchor(), 180.0f),
								actor::Target()
							),
							parallel(
								TeleportInCue(),
								playAnimation(options.Get("TeleportSmashIn")),
								std::make_unique<UAoeAttack>(actor::Self(), "TeleportSmash"),
								ShakeScreen(),
								dropFor(2.0s,
									sequence(
										FinishTeleportSmash(),
										set(bDoTeleportSmash, value(false))
									)
								)
							)
						)
					)
				),

				selector("Melee",
					chain(
						sequence(
							behavior::meleeAttack(mob, actor::Target()),
							delay(2.0s)
						),
						Teleport()
					),
					behavior::moveTo(actor::Target(), Relative(optRelativeSpeed))
				)
			)),
			parallel("update-targets",
				every(0.5s, set(actor::Target(), actor::ClosestEnemy()))
			)
		);
	}
}}}
