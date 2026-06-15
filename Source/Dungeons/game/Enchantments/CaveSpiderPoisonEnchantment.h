// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "CaveSpiderPoisonEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UCaveSpiderPoisonedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCaveSpiderPoisonedGameplayEffect(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float damagePerSecond = -15.0f;
};

UCLASS()
class DUNGEONS_API UCaveSpiderPoisonEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	UCaveSpiderPoisonEnchantment();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UCaveSpiderPoisonedGameplayEffect> Effect;
};
