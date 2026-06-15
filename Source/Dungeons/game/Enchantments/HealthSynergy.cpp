// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "HealthSynergy.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"

UHealthSynergy::UHealthSynergy() {
	TypeId = EEnchantmentTypeID::HealthSynergy;
	LevelMultiplier = [](int level) -> float {
		return 0.02f  + (0.01f * (float)level);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UHealthSynergy::OnItemSuccess() {
	if (auto HC = GetOwner()->FindComponentByClass<UHealthComponent>()) {
		float maxHealth = HC->GetMaximumHealth();
		float healthIncrease = maxHealth * LevelMultiplier(Level);
		HC->ApplyHeal(healthIncrease);

		if (GetOwner()->HasAuthority()) {
			BroadcastEnchantmentTriggeredEvent();

			auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
			FGameplayCueParameters params;
			params.Location = GetOwner()->GetActorLocation();
			params.EffectCauser = GetOwner();
			params.Instigator = GetOwner();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.HealthSynergy"), params);
		}
	}
}