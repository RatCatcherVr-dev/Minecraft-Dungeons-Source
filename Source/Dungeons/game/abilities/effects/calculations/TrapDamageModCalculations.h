#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "DamageModCalculations.h"
#include "TrapDamageModCalculations.generated.h"

UCLASS()
class DUNGEONS_API UTrapDamageModCalculation : public UWorldDamageModCalculation {
	GENERATED_BODY()
public:
	UTrapDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition ResistanceCapture;
};