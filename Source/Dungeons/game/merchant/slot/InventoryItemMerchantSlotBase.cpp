#include "Dungeons.h"
#include "game/component/ItemStashComponent.h"
#include "InventoryItemMerchantSlotBase.h"

void UInventoryItemMerchantSlotBase::WasChanged() const {
	Super::WasChanged();
	OnInventoryItemMerchantSlotChanged.Broadcast();
}

void UInventoryItemMerchantSlotBase::OnSetupSession() {
	Super::OnSetupSession();
	if (auto itemStash = GetSession().GetItemStashComponentMutable()) {
		itemStash->OnInventoryChanged.AddUObject(this, &UInventoryItemMerchantSlotBase::ValidateSelectedInventoryItem);
	}
}

void UInventoryItemMerchantSlotBase::OnCleanupSession() {
	Super::OnCleanupSession();
	if (auto itemStash = GetSession().GetItemStashComponentMutable()) {
		itemStash->OnInventoryChanged.RemoveAll(this);
	}
}

void UInventoryItemMerchantSlotBase::ValidateSelectedInventoryItem() {
	if (mInventoryItem) {
		auto itemStash = GetSession().GetItemStashComponentMutable();
		if (!itemStash || !itemStash->HasItem(mInventoryItem)) {
			ClearInventoryItem();
		}
	}
}

void UInventoryItemMerchantSlotBase::SetInventoryItem(UInventoryItem* offeringItem) {
	mInventoryItem = offeringItem;
	WasChanged();
}

void UInventoryItemMerchantSlotBase::ClearInventoryItem() {
	mInventoryItem = nullptr;
	WasChanged();
}

bool UInventoryItemMerchantSlotBase::HasInventoryItem() const {
	return mInventoryItem != nullptr;
}

UInventoryItem* UInventoryItemMerchantSlotBase::GetInventoryItem() const {
	return mInventoryItem;
}



