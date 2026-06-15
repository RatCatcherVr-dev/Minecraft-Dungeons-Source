#include "Dungeons.h"
#include "ShulkerBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/behavior/MeleeAttackBehavior.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/ChaseAndMeleeAttackBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/actor/ShulkerBullet.h"
#include "game/ai/action/FocusActions.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "Engine/Engine.h"
#include "game/Enchantments/Deflecting.h"
#include "game/component/EnchantmentComponent.h"
#include "Sound/SoundCue.h"

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createShulker(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		static Random rnd;

		enum class State {
			Aggressive,
			Defending
		};

		const auto stage = sharedRef(State::Aggressive);

		const auto bulletClass = options.GetClass("shulker bullet");

		const auto defendMin = options.Get("Defense Min", 3.0f);
		const auto defendMax = options.Get("Defense Max", 8.0f);
		const auto defensiveCooldown = options.Get("Defense Cooldown", 5.0s);

		const auto meleeDamageMultiplier = options.Get("Defend Melee Damage Multiplier", 0.2f);
		const auto rangeDamageMultiplier = options.Get("Defend Range Damage Multiplier", 0.f);

		bt::TimeStamp lastDefendedTime;

		const auto defendDurationProvider = [=] (bt::Duration defendMin, bt::Duration defendMax) {
				return ((defendMax - defendMin) * rnd.nextFloat(0.0f, 1.0f)) + defensiveCooldown;
			};

		auto SpawnBullet = [=, &mob]() -> bt::Action {
			return[=, &mob](bt::StateRef state) {

				// Try and spawn our attack cube
				if (auto target = Cast<ABaseCharacter>(actor::Target()(state))) {
					if (UWorld* world = target->GetWorld()) {
						if (AShulkerBullet* bullet = world->SpawnActor<AShulkerBullet>(bulletClass)) {
							bullet->SetOwner(&mob);

							FVector currentLocation = mob.GetActorLocation();

							bullet->SetActorLocation(currentLocation);

							bullet->SetTarget(target);

							// Set attack time
							state.params().lastAttackTime = bt::TimeStamp::Now(state);
							return true;
						}
						return true;
					}
				}

				return false;
			};
		};

		float latestDefensiveCooldown = FMath::FRandRange(defendMin, defendMax);

		auto applyGuard = [=]() -> bt::Action {
			return [=](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {
					auto spec = effects::CreateGameplayEffectSpec<UShulkerGuardGameplayEffect>(abilitySystem);
					spec.SetSetByCallerMagnitude(UShulkerGuardGameplayEffect::MeleeDamageTakenName, meleeDamageMultiplier);
					spec.SetSetByCallerMagnitude(UShulkerGuardGameplayEffect::RangeDamageTakenName, rangeDamageMultiplier);
					spec.SetSetByCallerMagnitude("Duration", latestDefensiveCooldown);
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
				}

				const auto enchantmentComponent = mob->FindComponentByClass<UEnchantmentComponent>();
				if (enchantmentComponent)
				{
					if (!enchantmentComponent->HasEnchantment(EEnchantmentTypeID::Deflecting))
					{
						enchantmentComponent->AddEnchantments({ { EEnchantmentTypeID::Deflecting, 1 } }, createEnchantmentSpawnConfig(enchantmentComponent->GetOwner()).SetTreatAsInherited());

					}
					UDeflecting* enchant = mob->FindComponentByClass<UDeflecting>();
					enchant->bAlwaysTrigger = true;
				}

				if (mob->HurtAudioComponent)
				{
					mob->HurtAudioComponent->SetSound(mob->blockSound);
				}
				else
				{
					mob->HurtAudioComponent = UGameplayStatics::SpawnSoundAttached(mob->blockSound, mob->GetRootComponent(), NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1.f, 1.f, 0.f, nullptr, nullptr, false);
				}
			};
		};

		const auto removeGuard = [=, &lastDefendedTime]() -> bt::Action {
			return [=, &lastDefendedTime](bt::StateRef state) {
				auto mob = state.owner;
				auto abilitySystem = mob->GetAbilitySystemComponent();
				if (abilitySystem) {

					FGameplayEffectQuery query;
					query.EffectDefinition = UShulkerGuardGameplayEffect::StaticClass();
					abilitySystem->RemoveActiveEffects(query);
				}

				const auto enchantmentComponent = mob->FindComponentByClass<UEnchantmentComponent>();
				if (enchantmentComponent)
				{
					enchantmentComponent->RemoveEnchantments();
				}

				if (mob->HurtAudioComponent)
				{
					mob->HurtAudioComponent->SetSound(mob->hurtSound);
				}
				else
				{
					mob->HurtAudioComponent = UGameplayStatics::SpawnSoundAttached(mob->hurtSound, mob->GetRootComponent(), NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1.f, 1.f, 0.f, nullptr, nullptr, false);
				}
				state.params().lastDefenseTime = bt::TimeStamp::Now(state);
			};
		};

		const auto DefendTriggerRange = options.Get("DefendTriggerRange", 200.f);
		const auto attack_cooldown = options.Get("attack-cooldown", 4s);
		const auto attack_range = options.Get("attack-range", 1000.f);

		const FString Close = "Close";
		const FString Attack = "Attack";
		const FString Defend = "Defend";
		const FString Open = "Open";

		return BehaviorTuple(
			root(selector("Shulker",
				sequence("Aggressive",
					predicate(equals(stage, value(State::Aggressive))),
					selector(
						sequence("defend",
							predicate(
								actor::IsRecentlyDamagedFromAttack(.5s) ||
								locator::IsInRange(actor::Target(), DefendTriggerRange)
							),
							predicate(!actor::HasRecentlyDefended(defensiveCooldown)),
							applyGuard(),
							sequence(
								uninterruptible(playAnimation(options.Get(Close), false))
							),
							set(stage, value(State::Defending))
						),
						sequence("attack",
							predicate(
								!actor::HasRecentlyAttacked(attack_cooldown) &&
								locator::IsInRange(actor::Target(), attack_range)),
							uninterruptible(playAnimation(options.Get(Attack), false)),
							SpawnBullet()
						)						
					)
				),
				chain(
					sequence("Defending",
						predicate(equals(stage, value(State::Defending))),
						onStop(set(stage, value(State::Aggressive))),
						onStop(removeGuard()),
						alwaysTrue(playAnimation(options.Get(Defend), true)),
						chain(
							delay(Seconds(latestDefensiveCooldown)),
							sequence(
								uninterruptible(playAnimation(options.Get(Open), false))
							)
						)
					)
				)
			)),
			parallel("update-targets",
				every(0.5s, set(actor::Target(), actor::ClosestEnemy(5000.f))),
				every(3s, sequence(
					isInRange(actor::Target(), mob.OffensiveRange),
					set(location::Anchor(), location::Self())
				))
			)
		);
	}
}
}
}

const FName UShulkerGuardGameplayEffect::MeleeDamageTakenName = FName("MeleeDamageTaken");
const FName UShulkerGuardGameplayEffect::RangeDamageTakenName = FName("RangeDamageTaken");

UShulkerGuardGameplayEffect::UShulkerGuardGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName(TEXT("Duration"));
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo takeMeleeDamageInfo;
	takeMeleeDamageInfo.Attribute = UHealthAttributeSet::TakeMeleeDamageMultiplierAttribute();
	FSetByCallerFloat takeMeleeDamageMagnitude;
	takeMeleeDamageMagnitude.DataName = MeleeDamageTakenName;
	takeMeleeDamageInfo.ModifierMagnitude = takeMeleeDamageMagnitude;
	takeMeleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(takeMeleeDamageInfo);

	FGameplayModifierInfo takeRangeDamageInfo;
	takeRangeDamageInfo.Attribute = UHealthAttributeSet::TakeRangeDamageMultiplierAttribute();
	FSetByCallerFloat takeRangeDamageMagnitude;
	takeRangeDamageMagnitude.DataName = RangeDamageTakenName;
	takeRangeDamageInfo.ModifierMagnitude = takeRangeDamageMagnitude;
	takeRangeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(takeRangeDamageInfo);
}
