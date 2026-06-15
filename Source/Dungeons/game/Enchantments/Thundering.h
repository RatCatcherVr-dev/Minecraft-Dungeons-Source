// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/component/MeleeAttackComponent.h"
#include "Thundering.generated.h"


UCLASS()
class DUNGEONS_API UThundering : public UEnchantment
{
	GENERATED_BODY()
public:
	UThundering();
	FText CreateDescription() const override;
protected:
	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext context) override;

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;

	// Main strike radius
	UPROPERTY(EditDefaultsOnly)
	float StrikeRadius = 400.0f;

	// Area tested on each mob affected by chain lightning to see if lightning can propagate to another mob
	UPROPERTY(EditDefaultsOnly)
	float IndividualChainRadius = 600.0f;

	// Time between chain lightning
	UPROPERTY(EditDefaultsOnly)
	float ChainLightningDelay = 0.1f;

	// Damage of everything
	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 12.5f;

	// Damage of everything
	UPROPERTY(EditDefaultsOnly)
	float MobDamage = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TriggerChance = 0.3f;

private:

	bool bCanTriggerThundering = false;
};

UCLASS()
class DUNGEONS_API UThunderingAoeDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UThunderingAoeDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UThunderingAoeRangedDamageGameplayEffect : public UThunderingAoeDamageGameplayEffect {
	GENERATED_BODY()
public:
	UThunderingAoeRangedDamageGameplayEffect();
};