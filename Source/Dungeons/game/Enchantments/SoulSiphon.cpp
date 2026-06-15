// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SoulSiphon.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/SoulComponent.h"
#include <AbilitySystemComponent.h>
#include "game/item/instance/MeleeWeaponGearItemInstance.h"

USoulSiphon::USoulSiphon() {
	TypeId = EEnchantmentTypeID::SoulSiphon;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseSoulSpawnAmount + PerLevelSoulSpawnAmount * (level-1);
	};
	MultiplierFormatter = valueformat::asConstant;
}

FText USoulSiphon::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)));
}

void USoulSiphon::SpawnSouls(AActor* toWhat, FRandomStream& randStream) {
	if ((randStream.FRand() < TriggerChance) || bAlwaysTrigger) {
		const auto characterOwner = GetCharacterOwner();
		if (characterOwner->HasAuthority()) {
			if (const auto soulComponent = characterOwner->FindComponentByClass<USoulComponent>()) {
				soulComponent->ServerSpawnSoul(toWhat->GetActorLocation(), LevelMultiplier(Level), 2.f, 5.f);
			}

			BroadcastEnchantmentTriggeredEvent();
		}

		FGameplayCueParameters params;
		params.NormalizedMagnitude = Level / 3.0f;
		params.Instigator = characterOwner;
		const auto abilitySystem = characterOwner->GetAbilitySystemComponent();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.SoulSiphon"), params);
	}
}
void USoulSiphon::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	SpawnSouls(toWhat, randStream);
}

void USoulSiphon::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	SpawnSouls(toWhat, randStream);
}
