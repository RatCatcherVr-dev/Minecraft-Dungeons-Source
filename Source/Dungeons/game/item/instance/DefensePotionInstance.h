// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "DefensePotionInstance.generated.h"


UCLASS()
class DUNGEONS_API UDefensePotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDefensePotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API ADefensePotionInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	ADefensePotionInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DefenseBoostAmount = 1.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDefensePotionGameplayEffect> Effect;
};
