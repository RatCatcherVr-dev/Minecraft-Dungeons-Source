// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FoodInstance.h"
#include "ChorusFruitFoodInstance.generated.h"

UCLASS()
class DUNGEONS_API AChorusFruitFoodInstance : public AFoodInstance
{
	GENERATED_BODY()
	
public:
	void Activate(const FPredictionKey& predictionKey) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TeleportRadius = 800;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TeleportChance = 0.5f;
};
