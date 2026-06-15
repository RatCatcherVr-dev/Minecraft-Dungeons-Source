
// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "game/actor/item/BaseProjectile.h"
#include "ExplosionImpactEffect.generated.h"

UCLASS()
class DUNGEONS_API UExplosionImpactGameplayEffect : public UBaseProjectileDamageGameplayEffect {
	GENERATED_BODY()
public:
	UExplosionImpactGameplayEffect();
};