#include "Dungeons.h"
#include "game/item/InventoryItemData.h"
#include "game/item/ItemUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "PickupItemComponent.h"
#include "EquipmentComponent.h"
#include "WalletComponent.h"
#include "util/Scoped.h"
#include "game/item/instance/AItemInstance.h"

bool UPickupItemComponent::Pickup(const FInventoryItemData& itemData) const {
	const auto& itemType = itemData.GetItemType();
	const int stackCount = itemData.GetStoreCount();

	if (itemType.hasTag(ItemTag::Currency)) {
		return HandleStoreCurrency(itemType, stackCount);
	}	
	if (itemType.isInstant()) {
		return HandleInstantItem(itemData);
	}
	if (itemType.isConsumable()) {
		bool stored = HandleStoreConsumable(itemType, stackCount);
		if (itemType.getId() == game::item::type::HealthPotion.getId() && !stored) {
			return HandleInstantItem(itemData);
		}

		return stored;
	}

	return HandleStoreInInventory(itemData);
}

bool UPickupItemComponent::CanPickup(const FInventoryItemData& itemData) const {
	const auto& itemType = itemData.GetItemType();

	if (itemType.hasTag(ItemTag::Currency) || itemType.isInstant()) {
		return true;
	}
	if (itemType.isConsumable()) {
		for (auto slot : GetPlayer()->GetEquipmentComponent()->GetSlotsOfType(itemType.slotType())) {
			if (slot->CanEquip(itemType)) {
				return true;
			}
		}
	}
	return !GetPlayer()->GetItemStashComponent()->IsInventoryFull();
}

bool UPickupItemComponent::HandleStoreConsumable(const ItemType& itemType, int stackCount) const {	
	const auto& slots = GetPlayer()->GetEquipmentComponent()->GetSlotsOfType(itemType.slotType());
	for (auto slot : slots) {
		if (slot->CanEquip(itemType)) {
			slot->EquipItem(FInventoryItemData(itemType.getId(), 1.f), stackCount, EEquipmentSource::PickUp);
			return true;
		}
	}
	return false;
}

bool UPickupItemComponent::HandleStoreCurrency(const ItemType& itemType, int stackCount) const {
	if (auto wallet = GetPlayer()->GetWalletComponent()) {		
		wallet->ClientAdd(itemType.getId(), stackCount, ECurrencyObtainReason::Pickup);
	}
	return true;
}

bool UPickupItemComponent::HandleStoreInInventory(const FInventoryItemData& itemData) const {
	auto inventory = GetPlayer()->GetItemStashComponent();

	// #D11.CM - Can't store items in a full inventory.
	if (inventory->IsInventoryFull()) {
		inventory->OnInventoryFull.Broadcast();
		GetPlayer()->SetInventoryFullWarning();
		return false;
	}
	
	inventory->ClientPickupItem(itemData);
	return true;
}


bool UPickupItemComponent::HandleInstantItem(const FInventoryItemData& itemData) const {
	const auto& itemType = itemData.GetItemType();
	if (itemType.isLocalActivateOnly()) {
		if (GetPlayer()->IsLocallyControlled()) {
			return HandleActivateInstantItem(itemData, GetPlayer());
		} else {
			return false;
		}
	} else {
		if (!GetPlayer()->HasAuthority()) {
			HandleActivateInstantItemServer(itemType.getId());
			return true;
		}
		return HandleActivateInstantItem(itemData, GetPlayer());
	}
}

bool UPickupItemComponent::HandleActivateInstantItemInstance(AItemInstance* instance) const {
	if (!instance->CanActivate()) {
		return false;
	}
	instance->TryActivate();

	if (instance->IsAutomaticDestructible()) {
		auto scoped = scoped::destroy(instance);
	}
	return true;

}

bool UPickupItemComponent::HandleActivateInstantItem(const FInventoryItemData& itemData, APlayerCharacter* player) const {
	if (itemData.GetItemType().hasTag(ItemTag::UIFriendly)) {
		player->GetItemStashComponent()->OnItemPickupInternal.Broadcast(itemData);
	}
	return HandleActivateInstantItemInstance(game::item::util::spawnItemInstance(itemData, 1., FVector::ZeroVector, player));
}

bool UPickupItemComponent::HandleActivateInstantItem(const ItemType& itemType, APlayerCharacter* player) const {
	return HandleActivateInstantItemInstance(game::item::util::spawnItemInstance(itemType, 1., FVector::ZeroVector, player));	
}

void UPickupItemComponent::HandleActivateInstantItemServer_Implementation(FSerializableItemId itemType) const {
	HandleActivateInstantItem(GetItemRegistry().Get(itemType), GetPlayer());
}

bool UPickupItemComponent::HandleActivateInstantItemServer_Validate(FSerializableItemId itemType) {
	return true;
}

class APlayerCharacter* UPickupItemComponent::GetPlayer() const {
	return Cast<APlayerCharacter>(GetOwner());
}
