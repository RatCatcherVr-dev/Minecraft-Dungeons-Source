// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "FireworkArrow.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AFireworkArrow : public AItemInstance
{
	GENERATED_BODY()

public:
	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;
	
	UFUNCTION()
	void OnArrowFired();
};
