// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Swirling.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/Enchantments/EnchantmentUtil.h"
#include "util/CharacterQuery.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;

USwirlingDamageGameplayEffect::USwirlingDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee.Swirling")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Melee.Swirling"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}


USwirling::USwirling() {
	TypeId = EEnchantmentTypeID::Swirling;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return (1.0f + DamageIncreaseFactorPerLevel * (float)(level-1)) * SwirlBaseDamage;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}



void USwirling::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	if ((hitCount < MinimalHitCountToTrigger) || !attackTarget) {
		return;
	}

	const auto characterOwner = GetCharacterOwner();	
	if (const auto meleeAttackComponent = characterOwner->FindComponentByClass<UMeleeAttackComponent>()) {
		const auto& activeAttackVariants = meleeAttackComponent->GetActiveAttackVariants();

		if (activeAttackVariants.Num() > 0) {
			if ((activeAttackVariants.Num() - 1) == index) {
				OnSwirl(window);
			}
		}
	}	
}

void USwirling::OnSwirl(FSharedPredictionContext context) {
	const auto characterOwner = GetCharacterOwner();
	
	if (CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), characterOwner->GetActorLocation(), Range, 50, FColor::Green, false, 3.0f);

	if (characterOwner->HasAuthority()) {
		BroadcastEnchantmentTriggeredEvent();
	}

	FGameplayCueParameters params;
	params.RawMagnitude = Range;

	const auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Swirling"), params);

	auto targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Range);
	const auto hostile = characterquery::is::hostile(characterOwner);

	targets = targets.FilterByPredicate([&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v); });

	for (auto target : targets) {		
		pushback::pushback(Pushback, *characterOwner, *target);
		const auto mobAbilitySystem = target->GetAbilitySystemComponent();
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USwirlingDamageGameplayEffect>(abilitySystem, effects::HealthName, IsOwnerMob() ? -MobSwirlDamage : -LevelMultiplier(Level), characterOwner, characterOwner, characterOwner->GetActorLocation(), 1.f);
		FVector normal = target->GetActorLocation() - characterOwner->GetActorLocation();
		effects::StorePushbackInNormal(spec, pushback::adjustDirectionZ(Pushback, normal, *target) * Pushback.pushbackStrength);
		effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mobAbilitySystem, context.GetKey());
	}
}
