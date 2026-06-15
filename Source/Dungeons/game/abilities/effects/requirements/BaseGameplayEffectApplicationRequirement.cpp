#include "Dungeons.h"
#include "BaseGameplayEffectApplicationRequirement.h"
#include <AbilitySystemComponent.h>
#include "game/component/HealthComponent.h"


bool UBaseGameplayEffectApplicationRequirement::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const
{
	if (const auto* HC = ASC->GetOwner()->FindComponentByClass<UHealthComponent>()) {
		return HC->IsAlive();
	}

	return true;
}
