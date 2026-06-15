#include "Dungeons.h"
#include "BuyableItemSlot.h"
#include "game/merchant/transaction/BuyItem.h"
#include "game/merchant/transaction/ReserveItem.h"
#include "game/merchant/transaction/UnreserveItem.h"


TArray<TSubclassOf<UMerchantTransactionBase>> UBuyableItemSlot::GetAvailableTransactions() const
{
	if (IsUnlocked() && HasItem()) {
		return {
			UBuyItem::StaticClass(),
			IsReserved() ? UUnreserveItem::StaticClass() : UReserveItem::StaticClass(),
		};
	}
	return Super::GetAvailableTransactions();
}

TOptional<FMerchantPricing> UBuyableItemSlot::GetOptionalPrice() const {
	if (auto maybeItem = GetOptionalItem()) {
		return GetSession().GetPrice(maybeItem.GetValue(), GetPriceMultiplier(), GetRebateFraction());
	}	
	return {};
}

