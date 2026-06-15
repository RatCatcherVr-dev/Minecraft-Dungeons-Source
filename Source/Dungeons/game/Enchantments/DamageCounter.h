// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "DamageCounter.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDamageCounter : public UEnchantment
{
	GENERATED_BODY()
public:
	UDamageCounter();

	void BeginPlay() override;

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void OnCountChanged(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 PreviousCount);

	bool HasReachedTarget();
	void RemoveStack();

	void HandleGameplayEffectAdded(UAbilitySystemComponent* abilitySystemComp, const FGameplayEffectSpec& effectSpec, FActiveGameplayEffectHandle effectHandle);

	UPROPERTY(EditDefaultsOnly, Category = "DamageCounter")
	float healthLossTriggerThreshold = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageCounter")
	float effectDuration = 3.5f;

	UPROPERTY(EditDefaultsOnly, Category = "DamageCounter")
	FGameplayTag damageTypeToTrack = FGameplayTag::RequestGameplayTag("Damage.Melee");

private:
	float cumulativeDamage = 0.0f;
	bool hasReachedTarget = false;

	float maxHealth = 0.0f;

	TArray<float> damageArray;

	UAbilitySystemComponent* abilitySystem;
	FActiveGameplayEffectHandle handle;

	bool firstHit = false;
};


UCLASS()
class DUNGEONS_API UDamageCounterGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDamageCounterGameplayEffect();

	static const FName EffectDurationName;
};
