// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "Dungeons.h"
#include "FallDamageEffect.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/component/HealthComponent.h"
#include "calculations/ResistanceModCalculations.h"
#include "GameplayEffectCalculation.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UFallDamageEffect::UFallDamageEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo healthInfo;
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UFallDamageModCalculation::StaticClass();

	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Fall")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Environmental")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::environmental());
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}

ULevitationFallDamageEffect::ULevitationFallDamageEffect()
{
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UFallDamageModCalculation::UFallDamageModCalculation()
	: TakeFallDamageAttributeCapture(UHealthAttributeSet::TakeFallDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(TakeFallDamageAttributeCapture);
}

float UFallDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters params;
	float rangedDamageMultiplier = 1.0f;
	GetCapturedAttributeMagnitude(TakeFallDamageAttributeCapture, Spec, params, rangedDamageMultiplier);
	
	return Super::CalculateBaseMagnitude_Implementation(Spec) * rangedDamageMultiplier;
}
