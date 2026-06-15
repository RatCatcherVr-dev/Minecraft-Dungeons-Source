// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "EffectApplyingEnchantment.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"

void UEffectApplyingEnchantment::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec = FGameplayEffectSpec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), Level);
	OnPreSpecApplication(spec);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UEffectApplyingEnchantment::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}
