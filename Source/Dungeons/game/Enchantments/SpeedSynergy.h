// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Synergy.h"
#include "SpeedSynergy.generated.h"

UCLASS()
class DUNGEONS_API USpeedSynergyGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USpeedSynergyGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API USpeedSynergy : public USynergy
{
	GENERATED_BODY()
public:
	USpeedSynergy();

	FText CreateDescription() const override;

protected:
	void OnItemSuccess() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MovementSpeedMultiplier = 1.2f;
};
