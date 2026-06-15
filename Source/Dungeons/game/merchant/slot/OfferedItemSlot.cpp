#include "Dungeons.h"
#include "game/merchant/transaction/OfferItem.h"
#include "game/merchant/transaction/WithdrawItem.h"
#include "OfferedItemSlot.h"

TArray<TSubclassOf<UMerchantTransactionBase>> UOfferedItemSlot::GetAvailableTransactions() const {
	if (IsUnlocked()) {
		TArray<TSubclassOf<UMerchantTransactionBase>> transactionClasses = {
			UOfferItem::StaticClass() 
		};
		if (GetInventoryItem()) {
			transactionClasses.Add(UWithdrawItem::StaticClass());
		}
		return transactionClasses;
	}
	return Super::GetAvailableTransactions();
}


