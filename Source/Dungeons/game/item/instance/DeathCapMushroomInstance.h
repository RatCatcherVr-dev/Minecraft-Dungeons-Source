// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameplayEffect.h>
#include "game/item/instance/AItemInstance.h"
#include "DeathCapMushroomInstance.generated.h"


UCLASS()
class DUNGEONS_API UDeathCapMushroomGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDeathCapMushroomGameplayEffect();
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API ADeathCapMushroomInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	ADeathCapMushroomInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MoveSpeedBuff = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AttackSpeedBuff = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UDeathCapMushroomGameplayEffect> Effect;
};
