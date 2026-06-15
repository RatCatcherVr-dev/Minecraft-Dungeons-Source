#include "Dungeons.h"
#include "WindCallerBehaviour.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/BtProbability.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/debug/BtDebug.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/MeleeTicketProvider.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/util/LocationQuery.h"
#include "util/Random.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "util/CharacterQuery.h"

#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/abilities/effects/UpdraftGameplayEffect.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/util/Pushback.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "game/ai/action/AttackActions.h"

class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {

	FVector markerLocation = FVector::ZeroVector;

	BehaviorTuple createWindCaller(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		
		const auto optFleeRange = options.Get("flee-range", 600.0f);
		const auto optFleeToRange = options.Get("flee-to-range", 800.0f);
		const auto optAttackRange = options.Get("attack-range", 1000.0f);
		const auto optApproachRange = options.Get("approach-range", 1500.0f);
		const auto optApproachToRange = options.Get("approach-to-range", 800.0f);

		const auto optApproachSpeed = options.Get("approach-speed", 1.2f);
		const auto optFleeSpeed = options.Get("flee-speed", 1.6f);
		const auto optFleeDuration = options.Get("flee-duration", 2.0f);

		const auto optConeDistance = options.Get("cone-distance", 500.0f);
		const auto optConeAngle = options.Get("cone-spread-radians", PI * 0.33f);

		const auto optBlastStrength = options.Get("blast-strength", 10.0f);
		const auto optBlastDamage = options.Get("blast-damage", 4.0f);
		const auto optBlastZFactor = options.Get("blast-zfactor", 0.5f);

		const auto optUpdraftStrength = options.Get("updraft-strength", 10.0f);
		const auto optUpdraftDamage = options.Get("updraft-damage", 5.0f);		
		const auto optResistanceDuration = options.Get("resistance-duration", 12.0f);
		
		const auto optUpDraftCooldown = options.Get("updraft-cooldown", 8.0f);
		const auto optWindBlastCooldown = options.Get("windblast-cooldown", 4.0f);

		const auto optUpDraftRadius = options.Get("updraft-hit-radius", 200.0f);
		const auto optWindBlastDistance = options.Get("windblast-min-distance", 400.0f);

		const auto optUpDraftProbability = options.Get("updraft-probability", 0.5f);
		const auto optUpDraftDamageDelay = options.Get("updraft-damage-delay", 0.5f);
		const auto optWindBlastProbability = options.Get("windblast-probability", 0.5f);
		const auto optWindBlastDamageDelay = options.Get("windblast-damage-delay", 0.5f);

		const Duration resistanceDurationFloat = Seconds(optResistanceDuration);

		const auto bIsAttacking = makeSharedRef<bool>(false);

		const auto getAttack = [=]() {
			return [=](StateRef state) {
				static Random rnd;
				std::vector<float> probabilities = {
					optWindBlastProbability,
					optUpDraftProbability,
				};

				float totalProbability = 0.0f;
				for (auto prob : probabilities) {
					totalProbability += prob;
				}

				float randomProbability = rnd.nextFloat(totalProbability);
				float currentProbability = 0.0f;
				int index = 0;
				for (auto prob : probabilities) {
					currentProbability += prob;
					if (randomProbability < currentProbability) {
						return index;
					}
					++index;
				}
				return 0;
			};
		};

		auto applyWindBlastEffect = [=, &mob](TArray<AActor*> targets)
		{
			auto abilitySystem = mob.GetAbilitySystemComponent();
			if (abilitySystem) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				auto damage = optBlastDamage;				
				auto spec = effects::CreateGameplayEffectSpec<UBlastGameplayEffect>(abilitySystem, effects::HealthName, -damage, &mob, &mob, mob.GetActorLocation());

				FPushback push;
				push.enablePushback = true;
				push.pushbackStrength = optBlastStrength;
				push.pushbackZFactor = optBlastZFactor;
				push.useForwardDirection = true;

				for (auto const target : targets)
				{
					ABaseCharacter* damageTakerCharacter = Cast<ABaseCharacter, AActor>(target);

					if (!damageTakerCharacter)
						return;

					auto targetAbilitySystem = damageTakerCharacter->GetAbilitySystemComponent();

					if (targetAbilitySystem) {
						auto attackHandle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);

						if (attackHandle.WasSuccessfullyApplied()) {						
							if (target->HasAuthority()) {
								pushback::pushback(push, mob, *target);
								effects::StorePushbackInNormal(spec, pushback::getLaunchVector(push, mob, *target));
							}
						}
					}
				}
			}
		};

		const auto IsTargetable = [&](const AActor* v) {
			if (auto character = Cast<ABaseCharacter>(v)) {
				return characterquery::is::hostile(character) && characterquery::is::targetable(character) && actorquery::is::alive(character);
			}
			return false;
		};
		const auto IsInvisible = [&](const AActor* v) {
			return[=](StateRef state) {
				if (auto character = Cast<ABaseCharacter>(v)) {
					auto abilitySystem = character->GetAbilitySystemComponent();
					if (abilitySystem) {
						if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible"))))
							return true;
					}
				}
				return false;
			};
		};

		auto GetMobsToTarget = [=, &mob]() -> TArray<AActor*> {
			return actorquery::getActorsInCone2D(&mob, ABaseCharacter::StaticClass(), optConeDistance, optConeAngle, IsTargetable);
		};

		auto WindBlast = [=]() -> Action{
			return[=](StateRef state) {
				state.params().lastAttackTime = TimeStamp::Now(state);
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					FGameplayCueParameters parameters;
					parameters.Location = mob->GetActorLocation();
					abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.WindCallerBlast.Attack"), parameters);

					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
					applyWindBlastEffect(GetMobsToTarget());
				}
			};
		};

		auto applyUpdraftEffect = [=, &mob](AActor* target)
		{
			auto abilitySystem = mob.GetAbilitySystemComponent();

			ABaseCharacter* damageTakerCharacter = Cast<ABaseCharacter, AActor>(target);
			if (!damageTakerCharacter)
				return;

			auto targetAbilitySystem = damageTakerCharacter->GetAbilitySystemComponent();

			if (abilitySystem && targetAbilitySystem) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				
				auto damage = optUpdraftDamage;
				auto spec = effects::CreateGameplayEffectSpec<UUpdraftGameplayEffect>(abilitySystem, effects::HealthName, -damage, &mob, &mob, mob.GetActorLocation());
				spec.SetSetByCallerMagnitude(pushback::PushbackZStrengthName, optUpdraftStrength);
				auto attackHandle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);

				if (attackHandle.WasSuccessfullyApplied()) {	
					FPushback push;
					push.enablePushback = true;
					push.pushbackStrength = optUpdraftStrength;

					if (target->HasAuthority()) {
						FVector directionVector = FVector::UpVector;
						pushback::pushback(push, directionVector, *target);
						effects::StorePushbackInNormal(spec, directionVector);
					}
				}
			}
		};

		auto PreUpdraft = [=]() -> Action{
			return[=](StateRef state) 
			{
				focus::Set(actor::Target(), EAIFocusPriority::Attacking);

				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					auto target = actor::Target()(state);
					FVector location = target->GetActorLocation();
					location += UDungeonsEffectLibrary::FindFeetOffsetForActor(target);
					markerLocation = location;

					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location);

					FGameplayCueParameters parameters;
					parameters.Location = location;
					abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Updraft.Attack"), parameters);
				}
			};
		};

		auto Updraft = [=]() -> Action{
			return[=](StateRef state) {
				state.params().lastAttackTime = TimeStamp::Now(state);
				TArray<ABaseCharacter*> targets = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(&state.world(), markerLocation, optUpDraftRadius, IsTargetable);
				for (auto target : targets) {
					applyUpdraftEffect(target);
				}
			};
		};

		auto canPerformUpdraftAttack = [=]() {
			return [=](StateRef state) {
				const auto& lastAttackTime = state.params().lastAttackTime;
				return lastAttackTime.IsPassedOrZero(state, Seconds(optUpDraftCooldown));
			};
		};

		auto canPerformWindBlastAttack = [=]() {
			return [=](StateRef state) {
				const auto& lastAttackTime = state.params().lastAttackTime;
				return lastAttackTime.IsPassedOrZero(state, Seconds(optWindBlastCooldown));
			};
		};

		auto ReturnActor = [=](AActor* actor) -> actor::Provider {
			return[=](StateRef state) {
				return actor;
			};
		};

		auto HasValidTarget = [&]() {
			return [=](StateRef state) {
				TArray< APlayerCharacter* > actors;
				UActorQuery::GetPlayerCharactersInRange(&state.world(), state.owner->GetActorLocation(), optAttackRange, actors, true);

				for (auto actor : actors)
				{
					if (IsTargetable(actor) && !IsInvisible(actor))
					{
						set(actor::Target(), ReturnActor(actor));
						return true;
					}
				}
				return false;
			};
		};		

		return BehaviorTuple(
			root(selector("WindCaller",

				sequence("flee",
					startPredicate(locator::IsInRange(actor::Target(), optFleeRange)),
					set(bIsAttacking, value(false)),
					maxRunTime(Seconds(optFleeDuration)),
					moveTo(move::LocationAwayFrom(actor::Target(), optFleeToRange), Relative(optFleeSpeed))
				),

				sequence("attack",
					predicate(locator::IsInRange(actor::Target(), optAttackRange)),
					moveTo(actor::Target(), Relative(1.f)),
					chain(
						selector(
							uninterruptibleSequence("WindBlast",
								startPredicate(locator::IsInRange(actor::Target(), optWindBlastDistance) && equals(bIsAttacking, value(false))),
								predicate(canPerformWindBlastAttack()),
								set(bIsAttacking, value(true)),
								onStart({ focus::Set(actor::Target(), EAIFocusPriority::Attacking), attack::OnBeforeCast() }),
								onStop(focus::Clear(EAIFocusPriority::Attacking)),
								minTimeBetweenStopAndStart(optWindBlastCooldown),
								playAnimation(options.Get("BlastAttack")),
								chain(
									delay(Seconds(optWindBlastDamageDelay)),
									sequence(
										parallel(
											WindBlast(),
											attack::OnAfterCast()
										),
										delay(Seconds(2.0f)),
										set(bIsAttacking, value(false))
									)
								)
							),

							uninterruptibleSequence("UpdraftAttack",
								startPredicate(equals(bIsAttacking, value(false))),
								predicate(canPerformUpdraftAttack()),
								predicate(HasValidTarget()),
								set(bIsAttacking, value(true)),
								onStart({ PreUpdraft(), attack::OnBeforeCast() }),
								onStop(focus::Clear(EAIFocusPriority::Attacking)),
								minTimeBetweenStopAndStart(optUpDraftCooldown),
								playAnimation(options.Get("UpdraftAttack")),
								chain(
									delay(Seconds(optUpDraftDamageDelay)),
									sequence(
										parallel(
											Updraft(),
											attack::OnAfterCast()
										),
										delay(Seconds(2.0f)),
										set(bIsAttacking, value(false))
									)
								)
							)
						)
					)
				),

				sequence("approach",
					predicate(
						locator::IsInRange(actor::Target(), optApproachRange) &&
						!locator::IsInRange(actor::Target(), optApproachToRange)
					),
					set(bIsAttacking, value(false)),
					maxRunTime(6.0s),
					moveTo(move::withSettings(actor::Target(),
					                          move::defaultRequest().SetAcceptanceRadius(optAttackRange - 100.0f)),
					       Relative(optApproachSpeed)
					)
				),

				sequence("random-flee",
					startPredicate(locator::IsInRange(actor::Target(), optFleeRange)),
					set(bIsAttacking, value(false)),
					maxRunTime(Seconds(optFleeDuration)),
					moveTo(move::RandomLocationAround(actor::Target(), optFleeToRange), Relative(optFleeSpeed))
				),
				defaultRoam(),
			sequence(
				every(0.5s, set(actor::Target(), actor::ClosestEnemy())),
				every(3s, set(location::Anchor(), location::Self()))
			)
	)));}
}}}
