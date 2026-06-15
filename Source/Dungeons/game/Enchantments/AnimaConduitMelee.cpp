// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "AnimaConduitMelee.h"
#include "game/component/SoulComponent.h"
#include "game/component/HealthComponent.h"
#include "util/CharacterQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/item/Soul.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"


UAnimaConduitMelee::UAnimaConduitMelee() {
	TypeId = EEnchantmentTypeID::AnimaConduitMelee;

	LevelMultiplier = [this](int level) -> float {
		return HealingPerSoulFactorBase + (HealingPerSoulFactorPerLevel * (level-1));
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UAnimaConduitMelee::OnStart() {
	const auto bc = Cast<ABaseCharacter>(GetOwner());

	if (bc && bc->HasAuthority()) {
		if (const auto soulComponent = bc->FindComponentByClass<USoulComponent>()) {
			soulAbsorbedDelegate = soulComponent->OnAbsorbedSoul.AddUObject(this, &UAnimaConduitMelee::OnSoulAbsorbed);
		}
	}
}

void UAnimaConduitMelee::OnEnd() {
	const auto bc = Cast<ABaseCharacter>(GetOwner());

	if (bc && bc->HasAuthority()) {
		if (const auto soulComponent = bc->FindComponentByClass<USoulComponent>()) {
			soulComponent->OnAbsorbedSoul.Remove(soulAbsorbedDelegate);
		}
	}
}

void UAnimaConduitMelee::OnSoulAbsorbed() {	
	const auto bc = Cast<ABaseCharacter>(GetOwner());

	if (bc && bc->HasAuthority()) {
		if (const auto healthComponent = bc->FindComponentByClass<UHealthComponent>()) {
			if(!healthComponent->IsHealthMaxed()){				
				const float healthPercentageHeal = LevelMultiplier(Level);
				healthComponent->ApplyHeal(healthComponent->GetMaximumHealth() * healthPercentageHeal);

				auto abilitySystem = bc->GetAbilitySystemComponent();
				abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.AnimaConduit.Absorb"), FGameplayCueParameters());

				BroadcastEnchantmentTriggeredEvent();
			}
		}
	}
}
