// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "CriticalHit.generated.h"

UCLASS()
class DUNGEONS_API UCriticalHit : public UEnchantment
{
	GENERATED_BODY()
public:
	UCriticalHit();

	FText CreateDescription() const override;
	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
	void OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CriticalDamageType;

	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 3.0f;
};


