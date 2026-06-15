// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Infinity.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UInfinity : public UEnchantment
{
	GENERATED_BODY()
public:
	UInfinity();

	void OnAfterAmmoConsumed(TSubclassOf<ABaseProjectile> projectileDefinition) override;

};
