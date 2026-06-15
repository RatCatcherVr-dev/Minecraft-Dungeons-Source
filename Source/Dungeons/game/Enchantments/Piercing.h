// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Piercing.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UPiercing : public UEnchantment
{
	GENERATED_BODY()
public:
	UPiercing();

	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int NumArrowsToShootUntilPiercing = 3;
};