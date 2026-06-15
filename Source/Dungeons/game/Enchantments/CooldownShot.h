// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "CooldownShot.generated.h"


UCLASS()
class DUNGEONS_API UCooldownShot : public UEnchantment
{
	GENERATED_BODY()
public:
	UCooldownShot();
	
	void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

private:
	UPROPERTY(EditDefaultsOnly)
	float CooldownDecreasePerLevelSeconds = 0.5f;
};
