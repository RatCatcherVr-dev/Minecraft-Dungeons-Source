// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "ProjectileCounter.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UProjectileCounter : public UEnchantment
{
	GENERATED_BODY()
public:
	UProjectileCounter();

	void BeginPlay() override;

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void OnCountChanged(const FGameplayTag Tag, int32 Count);

	bool HasReachedTarget();
	void RemoveStack();

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileCounter")
	int targetCount = 15;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileCounter")
	float effectDuration = 3.5f;

private:
	bool hasReachedTarget = false;

	UAbilitySystemComponent* abilitySystem;
	FActiveGameplayEffectHandle handle;
};


UCLASS()
class DUNGEONS_API URangedHitGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URangedHitGameplayEffect();

	static const FName EffectDurationName;
};
