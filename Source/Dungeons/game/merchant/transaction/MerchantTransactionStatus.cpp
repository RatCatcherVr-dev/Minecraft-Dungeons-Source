#include "Dungeons.h"
#include "MerchantTransactionStatus.h"

#include "locale/LocTableFromFile.h"

#define LOCTEXT_NAMESPACE "MerchantTransactionStatus"
FText UMerchantTransactionUtil::GetTransactionReasonText(EMerchantTransactionStatusReason statusReason) {
	switch (statusReason) {
	case EMerchantTransactionStatusReason::CANT_AFFORD:
		return LOCTEXT("CANT_AFFORD", "Can't Afford");
	case EMerchantTransactionStatusReason::INVENTORY_FULL:
		return LOCTEXT("INVENTORY_FULL", "Inventory Full");	
	case EMerchantTransactionStatusReason::RECIPIENT_INVENTORY_FULL:
		return LOCTEXT("RECIPIENT_INVENTORY_FULL", "Inventory Full");	
	case EMerchantTransactionStatusReason::NO_SLOT_SELECTED:
		return LOCTEXT("NO_SLOT_SELECTED", "No slot selected");
	case EMerchantTransactionStatusReason::SLOT_LOCKED:
		return LOCTEXT("SLOT_LOCKED", "Slot locked");
	case EMerchantTransactionStatusReason::SLOT_OCCUPIED:
		return LOCTEXT("SLOT_OCCUPIED", "Slot full");
	case EMerchantTransactionStatusReason::ITEM_LOCKED:
		return LOCTEXT("ITEM_LOCKED", "Item Locked");
	case EMerchantTransactionStatusReason::NO_ITEM_SELECTED:
		return LOCTEXT("NO_ITEM_SELECTED", "No item selected");
	case EMerchantTransactionStatusReason::NO_INVENTORY_SLOT_SELECTED:
		return LOCTEXT("NO_INVENTORY_SLOT_SELECTED", "No slot selected");
	case EMerchantTransactionStatusReason::NO_INVENTORY_ITEM_SELECTED:
		return LOCTEXT("NO_INVENTORY_ITEM_SELECTED", "No item selected");
	case EMerchantTransactionStatusReason::NO_REGIFTING:
		return LOCTEXT("NO_REGIFTING", "No regifting");
	case EMerchantTransactionStatusReason::CANT_GIFT_CLONES:
		return LOCTEXT("CANT_GIFT_CLONES", "Can't gift clones");
	case EMerchantTransactionStatusReason::NO_GIFTING_TO_SELF:
		return LOCTEXT("NO_GIFTING_TO_SELF", "Can't gift to yourself");
	case EMerchantTransactionStatusReason::NO_PLAYER_SELECTED:
		return LOCTEXT("NO_PLAYER_SELECTED", "No player selected");		
	case EMerchantTransactionStatusReason::ALREADY_RESERVED:
		return LOCTEXT("ALREADY_RESERVED", "Already Reserved");	
	case EMerchantTransactionStatusReason::NOTHING_TO_RESTOCK:
		return LOCTEXT("NOTHING_TO_RESTOCK", "Nothing to restock");
	case EMerchantTransactionStatusReason::NOTHING_TO_WITHDRAW:
		return LOCTEXT("NOTHING_TO_WITHDRAW", "Nothing to withdraw");		
	case EMerchantTransactionStatusReason::NOT_RESERVED:
		return LOCTEXT("NOT_RESERVED", "Not Reserved");	
	case EMerchantTransactionStatusReason::GIFT_ITEM:
		return LOCTEXT("GIFT_ITEM", "Gift Item");
	case EMerchantTransactionStatusReason::BUY_ITEM:
		return LOCTEXT("BUY_ITEM", "Buy Item");
	case EMerchantTransactionStatusReason::COLLECT_ITEM:
		return LOCTEXT("COLLECT_ITEM", "Collect");
	case EMerchantTransactionStatusReason::UPGRADE_COLLECT_ITEM:
		return LOCTEXT("UPGRADE_COLLECT_ITEM", "Collect");
	case EMerchantTransactionStatusReason::UPGRADE_ITEM:
		return LOCTEXT("UPGRADE_ITEM", "Upgrade");
	case EMerchantTransactionStatusReason::RESERVE_ITEM:
		return LOCTEXT("RESERVE_ITEM", "Reserve");
	case EMerchantTransactionStatusReason::UNRESERVE_ITEM:
		return LOCTEXT("UNRESERVE_ITEM", "Unreserve");
	case EMerchantTransactionStatusReason::RESTOCK:
		return LOCTEXT("RESTOCK_SLOTS", "Restock");
	case EMerchantTransactionStatusReason::MODIFY_ITEM:
		return LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_modify_item");
	case EMerchantTransactionStatusReason::NO_ENCHANTMENT_INDEX_SELECTED:
		return LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_no_enchantment_index_selected");
	case EMerchantTransactionStatusReason::NO_NEW_ENCHANTMENTS_AVAILABLE:
		return LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_no_new_enchantments_available");
	case EMerchantTransactionStatusReason::OFFER_AT_LEAST_ONE_ITEM:
		return LOCTEXT("RESTOCK_OFFER_AT_LEAST_ONE_ITEM", "Offer at least one item");
	case EMerchantTransactionStatusReason::MISSION_VOTE_IN_PROGRESS:
		return LOCTEXT("RESTOCK_MISSION_VOTE_IN_PROGRESS", "Mission vote in progress");		
	case EMerchantTransactionStatusReason::MISSION_DISABLED:
		return LOCTEXT("RESTOCK_MISSION_DISABLED", "Mission not currently available");
	case EMerchantTransactionStatusReason::NOT_A_HYPERMISSION:
		return LOCTEXT("RESTOCK_NOT_A_HYPERMISSION", "Invalid mission type");
	case EMerchantTransactionStatusReason::CANNOT_OFFER_GILDED_ITEMS:
		return LOCTEXT("CANNOT_OFFER_GILDED_ITEMS", "Can't offer gilded items");
	case EMerchantTransactionStatusReason::CANNOT_OFFER_CLONED_ITEMS:
		return LOCTEXT("CANNOT_OFFER_CLONED_ITEMS", "Can't offer cloned items");
	case EMerchantTransactionStatusReason::OFFER_ITEM:
		return LOCTEXT("RESTOCK_OFFER_ITEM", "Offer item");
	case EMerchantTransactionStatusReason::WITHDRAW_ITEM:
		return LOCTEXT("RESTOCK_WITHDRAW_ITEM", "Withdraw item");
	case EMerchantTransactionStatusReason::OFFERING_PROBLEM:
		return LOCTEXT("OFFERINGS_PROBLEM", "Offering Problem");
	case EMerchantTransactionStatusReason::CONFIRM_OFFERINGS:
		return LOCTEXT("RESTOCK_CONFIRM_OFFERINGS", "Confirm Offerings");
	default:		
	case EMerchantTransactionStatusReason::UNSET:
		return FText::GetEmpty();		

	}
}

