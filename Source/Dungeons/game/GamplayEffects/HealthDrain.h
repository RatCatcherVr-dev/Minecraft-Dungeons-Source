#pragma once

#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectTypes.h"
#include "HealthDrain.generated.h"

UCLASS()
class DUNGEONS_API UPercentageHealthDrainGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPercentageHealthDrainGameplayEffect();
};

UCLASS()
class DUNGEONS_API UPercentageHealthDrainModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UPercentageHealthDrainModCalculation();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta=(AllowPrivateAccess=true))
	float PercentageDrain = 3.3f;

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	const FGameplayEffectAttributeCaptureDefinition HealthCapture;
	const FGameplayEffectAttributeCaptureDefinition MaxHealthCapture;
};