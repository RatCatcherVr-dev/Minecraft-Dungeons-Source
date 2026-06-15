#include "Dungeons.h"
#include "game/inventory/InventoryItem.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/restock/MerchantItemGeneratorBase.h"
#include "MerchantItemSlotBase.h"

UMerchantItemSlotBase* UMerchantItemSlotBase::SetItemGenerator(UMerchantItemGeneratorBase* itemGenerator) {
	mItemGenerator = itemGenerator;
	return this;
}

TOptional<FInventoryItemData> UMerchantItemSlotBase::TryPopItem() {
	if (HasItem()) {
		TOptional<FInventoryItemData> itemToPopCopy = GetOptionalItem();
		ClearItem();
		return itemToPopCopy;
	}
	return {};
}

FInventoryItemData UMerchantItemSlotBase::PopItem() {
	return TryPopItem().GetValue();
}

const TOptional<FInventoryItemData>& UMerchantItemSlotBase::GetOptionalItem() const {
	return ReadSaveData().item;
}

void UMerchantItemSlotBase::SetItem(FInventoryItemData itemToSet) {
	EditSaveData().item = itemToSet;
	if (IsReserved()) {
		SetReserved(false);
	}
	OnItemSet();
	RefreshDisplayItemCache();
	WasChanged();
}

void UMerchantItemSlotBase::SetMerchantItem(FMerchantItemData generatedData) {
	SetItem(generatedData.mItem);
	SetPriceMultiplier(generatedData.mPriceMultiplier);
	SetRebateFraction(generatedData.mRebateFraction);
}

void UMerchantItemSlotBase::ClearItem() {
	EditSaveData().item.Reset();
	if(IsReserved()){
		SetReserved(false);
	}
	RefreshDisplayItemCache();
	WasChanged();
}


void UMerchantItemSlotBase::SetDeliveredDisplayItem(UInventoryItem* item) {
	mDeliveredDisplayItem = item;	
	WasChanged();	
}

const FInventoryItemData& UMerchantItemSlotBase::GetItem() const {
	return GetOptionalItem().GetValue();
}

UInventoryItem* UMerchantItemSlotBase::GetDisplayItemCache() const {
	return mDisplayItemCache ? mDisplayItemCache : mDeliveredDisplayItem;
}

bool UMerchantItemSlotBase::HasItem() const {
	return ReadSaveData().item.IsSet();
}

bool UMerchantItemSlotBase::IsReserved() const {
	return ReadSaveData().bReserved;
}

bool UMerchantItemSlotBase::IsItemUnlocked() const {
	return IsUnlocked() && HasItem();
}

void UMerchantItemSlotBase::EnsureSaveData() {
	Super::EnsureSaveData();
	RefreshDisplayItemCache();
	WasChanged();
}

bool UMerchantItemSlotBase::IsItemReserved() const {
	return HasItem() && IsReserved();
}

void UMerchantItemSlotBase::SetReserved(bool Reserved) {	
	EditSaveData().bReserved = Reserved;
	WasChanged();
}

void UMerchantItemSlotBase::RefreshDisplayItemCache() {
	if (HasItem()) {		
		mDisplayItemCache = NewObject<UInventoryItem>(this);
		mDisplayItemCache->Item = GetItem();
	}
	else {
		mDisplayItemCache = nullptr;	
	}
}

float UMerchantItemSlotBase::GetRebateFraction() const {
	return ReadSaveData().mRebateFraction;
}

void UMerchantItemSlotBase::SetRebateFraction(float fraction) const {
	if(GetRebateFraction() != fraction){
		EditSaveData().mRebateFraction = fraction;
		WasChanged();
	}
}

float UMerchantItemSlotBase::GetPriceMultiplier() const {
	return ReadSaveData().mPriceMultiplier;
}

void UMerchantItemSlotBase::SetPriceMultiplier(float multiplier) const {
	if(GetPriceMultiplier() != multiplier){
		EditSaveData().mPriceMultiplier = multiplier;
		WasChanged();
	}
}

void UMerchantItemSlotBase::WasChanged() const {
	Super::WasChanged();
	OnMerchantItemSlotChanged.Broadcast();
}

void UMerchantItemSlotBase::OnItemSet() {}

void UMerchantItemSlotBase::Restock() {
	Super::Restock();
	if (mItemGenerator) {
		SetDeliveredDisplayItem(nullptr);
		if (IsUnlocked()){
			if (IsItemReserved()) {
				//NOP! Item is reserved, leave it.
			} else {
				auto maybeGeneratedItem = mItemGenerator->generate();
				if (maybeGeneratedItem) {
					SetMerchantItem(maybeGeneratedItem.GetValue());					
				} else {
					ClearItem();
				}
			}
		} else {
			if (HasItem()) {
				//Should not have item if we are locked.
				ClearItem();
			}
		}
	}
}

bool UMerchantItemSlotBase::CanRestock() const {
	return mItemGenerator != nullptr && IsUnlocked() && !IsItemReserved();
}


