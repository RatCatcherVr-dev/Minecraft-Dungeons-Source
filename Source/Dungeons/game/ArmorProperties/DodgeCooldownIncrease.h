// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "DodgeCooldownIncrease.generated.h"


UCLASS()
class DUNGEONS_API UDodgeCooldownIncreaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDodgeCooldownIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDodgeCooldownIncrease : public UArmorProperty
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UDodgeCooldownIncrease();

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDodgeCooldownIncreaseGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
