// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "Accelerating.generated.h"

UCLASS()
class DUNGEONS_API UAcceleratingBaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAcceleratingBaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UAcceleratingLevelOneGameplayEffect : public UAcceleratingBaseGameplayEffect {
	GENERATED_BODY()
public:
	UAcceleratingLevelOneGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
UCLASS()
class DUNGEONS_API UAcceleratingLevelTwoGameplayEffect : public UAcceleratingBaseGameplayEffect {
	GENERATED_BODY()
public:
	UAcceleratingLevelTwoGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
UCLASS()
class DUNGEONS_API UAcceleratingLevelThreeGameplayEffect : public UAcceleratingBaseGameplayEffect {
	GENERATED_BODY()
public:
	UAcceleratingLevelThreeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UAcceleratingMobGameplayEffect : public UAcceleratingBaseGameplayEffect {
	GENERATED_BODY()
public:
	UAcceleratingMobGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UAccelerating : public UEnchantment
{
	GENERATED_BODY()
public:
	UAccelerating();
	
	FText CreateDescription() const override;

	void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Enchantments|Accelerating")
	static float GetSpeedLimit();

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UAcceleratingBaseGameplayEffect>> EffectsForLevel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAcceleratingBaseGameplayEffect> MobEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float resetDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float resetDurationMob = 5.0f;
};
