// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/actor/item/BaseProjectile.h"
#include "MultiShot.generated.h"


/**
 * 
 */
UCLASS()
class DUNGEONS_API UMultiShot : public UEnchantment
{
	GENERATED_BODY()
public:
	UMultiShot();

	FText CreateDescription() const override;

	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> projectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectilesMulticast(ABaseCharacter * characterOwner, AActor* attackTarget, const FVector& attackTargetLocation, FVector spawnLocation, FRotator rotation, TSubclassOf<ABaseProjectile> projectileClass, float ItemPower, float sourceItemPower, bool isCharged, int32 seed);

	FRotator GetNewArrowRotation(const FRotator& baseRotation, bool clockwise, float offset);

protected:
	UPROPERTY(EditDefaultsOnly)
	int ExtraArrowsWhenTriggered = 4;
};
