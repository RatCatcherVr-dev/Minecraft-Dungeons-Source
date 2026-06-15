// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "CooldownShot.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/EquipmentComponent.h"
#include "game/UEffectBasedCooldownProvider.h"

UCooldownShot::UCooldownShot() {
	TypeId = EEnchantmentTypeID::CooldownShot;
	ServerOnlyExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return CooldownDecreasePerLevelSeconds * level;
	};
	MultiplierFormatter = valueformat::asDurationSingleDecimalSecond;
}



void UCooldownShot::OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwnerRole() != ROLE_Authority || !charged)
		return;

	if (const auto equipmentComponent = GetCharacterOwner()->FindComponentByClass<UEquipmentComponent>()) {
		TArray < UItemSlot* > slots = equipmentComponent->GetSlotsOfType(ESlotType::ActivePermanent);

		bool hadCooldowns = false;

		for (auto slot : slots) {
			hadCooldowns = hadCooldowns || slot->IsOnCooldown();
			slot->ModifyCooldown(-LevelMultiplier(Level));
		}

		if (hadCooldowns) {
			BroadcastEnchantmentTriggeredEvent();

			auto abilitySystem = GetAbilitySystemComponent();
			FScopedPredictionWindow window(abilitySystem, FPredictionKey());
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Enchantment.CurioCharge")), FGameplayCueParameters());
		}
	}
}

