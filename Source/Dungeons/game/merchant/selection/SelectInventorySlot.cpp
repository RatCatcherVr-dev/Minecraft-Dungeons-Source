#include "Dungeons.h"
#include "SelectInventorySlot.h"
#include "game/component/ItemStashComponent.h"
#include "util/Algo.hpp"


void USelectInventorySlot::EnsureSelection() {
	if (GetInventorySlot() == nullptr){
		auto selectable = GetSelectableInventorySlots();
		if (selectable.IsValidIndex(0)) {
			SelectInventorySlot(selectable[0]);
		}
	}
}

void USelectInventorySlot::ClearSelection() {
	if (GetInventorySlot()) {
		SelectInventorySlot(nullptr);
	}
}

void USelectInventorySlot::RefreshSelectionCache() {
	SelectionCache.Reset();
	TArray<UInventoryItemSlot*> slots;
	if (auto stash = GetSession().GetItemStashComponent()) {
		stash->GetEquipmentSlots().GenerateValueArray(slots);
		slots.Append(stash->GetInventorySlots());
	}
	SelectionCache = algo::copy_if(slots, RETLAMBDA(IsInventorySlotSelectable(it);));
	if (mSelectedInventorySlot && !SelectionCache.Contains(mSelectedInventorySlot)) {
		ClearSelection();
	}
}

void USelectInventorySlot::OnSetupSession() {
	Super::OnSetupSession();
	if (auto itemStash = GetSession().GetItemStashComponentMutable()) {
		itemStash->OnInventoryChanged.AddUObject(this, &USelectInventorySlot::RefreshSelectionCache);
	}
	RefreshSelectionCache();
}

void USelectInventorySlot::OnCleanupSession() {
	Super::OnCleanupSession();
	if (auto itemStash = GetSession().GetItemStashComponentMutable()) {
		itemStash->OnInventoryChanged.RemoveAll(this);
	}
	SelectionCache.Reset();
}

bool USelectInventorySlot::IsInventorySlotSelectable(UInventoryItemSlot* slot) const {
	return true;
}

UInventoryItemSlot* USelectInventorySlot::GetInventorySlot() const {
	return mSelectedInventorySlot;
}

const TArray<UInventoryItemSlot*>& USelectInventorySlot::GetSelectableInventorySlots() const {	
	return SelectionCache;
}

void USelectInventorySlot::SelectInventorySlot(UInventoryItemSlot* slot) {
	if(slot != mSelectedInventorySlot){
		mSelectedInventorySlot = slot;
		SelectionChanged();
	} else if (slot) {
		TryConfirmSelection();
	}
}

bool USelectInventorySlot::PollHasSelectablesChanged() {
	auto currentChangeIndex = GetSession().GetItemStashComponent()->GetChangeIndex();
	if (mLastChangeIndex != currentChangeIndex) {
		mLastChangeIndex = currentChangeIndex;
		SelectionSelectableChanged();
		return true;
	}
	return false;
}

ESlotType USelectInventorySlot::GetUISlotTypeIcon() const {
	return mUISlotTypeIcon;
}

bool USelectInventorySlot::CanSelectAny() const {
	return GetSelectableInventorySlots().Num() > 0;
}

bool USelectInventorySlot::HasSelectedAny() const {
	return GetInventorySlot() != nullptr;
}
