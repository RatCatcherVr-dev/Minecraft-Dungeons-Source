#include "Dungeons.h"
#include "GiftWrappingItemSlot.h"
#include "game/merchant/transaction/GiftWrapItem.h"

TArray<TSubclassOf<UMerchantTransactionBase>> UGiftWrappingItemSlot::GetAvailableTransactions() const
{
	if (IsUnlocked()) {
		return { UGiftWrapItem::StaticClass() };
	}
	return Super::GetAvailableTransactions();
}
