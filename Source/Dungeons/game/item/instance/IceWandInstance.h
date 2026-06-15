// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"

#include "game/item/instance/AItemInstance.h"
#include "game/actor/FallingIceActor.h"

#include "IceWandInstance.generated.h"

UCLASS()
class DUNGEONS_API AIceWandInstance : public AItemInstance
{
	GENERATED_BODY()

public:
	AIceWandInstance();
	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const override;

protected:
	// How far away should we spawn our iceblock.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float IceSpawnDistance = 400.0f;

	// How far upward should we spawn our iceblock.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float IceSpawnHeight = 300.0f;

	// Override our falling iceblock damage
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float IceDamageOverride = 75.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AFallingIceActor> IceblockActor;
};
