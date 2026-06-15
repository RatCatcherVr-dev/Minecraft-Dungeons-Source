// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Unchanting.h"
#include "game/component/MobEnchantmentComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UUnchanting::UUnchanting() {
	TypeId = EEnchantmentTypeID::Unchanting;
	LevelMultiplier = [](int level) -> float {
		return 1.25f + (0.25f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UUnchanting::OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!IsSourceItemMelee()) {
		return;
	}
	if (GetOwner()->HasAuthority()) {
		if (auto mob = Cast<AMobCharacter>(targetComponent->GetAvatarActor())) {
			if (mob->IsEnchanted()) {
				BroadcastEnchantmentTriggeredEvent();
				mutableSpec.SetSetByCallerMagnitude(effects::HealthName, mutableSpec.GetSetByCallerMagnitude(effects::HealthName) * LevelMultiplier(Level));
			}
		}
	}
}
void UUnchanting::OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (!IsSourceItemRanged()) {
		return;
	}
	if (GetOwner()->HasAuthority()) {
		if (auto mob = Cast<AMobCharacter>(toWhom)) {			
			if (mob->IsEnchanted()) {
				BroadcastEnchantmentTriggeredEvent();
				outDamage *= LevelMultiplier(Level);
			}			
		}
	}
}
