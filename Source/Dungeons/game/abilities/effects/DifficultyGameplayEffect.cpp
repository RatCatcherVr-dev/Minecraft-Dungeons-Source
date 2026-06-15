// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DifficultyGameplayEffect.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/calculations/DifficultyModCalculation.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"

UBaseDifficultyGameplayEffect::UBaseDifficultyGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;	

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;	
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;


	//Deal Damage
	FCustomCalculationBasedFloat DealDamageMagnitude;
	DealDamageMagnitude.CalculationClassMagnitude = UDifficultyMobDamageMultiplicationCalculation::StaticClass();
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UDifficultyAttributeSet::DifficultyDealDamageMultiplierAttribute();
	DamageInfo.ModifierMagnitude = DealDamageMagnitude;
	DamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(DamageInfo);



	//Perform Healing
	FCustomCalculationBasedFloat PerformHealingMagnitude;
	PerformHealingMagnitude.CalculationClassMagnitude = UDifficultyMobHealingMultiplicationCalculation::StaticClass();


	FGameplayModifierInfo	HealingInfo;
	HealingInfo.Attribute = UDifficultyAttributeSet::DifficultyPerformHealingMultiplierAttribute();
	HealingInfo.ModifierMagnitude = PerformHealingMagnitude;
	HealingInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(HealingInfo);



	//Max Endurance
	FCustomCalculationBasedFloat MaxEnduranceMagnitude;
	MaxEnduranceMagnitude.CalculationClassMagnitude = UDifficultyMobMaxEnduranceMultiplicationCalculation::StaticClass();

	FGameplayModifierInfo MaxEnduranceInfo;
	MaxEnduranceInfo.Attribute = UHealthAttributeSet::MaxEnduranceAttribute();
	MaxEnduranceInfo.ModifierMagnitude = MaxEnduranceMagnitude;
	MaxEnduranceInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(MaxEnduranceInfo);



	//Recover Endurance
	FCustomCalculationBasedFloat EnduranceRecoveryMagnitude;
	EnduranceRecoveryMagnitude.CalculationClassMagnitude = UDifficultyMobEnduranceRecoveryMultiplicationCalculation::StaticClass();


	FGameplayModifierInfo	EnduranceRecoveryInfo;
	EnduranceRecoveryInfo.Attribute = UHealthAttributeSet::EnduranceRecoveryPerSecondAttribute();
	EnduranceRecoveryInfo.ModifierMagnitude = EnduranceRecoveryMagnitude;
	EnduranceRecoveryInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(EnduranceRecoveryInfo);


	//Speed
	FCustomCalculationBasedFloat SpeedMagnitude;
	SpeedMagnitude.CalculationClassMagnitude = UDifficultyMobSpeedMultiplicationCalculation::StaticClass();

	FGameplayModifierInfo SpeedInfo;
	SpeedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	SpeedInfo.ModifierMagnitude = SpeedMagnitude;
	SpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(SpeedInfo);

	//Pushback Resistance
	FCustomCalculationBasedFloat PushbackResistanceMagnitude;
	PushbackResistanceMagnitude.CalculationClassMagnitude = UDifficultyMobPushbackMultiplicationCalculation::StaticClass();

	FGameplayModifierInfo PushbackResistanceInfo;
	PushbackResistanceInfo.Attribute = UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute();
	PushbackResistanceInfo.ModifierMagnitude = PushbackResistanceMagnitude;
	PushbackResistanceInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(PushbackResistanceInfo);


	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}


UNormalDifficultyGameplayEffect::UNormalDifficultyGameplayEffect() {
	//Max Health
	FCustomCalculationBasedFloat MaxHealthMagnitude;
	MaxHealthMagnitude.CalculationClassMagnitude = UDifficultyMobMaxHealthMultiplicationCalculation::StaticClass();

	FGameplayModifierInfo MaxHealthInfo;
	MaxHealthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	MaxHealthInfo.ModifierMagnitude = MaxHealthMagnitude;
	MaxHealthInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(MaxHealthInfo);

}

USpecialDifficultyGameplayEffect::USpecialDifficultyGameplayEffect() {
	//Max Health
	FCustomCalculationBasedFloat MaxHealthMagnitude;
	MaxHealthMagnitude.CalculationClassMagnitude = UDifficultySpecialMobMaxHealthMultiplicationCalculation::StaticClass();

	FGameplayModifierInfo MaxHealthInfo;
	MaxHealthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	MaxHealthInfo.ModifierMagnitude = MaxHealthMagnitude;
	MaxHealthInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(MaxHealthInfo);
}

UPlayerDifficultyGameplayEffect::UPlayerDifficultyGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	//Perform Healing
	FCustomCalculationBasedFloat PerformHealingMagnitude;
	PerformHealingMagnitude.CalculationClassMagnitude = UDifficultyPlayerHealingDivisionCalculation::StaticClass();

	FGameplayModifierInfo	HealingInfo;
	HealingInfo.Attribute = UHealthAttributeSet::ReceiveHealingMultiplierAttribute();
	HealingInfo.ModifierMagnitude = PerformHealingMagnitude;
	HealingInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(HealingInfo);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}