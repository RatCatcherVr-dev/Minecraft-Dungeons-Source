// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffectExtension.h>
#include "Recycler.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API URecycler : public UEnchantment
{
	GENERATED_BODY()
public:
	URecycler();

	virtual void OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData &data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) override;
	
private:
	int AbsorbedCount = 0;
};
