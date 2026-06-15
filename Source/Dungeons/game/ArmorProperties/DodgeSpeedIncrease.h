// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "DodgeSpeedIncrease.generated.h"


UCLASS()
class DUNGEONS_API UDodgeSpeedIncreaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDodgeSpeedIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDodgeSpeedIncrease : public UArmorProperty
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UDodgeSpeedIncrease();

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDodgeSpeedIncreaseGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
