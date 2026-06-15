#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include <GameplayEffect.h>
#include "DamageModCalculations.h"
#include "HealingModCalculations.generated.h"

/**
Gets the effects::HealthName base value.
Is used in conjuction with effects::CreateGameplayEffectSpec and
settings the effects::HealthName attribute to a base value.
This is a pure health modifier - it will be applied without any modifiers.
*/
UCLASS()
class DUNGEONS_API UPositiveHealthModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UPositiveHealthModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

/**
This is considered 'healing'
It is not considered to come from anywhere and will thus not be scaled by the source ability system.
*/
UCLASS()
class DUNGEONS_API UHealingModCalculation : public UPositiveHealthModCalculation {
	GENERATED_BODY()
public:
	UHealingModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:	
	const FGameplayEffectAttributeCaptureDefinition PerformHealingAttributeCapture;
};

/**
Modifies the value using
UDifficultyAttributeSet::DifficultyPerformHealingMultiplier()
Is used to make mobs deal more damage at higher difficulties.
*/
UCLASS()
class DUNGEONS_API UDifficultyHealingModCalculation : public UHealingModCalculation {
	GENERATED_BODY()
public:
	UDifficultyHealingModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition PerformHealingAttributeCapture;
};

UCLASS()
class DUNGEONS_API UMaxHealthHealingModCalculation : public UHealingModCalculation {
	GENERATED_BODY()
public:
	UMaxHealthHealingModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition MaxHealthAttributeCapture;
};

/**
Modifies the value using
UMeleeAttributeSet::MeleeHealItemPowerFactorAttribute()
*/
UCLASS()
class DUNGEONS_API UMeleeItemPowerOnlyModHealingCalculation : public UDifficultyHealingModCalculation {
	GENERATED_BODY()
public:
	UMeleeItemPowerOnlyModHealingCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition MeleeItemPowerFactorCapture;
};

/**
Modifies the value using
URangedAttributeSet::RangedHealItemPowerFactorAttribute()
*/
UCLASS()
class DUNGEONS_API URangedItemPowerOnlyModHealingCalculation : public UDifficultyHealingModCalculation {
	GENERATED_BODY()
public:
	URangedItemPowerOnlyModHealingCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition RangedItemPowerFactorCapture;
};



/**
Modifies the value using
UHealthAttributeSet::ArmorHealingItemPowerFactorAttribute()
*/
UCLASS()
class DUNGEONS_API UArmorItemPowerOnlyModHealingCalculation : public UDifficultyHealingModCalculation {
	GENERATED_BODY()
public:
	UArmorItemPowerOnlyModHealingCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition ArmorItemPowerFactorCapture;
};