// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "StrengthPotionInstance.generated.h"


UCLASS()
class DUNGEONS_API UStrengthPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UStrengthPotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API AStrengthPotionInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AStrengthPotionInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float StrengthBoostAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UStrengthPotionGameplayEffect> Effect;
};
