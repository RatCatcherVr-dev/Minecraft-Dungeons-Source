// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Rampaging.generated.h"

UCLASS()
class DUNGEONS_API URampagingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URampagingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API URampaging : public UEnchantment
{
	GENERATED_BODY()

	URampaging();

	FText CreateDescription() const override;

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<URampagingGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AttackSpeedBoost = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BoostTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TriggerChance = 0.1f;
};
