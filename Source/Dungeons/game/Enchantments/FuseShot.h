// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/actor/item/BaseProjectile.h"
#include "FuseShot.generated.h"

UCLASS()
class DUNGEONS_API UFuseShot : public UEnchantment
{
	GENERATED_BODY()
public:
	UFuseShot();
protected:
	bool ShouldTriggerForCount(uint32 count) const;

	FText CreateDescription() const override;

	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	FGameplayTag ExplosionTag;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TArray<TSubclassOf<ABaseProjectile>> DeniedProjectiles;
private:	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamageExplosionDamageFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExplosionDelaySeconds = 1.0f;
};
