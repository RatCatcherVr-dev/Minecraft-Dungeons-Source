// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/component/MeleeAttackComponent.h"
#include "Swirling.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API USwirling : public UEnchantment
{
	GENERATED_BODY()

	USwirling();

	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Range = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageIncreaseFactorPerLevel = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SwirlBaseDamage = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobSwirlDamage = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MinimalHitCountToTrigger = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FPushback Pushback;

private:

	void OnSwirl(FSharedPredictionContext);
};

UCLASS()
class DUNGEONS_API USwirlingDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USwirlingDamageGameplayEffect();
};
