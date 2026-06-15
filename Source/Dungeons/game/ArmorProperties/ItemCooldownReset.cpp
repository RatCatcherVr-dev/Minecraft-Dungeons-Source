#include "Dungeons.h"
#include "ItemCooldownReset.h"
#include "game/component/EquipmentComponent.h"

UItemCooldownReset::UItemCooldownReset(){
	TypeID = EArmorPropertyID::ItemCooldownReset;
}

void UItemCooldownReset::BeginPlay() {
	Super::BeginPlay();
	if (GetOwner()->HasAuthority()) {
		if (const auto equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
			for (auto slot : equipmentComponent->GetSlotsOfType(ESlotType::ActivePermanent)) {
				SlotsToReset.Emplace(slot);
			}
			for (auto slot : equipmentComponent->GetSlotsOfType(ESlotType::HealthPotion)) {
				if (!slot->OnItemSlotActivationCompleted.Contains(this, "OnHealthPotionActivated")) {
					slot->OnItemSlotActivationCompleted.AddDynamic(this, &UItemCooldownReset::OnHealthPotionActivated);
				}
			}
		}
	}
}

void UItemCooldownReset::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (const auto equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		for (auto slot : equipmentComponent->GetSlotsOfType(ESlotType::HealthPotion)) {
			if (!slot->OnItemSlotActivationCompleted.Contains(this, "OnHealthPotionActivated")) {
				slot->OnItemSlotActivationCompleted.RemoveDynamic(this, &UItemCooldownReset::OnHealthPotionActivated);
			}
		}
	}
}

void UItemCooldownReset::OnHealthPotionActivated(UItemSlot* slot, bool success) {
	if (GetOwner()->HasAuthority() && success) {
		for (auto resetSlot : SlotsToReset) {
			if (resetSlot.IsValid()) {
				resetSlot->ResetCooldown();
			}
		}
	}
}
