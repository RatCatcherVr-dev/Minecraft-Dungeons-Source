// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "AnimaConduitMelee.generated.h"

class ASoul;

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAnimaConduitMelee : public UEnchantment
{
	GENERATED_BODY()
public:
	UAnimaConduitMelee();

	void OnStart() override;

	void OnEnd() override;	

	void OnSoulAbsorbed();

protected:
	FDelegateHandle soulAbsorbedDelegate;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealingPerSoulFactorPerLevel = 0.02f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealingPerSoulFactorBase = 0.02f;
};
