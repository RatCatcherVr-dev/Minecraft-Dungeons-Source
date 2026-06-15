// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/component/HealthComponent.h"
#include "Committed.generated.h"

UCLASS()
class DUNGEONS_API UCommitted : public UEnchantment
{
	GENERATED_BODY()

public:
	UCommitted();

	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float MobMaxDamageBonus = 3.0f;

	void CalculateEffects(const UHealthComponent* healthComponent, float& outDamageMultiplier, FGameplayTag& gameplayTag) const;
};


