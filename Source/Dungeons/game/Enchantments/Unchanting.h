// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Unchanting.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UUnchanting : public UEnchantment
{
	GENERATED_BODY()
	
public:
	UUnchanting();

	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);
	void OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};
