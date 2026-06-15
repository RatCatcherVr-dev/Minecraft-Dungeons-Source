// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/component/GrowingArrowComponent.h"
#include "Growing.generated.h"

UCLASS()
class DUNGEONS_API UGrowing : public UEnchantment
{
	GENERATED_BODY()
public:
	UGrowing();

	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;
	void OnResetRangedProjectile(ABaseProjectile* fromProjectile) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	class USoundBase* GrowingSound = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float MaxDistance = 1600.0f;

	UPROPERTY(EditDefaultsOnly)
	float MobMaxDistance = 1200.0f;

	UPROPERTY(EditDefaultsOnly)
	float MobDamageFractionBonus = 3.0f;
};
