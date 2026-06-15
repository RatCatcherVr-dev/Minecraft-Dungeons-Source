#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include <GameplayEffect.h>
#include "EnduranceModCalculations.generated.h"

/**
Uses the EnduranceRecoveryPerSecondAttribute to recover endurance.
Multiplied with .period
*/
UCLASS()
class DUNGEONS_API URecoverEndurancePerSecondModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	URecoverEndurancePerSecondModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	const FGameplayEffectAttributeCaptureDefinition EnduranceRecoveryPerSecondAttributeCapture;
};

