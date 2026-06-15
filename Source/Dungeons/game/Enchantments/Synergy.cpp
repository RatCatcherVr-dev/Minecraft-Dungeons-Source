// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Synergy.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemSlot.h"
#include "game/item/instance/AItemInstance.h"
#include <AbilitySystemComponent.h>



void USynergy::OnStart() {
	auto characterOwner = GetCharacterOwner();
	if (characterOwner->HasAuthority()) {
		if (const auto equipmentComponent = characterOwner->FindComponentByClass<UEquipmentComponent>()) {
			for (auto slot : equipmentComponent->GetSlotsOfType(ESlotType::ActivePermanent)) {
				ItemSlots.Emplace(slot);
			}
		}
	}

	RefreshItems();
}

void USynergy::OnEnd() {
	if (GetOwner()->HasAuthority()) {
		for (auto slot : ItemSlots) {
			if (slot.IsValid()) {
				slot->OnItemInstanceReplicatedInternal.RemoveAll(this);

				if (auto item = slot->GetItem()) {
					item->OnItemSuccess.RemoveAll(this);
				}
			}
		}
	}
}

void USynergy::OnItemInstanceChanged(UItemSlot* slot) {
	RefreshItems();
}

void USynergy::RefreshItems() {
	for (auto slot : ItemSlots) {
		if(!slot->OnItemInstanceReplicatedInternal.IsBoundToObject(this)){
			slot->OnItemInstanceReplicatedInternal.AddUObject(this, &USynergy::OnItemInstanceChanged);
		}
		if (auto item = slot->GetItem()) {
			if (!item->OnItemSuccess.IsBoundToObject(this)){
				item->OnItemSuccess.AddUObject(this, &USynergy::OnItemSuccess);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void UHealthPotionSynergy::OnStart() {
	Super::OnStart();
	auto characterOwner = GetCharacterOwner();
	if (characterOwner->HasAuthority()) {
		if (const auto equipmentComponent = characterOwner->FindComponentByClass<UEquipmentComponent>()) {
			if(auto* slot = equipmentComponent->GetFirstSlotOfType(ESlotType::HealthPotion)) {
				slot->OnItemSlotActivationCompleted.AddUniqueDynamic(this, &UHealthPotionSynergy::OnItemSuccess);
			}
		}
	}
}

void UHealthPotionSynergy::OnEnd() {
	Super::OnEnd();
	auto characterOwner = GetCharacterOwner();
	if (GetOwner()->HasAuthority()) {
		if (const auto equipmentComponent = characterOwner->FindComponentByClass<UEquipmentComponent>()) {
			if(auto* slot = equipmentComponent->GetFirstSlotOfType(ESlotType::HealthPotion)) {
				slot->OnItemSlotActivationCompleted.RemoveDynamic(this, &UHealthPotionSynergy::OnItemSuccess);
			}
		}
	}
}

void UHealthPotionSynergy::OnItemSuccess(UItemSlot*, bool) {
	if (GetCharacterOwner()->HasAuthority()) {
		OnHealthPotionUsed();
	}
}



