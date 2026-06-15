// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "RangedDamageBoost.generated.h"

UCLASS()
class DUNGEONS_API URangedDamageBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URangedDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API URangedDamageBoost : public UArmorProperty
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	URangedDamageBoost();
protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<URangedDamageBoostGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
