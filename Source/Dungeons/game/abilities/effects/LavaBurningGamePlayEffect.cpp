// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/DamageModCalculations.h"
#include "executions/DamageExecutionCalculation.h"
#include <GameplayEffect.h>
#include "LavaBurningGamePlayEffect.h"
#include "game/component/HealthComponent.h"
#include "calculations/ResistanceModCalculations.h"
#include "calculations/StatusModCalculations.h"

ULavaBurningGamePlayEffect::ULavaBurningGamePlayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.2f;

	FGameplayModifierInfo info;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UEnvironmentalProtectionModCalculation::StaticClass();
	healthMagnitude.Coefficient = Period.GetValueAtLevel(1) * DamagePerSecond;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = healthMagnitude;
	healthModifier.Attribute = UHealthAttributeSet::HealthAttribute();	
	Modifiers.Add(healthModifier);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak.Fire"));
	const auto environmentalDamageTag = damageTag::environmental();
	InheritableGameplayEffectTags.AddTag(weakDamageTag);
	InheritableGameplayEffectTags.AddTag(environmentalDamageTag);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();	
	damage.PassedInTags.AddTag(weakDamageTag);
	Executions.Add(std::move(damage));


	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Burning"), 0, 1);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Environmental")));
	const auto burningEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Fire.Burning");
	InheritableOwnedTagsContainer.AddTag(burningEffectTag);
	InheritableGameplayEffectTags.AddTag(burningEffectTag);		
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"));
}