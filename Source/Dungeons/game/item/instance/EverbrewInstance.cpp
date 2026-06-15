#include "Dungeons.h"
#include "EverbrewInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/util/ValueFormat.h"
#include "game/component/HealthComponent.h"
#include "game/Enchantments/Regeneration.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

AEverbrewInstance::AEverbrewInstance() {
	PrimaryActorTick.bCanEverTick = false;
	PowerEffects = { UHealingIncrease::StaticClass() };
}

void AEverbrewInstance::ApplyEquippedEffects() {
	Super::ApplyEquippedEffects();
	if (HasAuthority()) {
		ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
		auto ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
		const float ScaledHealAmountPerSecond = HealAmountPerSecond * ItemPowerMultiplier;

		if (ScaledHealAmountPerSecond <= 0.0f) return;

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<URegenerationGameplayEffect>(abilitySystem, ItemPower);
		spec.Period = FMath::Max(spec.Period, valueformat::getPeriodForAnyDisplayHealthChange(ScaledHealAmountPerSecond));
		spec.SetSetByCallerMagnitude(UHealthPerSecondCalculation::HealthPerSecondName, ScaledHealAmountPerSecond);
		handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void AEverbrewInstance::RemoveEquippedEffects() {
	Super::RemoveEquippedEffects();
	if (handle.IsValid()) {
		ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
		abilitySystem->RemoveActiveGameplayEffect(handle);
	}
}

int AEverbrewInstance::GetDisplayCount() const {
	return 0;
}

bool AEverbrewInstance::CanActivate() const {
	return true;
}