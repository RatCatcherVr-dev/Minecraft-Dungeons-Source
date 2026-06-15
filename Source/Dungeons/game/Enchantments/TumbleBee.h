// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "MobSummonRandomChanceEnchantment.h"
#include "TumbleBee.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UTumbleBee : public UMobSummonRandomChanceEnchantment
{
	GENERATED_BODY()
public:
	UTumbleBee();
protected:
	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;
};
