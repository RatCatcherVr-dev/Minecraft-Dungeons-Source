// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "MeleeAttackSpeedBoost.generated.h"


UCLASS()
class DUNGEONS_API UMeleeAttackSpeedBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeAttackSpeedBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMeleeAttackSpeedBoost : public UArmorProperty
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UMeleeAttackSpeedBoost();
protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMeleeAttackSpeedBoostGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
