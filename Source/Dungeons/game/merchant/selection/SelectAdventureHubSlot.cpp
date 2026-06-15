#include "Dungeons.h"
#include "SelectAdventureHubSlot.h"
#include "online/sessions/OnlineUtil.h"
#include "game/merchant/MerchantContext.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/component/ItemStashComponent.h"
#include "game/item/ItemType.h"
#include "game/cosmetics/CosmeticsLibrary.h"
#include "DungeonsGameInstance.h"
#include "online/seasons/LiveOps.h"
#include "game/actor/item/StorableItem.h"


namespace adventurehub {
	EAdventureHubSlotState ToSlotType(minecraft::api::RewardState state) {
		switch (state) {
		case minecraft::api::RewardState::Unavailable:
			return EAdventureHubSlotState::Unavailable;
		case minecraft::api::RewardState::Locked:
			return EAdventureHubSlotState::Locked;
		case minecraft::api::RewardState::Unlocked:
			return EAdventureHubSlotState::Unlocked;
		case minecraft::api::RewardState::Claimed:
			return EAdventureHubSlotState::Claimed;
		default:
			break;
		}
		return EAdventureHubSlotState::Unavailable;
	}

	EAdventureHubRewardType ToRewardType(minecraft::api::RewardTypes reward) {
		switch (reward) {
		case minecraft::api::RewardTypes::Emeralds:
		case minecraft::api::RewardTypes::Gold:
		case minecraft::api::RewardTypes::Item:
			return EAdventureHubRewardType::Item;
		case minecraft::api::RewardTypes::Cosmetic:
			return EAdventureHubRewardType::Cosmetic;
		default:
			break;
		}
		return EAdventureHubRewardType::Cosmetic;
	}
}

void USelectAdventureHubSlot::EnsureSelection() {
	if (GetHubSlot() == nullptr){
		auto selectable = GetHubSlots();
		if (selectable.IsValidIndex(0)) {
			SelectHubSlot(selectable[0]);
		}
	}
}

void USelectAdventureHubSlot::ClearSelection() {
	if (GetHubSlot()) {
		SelectHubSlot(nullptr);
	}
}

void USelectAdventureHubSlot::UpdateSelectionCache(const online::liveops::SeasonView& season) {
	SelectionCache.Reset();
	const auto& rewards = season.RewardLevels;
	for (int i = 0; i < rewards.size(); ++i) {
		if (rewards[i].PaidReward) {
			auto premiumSlot = NewObject<UAdventureHubSlot>(this);
			premiumSlot->MonetizationType = EMonetizationType::Paid;
			premiumSlot->SlotState = adventurehub::ToSlotType(rewards[i].PaidReward->State);
			premiumSlot->RewardType = adventurehub::ToRewardType(rewards[i].PaidReward->Type);
			premiumSlot->Id = rewards[i].PaidReward->ItemNameId;
			premiumSlot->Level = i;
			SelectionCache.Emplace(premiumSlot);
		}

		if (rewards[i].FreeReward) {
			auto freeSlot = NewObject<UAdventureHubSlot>(this);
			freeSlot->MonetizationType = EMonetizationType::Free;
			freeSlot->SlotState = adventurehub::ToSlotType(rewards[i].FreeReward->State);
			freeSlot->RewardType = adventurehub::ToRewardType(rewards[i].FreeReward->Type);
			freeSlot->Id = rewards[i].FreeReward->ItemNameId;
			freeSlot->Level = i;
			SelectionCache.Emplace(freeSlot);
		}
		else {
			auto freeSlot = NewObject<UAdventureHubSlot>(this);
			freeSlot->MonetizationType = EMonetizationType::Free;
			freeSlot->RewardType = EAdventureHubRewardType::None;
			freeSlot->Level = i;
			SelectionCache.Emplace(freeSlot);
		}
	}
	if (mSelectedHubSlot && !SelectionCache.Contains(mSelectedHubSlot)) {
		ClearSelection();
	}
}

void USelectAdventureHubSlot::OnSetupSession() {
	Super::OnSetupSession();
	auto gameInstance = GetWorld()->GetGameInstance();
	auto liveOps = online::getLiveOps(gameInstance);
	DataUpdateHandle = liveOps->OnSeasonViewUpdate.AddUObject(this, &USelectAdventureHubSlot::OnDataUpdate);
	liveOps->RequestSeasonViewUpdateAsync();
	UpdateSelectionCache(liveOps->GetSeasonView());
}

void USelectAdventureHubSlot::OnCleanupSession() {
	Super::OnCleanupSession();
	auto gameInstance = GetWorld()->GetGameInstance();
	auto liveOps = online::getLiveOps(gameInstance);
	liveOps->OnSeasonViewUpdate.Remove(DataUpdateHandle);
	SelectionCache.Reset();
}

UAdventureHubSlot* USelectAdventureHubSlot::GetHubSlot() const {
	return mSelectedHubSlot;
}

const TArray<UAdventureHubSlot*>& USelectAdventureHubSlot::GetHubSlots() const {
	return SelectionCache;
}

void USelectAdventureHubSlot::SelectHubSlot(UAdventureHubSlot* slot) {
	if(slot != mSelectedHubSlot){
		mSelectedHubSlot = slot;
		SelectionChanged();
	} else if (slot) {
		TryConfirmSelection();
	}
}

bool USelectAdventureHubSlot::PollHasSelectablesChanged() {
	// Let's see what we need to do here
	/*auto currentChangeIndex = GetSession().GetItemStashComponent()->GetChangeIndex();
	if (mLastChangeIndex != currentChangeIndex) {
		mLastChangeIndex = currentChangeIndex;
		SelectionSelectableChanged();
		return true;
	}*/
	return false;
}

bool USelectAdventureHubSlot::CanSelectAny() const {
	return GetHubSlots().Num() > 0;
}

bool USelectAdventureHubSlot::HasSelectedAny() const {
	return GetHubSlot() != nullptr;
}

void USelectAdventureHubSlot::OnDataUpdate(online::liveops::UpdateRequestStatus status, const online::liveops::SeasonView& season) {
	UpdateSelectionCache(season);
}

UTexture2D* UAdventureHubSlot::GetIcon(UObject* worldContext) const {
	auto gameInstance = Cast<UDungeonsGameInstance>(worldContext->GetWorld()->GetGameInstance());
	switch (RewardType) {
	case EAdventureHubRewardType::Cosmetic:
		return gameInstance->GetCosmeticsLibrary()->FindDefinition(Id)->GetCosmeticIconTexture();
	case EAdventureHubRewardType::Item: {
		auto itemId = GetItemRegistry().Request(Id);
		if (itemId.IsSet()) {
			return UInventoryItemSlot::GetIconTextureForItemId(itemId.GetValue());
		}
		break;
	}
	case EAdventureHubRewardType::None:
	default:
		break;
	}
	return nullptr;
}

FText UAdventureHubSlot::GetDisplayName(UObject* worldContext) const {
	auto gameInstance = Cast<UDungeonsGameInstance>(worldContext->GetWorld()->GetGameInstance());
	switch (RewardType) {
	case EAdventureHubRewardType::Cosmetic:
		return gameInstance->GetCosmeticsLibrary()->FindDefinition(Id)->GetCosmeticDisplayName();
	case EAdventureHubRewardType::Item: {
		auto itemId = GetItemRegistry().Request(Id);
		if (itemId.IsSet()) {
			return FText::FromString(GetItemRegistry().Get(itemId.GetValue()).getName());
		}
		break;
	}
	case EAdventureHubRewardType::None:
	default:
		break;
	}
	return FText::GetEmpty();
}
