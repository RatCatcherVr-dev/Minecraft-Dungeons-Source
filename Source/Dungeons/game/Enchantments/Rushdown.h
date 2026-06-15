// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Rushdown.generated.h"



UCLASS()
class DUNGEONS_API URushdownGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URushdownGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API URushdown : public UEnchantment
{
	GENERATED_BODY()
public:
	URushdown();

protected:
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
	void BoostSpeed();

	UPROPERTY(EditDefaultsOnly)
	float speedBonus = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float speedDurationAfterKill = 0.75f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<URushdownGameplayEffect> Effect;
};
