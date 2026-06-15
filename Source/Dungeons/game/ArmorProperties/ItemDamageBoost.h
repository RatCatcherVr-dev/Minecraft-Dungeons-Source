// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "ItemDamageBoost.generated.h"

UCLASS()
class DUNGEONS_API UItemDamageBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UItemDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UItemDamageBoost : public UArmorProperty
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UItemDamageBoost();

protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UItemDamageBoostGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
