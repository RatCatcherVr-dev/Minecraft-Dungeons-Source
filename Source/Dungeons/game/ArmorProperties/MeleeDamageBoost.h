// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "MeleeDamageBoost.generated.h"

 
UCLASS()
class DUNGEONS_API UMeleeDamageBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMeleeDamageBoost : public UArmorProperty
{
	GENERATED_BODY()
	
public:	
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UMeleeDamageBoost();
protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMeleeDamageBoostGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
