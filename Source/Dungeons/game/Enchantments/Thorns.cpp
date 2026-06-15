// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Thorns.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "GameplayEffectExtension.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include <GameplayEffectTypes.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/BeamAttackComponent.h"

UThornsDamageGameplayEffect::UThornsDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UReflectedDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Thorns"), 0, 30);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}


UThorns::UThorns() {
	TypeId = EEnchantmentTypeID::Thorns;

	LevelMultiplier = [this](int level) -> float {
		return PercentDamageReturnedBase + PercentDamageReturnedPerLevel * (level-1);
	};
	MultiplierFormatter = valueformat::asPercentage;

	
}

void UThorns::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	auto owner = GetOwner();

	if (owner->Role != ROLE_Authority)
		return;

	if (auto projectile = Cast<ABaseProjectile>(data.EffectSpec.GetContext().GetEffectCauser())) {
		if (projectile->GetDamage() <= 0.0f) {
			return;
		}
	}
	AActor* byWhom = data.EffectSpec.GetContext().GetInstigator();

	// early out if Thorns is triggered by anything but ranged or Melee attacks
	if (!Cast<UBaseProjectileDamageGameplayEffect>(data.EffectSpec.Def) && !Cast<UMeleeDamageGameplayEffect>(data.EffectSpec.Def) && !Cast<UBeamDamageGameplayEffect>(data.EffectSpec.Def))
		return;

	if (const auto character = Cast<ABaseCharacter>(byWhom)) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal | EGameplayCueParametersField::Instigator);
		auto characterOwner = GetCharacterOwner();
		UAbilitySystemComponent* ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
		UAbilitySystemComponent* targetAbilitySystem = character->GetAbilitySystemComponent();
		float damage = data.EvaluatedData.Magnitude * (owner->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : PercentDamageReturnedMob);
		
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UThornsDamageGameplayEffect>(ownerAbilitySystem, effects::HealthName, damage, characterOwner, data.EffectSpec.GetContext().GetEffectCauser(), characterOwner->GetActorLocation(), Level);
		FPredictionKey key = ownerAbilitySystem->ScopedPredictionKey;
		effects::StorePushbackInNormal(spec, pushback::getLaunchVector(OnKillPushback, *owner, *byWhom, 1.0f, 1.0f));
		effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
		ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem, key);

		BroadcastEnchantmentTriggeredEvent();
	}
}
