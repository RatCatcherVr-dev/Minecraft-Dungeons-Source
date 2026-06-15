// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Knockback.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UKnockback : public UEnchantment
{
	GENERATED_BODY()
public:
	UKnockback();

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context);

protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float knockbackPower = 2.0f;
};
