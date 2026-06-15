#pragma once

#include <GameplayEffectExecutionCalculation.h>
#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "DamageModCalculations.h"
#include "StatusModCalculations.generated.h"

UENUM()
enum class EStatusEffectType : uint8 {
	UNTAGGED,
	POSITIVE,
	NEGATIVE
};

// ----- Generic Mod ----- //
UCLASS()
class DUNGEONS_API UStatusModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UStatusModCalculation();
};

// ----- Tag Determined Duration Mod ----- //
UCLASS()
class DUNGEONS_API UStatusDurationModCalculation : public UStatusModCalculation {
	GENERATED_BODY()
public:
	UStatusDurationModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition PositiveDurationCapture;
	const FGameplayEffectAttributeCaptureDefinition NegativeDurationCapture;
};