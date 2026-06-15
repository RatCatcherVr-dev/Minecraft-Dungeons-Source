// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayModMagnitudeCalculation.h"
#include "game/Enchantments/Enchantment.h"
#include "PainCycle.generated.h"

UCLASS()
class DUNGEONS_API UPainCycleHealthDrainModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UPainCycleHealthDrainModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

	static const FName DrainKey;
	
protected:
	const FGameplayEffectAttributeCaptureDefinition HealthAttributeCapture;
	const FGameplayEffectAttributeCaptureDefinition MaxHealthAttributeCapture;
};

UCLASS()
class DUNGEONS_API UPainCycleStackerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPainCycleStackerGameplayEffect();

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int MaximumPainCycleStacks = 4;
};

UCLASS()
class DUNGEONS_API UPainCycleDrainGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPainCycleDrainGameplayEffect();
};

UCLASS()
class DUNGEONS_API UPainCycleBuffGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPainCycleBuffGameplayEffect();
};


UCLASS()
class DUNGEONS_API UPainCycle : public UEnchantment
{
	GENERATED_BODY()
public:
	UPainCycle();

	void OnEnd() override;
	FText CreateDescription() const override;
	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UPainCycleStackerGameplayEffect> TargetEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UPainCycleDrainGameplayEffect> DrainEffect;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int MinimumBonusDamageMultiplier = 3;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int BonusDamageMultiplierPerExtraLevel = 1;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float DrainMagnitude = 0.05f;
};
