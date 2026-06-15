// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "HuntingBowEnchantment.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UHuntingBowEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	UHuntingBowEnchantment();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

};
