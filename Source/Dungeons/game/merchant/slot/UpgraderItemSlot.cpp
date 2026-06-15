#include "Dungeons.h"
#include "UpgraderItemSlot.h"
#include "game/merchant/transaction/UpgradeInsertItem.h"
#include "game/merchant/transaction/UpgradeCollectItem.h"

UUpgraderItemSlot* UUpgraderItemSlot::CreateSubobject(UObject* object, const FName& name, UMerchantQuestBase* upgradeQuest) {
	auto slot = object->CreateDefaultSubobject<UUpgraderItemSlot>(name);
	slot->mUpgradeQuest = upgradeQuest;
	return slot;
}

UUpgraderItemSlot& UUpgraderItemSlot::SetUpgradeQuest(UMerchantQuestBase* quest) {
	mUpgradeQuest = quest;
	return *this;
}

UMerchantQuestBase* UUpgraderItemSlot::GetActiveUpgradeQuest() const {
	return HasItem() && !IsUpgradeCompleted() ? GetCurrentUpgradeQuest() : nullptr;
}

UMerchantQuestBase* UUpgraderItemSlot::GetCompletedUpgradeQuest() const {
	return HasItem() && IsUpgradeCompleted() ? mUpgradeQuest : nullptr;
}

bool UUpgraderItemSlot::ShouldShowObjectiveMarker() const {
	return HasItem() && IsUpgradeCompleted();
}

bool UUpgraderItemSlot::IsUpgradeCompleted() const {
	if (auto quest = GetCurrentUpgradeQuest()) {
		return quest->IsCompleted();
	}
	return true;
}

void UUpgraderItemSlot::OnItemSet() {
	Super::OnItemSet();
	if (auto quest = GetCurrentUpgradeQuest()) {
		quest->ResetProgress();
	}
}

UMerchantQuestBase* UUpgraderItemSlot::GetCurrentUpgradeQuest() const {
	return mUpgradeQuest;		
}


bool UUpgraderItemSlot::IsItemUnlocked() const {
	return Super::IsItemUnlocked() && IsUpgradeCompleted();
}

TArray<TSubclassOf<UMerchantTransactionBase>> UUpgraderItemSlot::GetAvailableTransactions() const
{
	if(IsUnlocked()){
		if (HasItem()) {
			if (IsUpgradeCompleted()) {
				return { UUpgradeCollectItem::StaticClass() };
			}
		} else {
			return { UUpgradeInsertItem::StaticClass() };
		}
	}
	return Super::GetAvailableTransactions();
}

bool UUpgraderItemSlot::CanRestock() const {
	return false;
}

void UUpgraderItemSlot::EnsureSaveData() {
	Super::EnsureSaveData();
	if(auto quest = GetCurrentUpgradeQuest()){
		quest->EnsureSaveData();
	}
}