#undef LOCTEXT_NAMESPACE

bool FMerchantTransactionStatus::CanExecute() const {
	return mValidatedTransaction.IsSet();
}

void FMerchantTransactionStatus::Execute() {
	mValidatedTransaction.GetValue()();
}

TOptional<FMerchantPricing> FMerchantTransactionStatus::GetOptionalPrice() const {
	return mPrice;
}

TOptional<EMerchantTransactionStatusReason> FMerchantTransactionStatus::GetOptionalStatus() const {
	if(mExecuteStatus){
		return mExecuteStatus->mStatusReason;
	}
	return {};
}

TOptional<FProblemStatus> FMerchantTransactionStatus::GetOptionalProblemStatus() const {
	if (mExecuteStatus) {
		return mExecuteStatus->mProblem;
	}
	return {};
}

TArray<FItemBulletPoint> FMerchantTransactionStatus::GetBulletPoints() const {
	return mBulletPoints;
}

FMerchantExecuteStatus::FMerchantExecuteStatus(EMerchantTransactionStatusReason StatusReason, FProblemStatus Problem) : mStatusReason(StatusReason), mProblem(Problem) {}
FMerchantExecuteStatus::FMerchantExecuteStatus(EMerchantTransactionStatusReason StatusReason) : mStatusReason(StatusReason) {}
FMerchantExecuteStatus::FMerchantExecuteStatus() {}
