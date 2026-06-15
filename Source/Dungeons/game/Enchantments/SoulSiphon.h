// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "SoulSiphon.generated.h"

UCLASS()
class DUNGEONS_API USoulSiphon : public UEnchantment
{
	GENERATED_BODY()
	
public:
	USoulSiphon();

	FText CreateDescription() const override;
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	
	UPROPERTY(EditDefaultsOnly)
	int BaseSoulSpawnAmount = 2;

	UPROPERTY(EditDefaultsOnly)
	int PerLevelSoulSpawnAmount = 2;

	UPROPERTY(EditDefaultsOnly)
	float TriggerChance = 0.05f;

	void SpawnSouls(AActor* toWhat, FRandomStream& randStream);
};


