// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/util/Pushback.h"
#include <GameplayEffect.h>
#include "../component/HealthComponent.h"
#include "Thorns.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UThorns : public UEnchantment
{
	GENERATED_BODY()

protected:
	UThorns();

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	UPROPERTY(EditDefaultsOnly)
	float PercentDamageReturnedBase = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float PercentDamageReturnedPerLevel = 0.5f;	

	UPROPERTY(EditDefaultsOnly)
	float PercentDamageReturnedMob = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	FPushback OnKillPushback;
};

UCLASS()
class DUNGEONS_API UThornsDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UThornsDamageGameplayEffect();
};
