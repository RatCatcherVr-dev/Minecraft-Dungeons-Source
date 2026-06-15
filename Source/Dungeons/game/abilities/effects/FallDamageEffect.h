
// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "calculations/ResistanceModCalculations.h"
#include "FallDamageEffect.generated.h"

UCLASS()
class DUNGEONS_API UFallDamageEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFallDamageEffect();
};

UCLASS()
class DUNGEONS_API ULevitationFallDamageEffect : public UFallDamageEffect {
	GENERATED_BODY()
public:
	ULevitationFallDamageEffect();
};

/**
Modifies the value using
URangedAttributeSet::FallDamageMultiplerAttribute(),
*/
UCLASS()
class DUNGEONS_API UFallDamageModCalculation : public UEnvironmentalProtectionModCalculation {
	GENERATED_BODY()
public:
	UFallDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition TakeFallDamageAttributeCapture;
};