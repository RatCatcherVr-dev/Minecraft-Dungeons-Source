// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Leeching.generated.h"

UCLASS() 
class DUNGEONS_API ULeechingGameplayEffect : public UGameplayEffect{
	GENERATED_BODY()
public:
	ULeechingGameplayEffect();
};
/**
 * 
 */
UCLASS()
class DUNGEONS_API ULeeching : public UEnchantment
{
	GENERATED_BODY()
	
public:
	ULeeching();

	UPROPERTY(EditDefaultsOnly)
	float HealPerLevelFactor = 0.02f;

	UPROPERTY(EditDefaultsOnly)
	float BaseHealFactor = 0.05f;

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;

};
