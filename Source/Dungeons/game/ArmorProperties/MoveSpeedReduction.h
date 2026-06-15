// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "MoveSpeedReduction.generated.h"

UCLASS()
class DUNGEONS_API UMoveSpeedReductionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMoveSpeedReductionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UMoveSpeedReduction : public UArmorProperty
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UMoveSpeedReduction();

protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMoveSpeedReductionGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
