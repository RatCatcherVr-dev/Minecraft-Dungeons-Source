// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "BootsOfSwiftnessInstance.generated.h"

UCLASS()
class DUNGEONS_API UBootsOfSwiftnessGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBootsOfSwiftnessGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API ABootsOfSwiftnessInstance : public AItemInstance
{
	GENERATED_BODY()

	ABootsOfSwiftnessInstance();

	int GetDisplayCount() const override;
	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "Dungeons")
	float SpeedBoostAmount = 1.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBootsOfSwiftnessGameplayEffect> Effect;
};