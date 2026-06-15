#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "ItemPowerModCalculations.generated.h"

UCLASS()
class DUNGEONS_API UItemPowerMeleeDamageModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerMeleeHealingModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerRangedDamageModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerRangedHealingModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerHealthModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerArmorDamageModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UItemPowerArmorHealingModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
