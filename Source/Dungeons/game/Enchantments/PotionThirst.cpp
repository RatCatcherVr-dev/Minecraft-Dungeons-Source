#include "PotionThirst.h"
#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include <GameplayEffect.h>
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UPotionThirstMelee::UPotionThirstMelee() {
	TypeId = EEnchantmentTypeID::PotionThirstMelee;

	LevelMultiplier = [this](int level) -> float {
		return cooldownReductionBase + cooldownReductionPerLevel * float(level) - cooldownReductionPerLevel;
	};
	MultiplierFormatter = valueformat::asDurationSingleDecimalSecond;
}

void UPotionThirstMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (!missedAttack && GetOwner()->HasAuthority()) {
		if (ABaseCharacter* attackTarget = Cast<ABaseCharacter>(toWhom)) {
			if (attackTarget->IsNotAlive()) {
				ApplyEffect();
			}
		}
	}
}

void UPotionThirstMelee::ApplyEffect() {
	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetCharacterOwner())) {
		if (UEquipmentComponent* equipmentComponent = playerCharacter->GetEquipmentComponent()) {
			if (UItemSlot* healthPotionSlot = equipmentComponent->GetFirstSlotOfType(ESlotType::HealthPotion)) {
				TPair<float, float> cooldown = healthPotionSlot->GetCooldownSecondsRemainingAndDuration();
				if (cooldown.Key > 0.0f) {
					cooldown.Key = std::max(cooldown.Key - LevelMultiplier(Level), 0.0f);
					float cooldownReduction = healthPotionSlot->GetCooldownSecondsReduction() + (cooldown.Value - cooldown.Key);
					healthPotionSlot->ResetCooldown();
					healthPotionSlot->TriggerCooldown(cooldown.Key, cooldownReduction);
					BroadcastEnchantmentTriggeredEvent();
				}
			}
		}
	}
}

UPotionThirstRanged::UPotionThirstRanged() {
	TypeId = EEnchantmentTypeID::PotionThirstRanged;

	LevelMultiplier = [this](int level) -> float {
		return cooldownReductionBase + cooldownReductionPerLevel * float(level) - cooldownReductionPerLevel;
	};
	MultiplierFormatter = valueformat::asDurationSingleDecimalSecond;
}

void UPotionThirstRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		if (ABaseCharacter* attackTarget = Cast<ABaseCharacter>(toWhom)) {
			if (attackTarget->IsNotAlive()) {
				ApplyEffect();
			}
		}
	}
}

void UPotionThirstRanged::ApplyEffect() {
	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetCharacterOwner())) {
		if (UEquipmentComponent* equipmentComponent = playerCharacter->GetEquipmentComponent()) {
			if (UItemSlot* healthPotionSlot = equipmentComponent->GetFirstSlotOfType(ESlotType::HealthPotion)) {
				TPair<float, float> cooldown = healthPotionSlot->GetCooldownSecondsRemainingAndDuration();
				if (cooldown.Key > 0.0f) {
					cooldown.Key = std::max(cooldown.Key - LevelMultiplier(Level), 0.0f);
					float cooldownReduction = healthPotionSlot->GetCooldownSecondsReduction() + (cooldown.Value - cooldown.Key);
					healthPotionSlot->ResetCooldown();
					healthPotionSlot->TriggerCooldown(cooldown.Key, cooldownReduction);
					BroadcastEnchantmentTriggeredEvent();
				}
			}
		}
	}
}