// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/util/Pushback.h"
#include <GameplayEffect.h>
#include "game/component/HealthComponent.h"
#include "game/Enchantments/Thorns.h"
#include "AncientGuardianThorns.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAncientGuardianThorns : public UThorns
{
	GENERATED_BODY()
public:
	UAncientGuardianThorns();
	
	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void SetEnabled(bool newState);

private:
	bool bIsEnabled = true;
};

UCLASS()
class DUNGEONS_API UAncientGuardianThornsDamageGameplayEffect : public UThornsDamageGameplayEffect {
	GENERATED_BODY()
public:
	UAncientGuardianThornsDamageGameplayEffect();
};
