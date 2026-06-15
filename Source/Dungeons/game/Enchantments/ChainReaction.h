// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "ChainReaction.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UChainReaction : public UEnchantment
{
	GENERATED_BODY()
public:
	UChainReaction();
	
	FText CreateDescription() const override;

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromFrojectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectilesMulticast(ABaseCharacter* characterOwner, ABaseCharacter* targetMob, TSubclassOf<ABaseProjectile> ProjectileClass, FVector actorLocation, FRotator rotation, float fRotationSpawnOffset, float DamageFactor, int8 iSpawnRecursionCount);
	
	static bool CanTrigger(const ABaseProjectile* projectile);

protected:
	UPROPERTY(Transient)
	ABaseProjectile* SourceProjectile = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int ArrowsToSpawn = 5;
};
