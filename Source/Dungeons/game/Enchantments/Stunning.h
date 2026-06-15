// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "Stunning.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UStunning : public UEnchantment
{
	GENERATED_BODY()

public:
	UStunning();

protected:
	UPROPERTY(EditDefaultsOnly)
	float ChanceToStunPerLevel = 0.05f;
	
	UPROPERTY(EditDefaultsOnly)
	float DurationSeconds = 2.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStaggerGameplayEffect> Effect = UStaggerGameplayEffect::StaticClass();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;

private:
	bool IsNotAlive(AActor* toWhat) const;
};
