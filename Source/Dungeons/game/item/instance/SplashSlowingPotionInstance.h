// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/item/instance/ThrowableItemInstance.h"
#include "game/actor/item/SplashSlowingPotionItem.h"
#include "SplashSlowingPotionInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API ASplashSlowingPotionInstance : public AThrowableItemInstance
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASplashSlowingPotion> ClassToSpawn;
public:
	void Activate(const FPredictionKey& predictionKey) override;
};
