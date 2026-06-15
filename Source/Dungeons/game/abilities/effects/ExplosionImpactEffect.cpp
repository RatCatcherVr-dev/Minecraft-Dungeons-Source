// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "Dungeons.h"
#include "ExplosionImpactEffect.h"
#include "game/component/HealthComponent.h"

UExplosionImpactGameplayEffect::UExplosionImpactGameplayEffect() {
	InheritableGameplayEffectTags.AddTag(damageTag::explosion());
}