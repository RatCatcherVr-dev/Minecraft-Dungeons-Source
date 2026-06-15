// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Freezing.generated.h"

namespace FreezingMagnitudes {
	extern FName FreezingSlowingEffectDivider;
	extern FName FreezingDurationEffectMagnitude;
}

UCLASS()
class DUNGEONS_API UFreezingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFreezingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFreezingEnchantmentBase : public UEnchantment
{
	GENERATED_BODY()

public:

	UFreezingEnchantmentBase();

	FText CreateDescription() const override;

protected:

	void ApplyEffectToTarget(AActor* ToTarget, FPredictionKey PredictionKey = FPredictionKey());

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UFreezingGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SlowAmountBase = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SlowAmountPerLevel = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float FreezeTime = 3.0f;
};


UCLASS()
class DUNGEONS_API UFreezing : public UFreezingEnchantmentBase
{
	GENERATED_BODY()

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};

UCLASS()
class DUNGEONS_API UFreezingRanged : public UFreezingEnchantmentBase
{
	GENERATED_BODY()

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};

UCLASS()
class DUNGEONS_API UFreezingAoe : public UFreezingEnchantmentBase
{
	GENERATED_BODY()

	void OnAfterDealtAoeDamage(AActor* attackTarget, FRandomStream& randStream, FSharedPredictionContext context) override;
};
