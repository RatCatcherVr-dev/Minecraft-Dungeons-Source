#include "Dungeons.h"
#include "RestockSlots.h"
#include "game/merchant/type/MerchantBase.h"
#include "util/Algo.hpp"

FMerchantTransactionStatus URestockSlots::Validate() const
{
	auto session = GetSession();
	auto maybePrice = session.GetRestockPrice();
	if (maybePrice.IsSet()) {
		if (!session.CanAfford(maybePrice.GetValue())) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANT_AFFORD), maybePrice };
		}
	}

	if (!algo::any_of(session.GetMerchant().GetSlots(), RETLAMBDA( it->CanRestock() ))) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NOTHING_TO_RESTOCK), maybePrice };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::RESTOCK), maybePrice, {},
		FMerchantTransactionStatus::ValidatedTransaction([maybePrice, session]() -> void {
			//Restock
			session.RestockSlots();
			if (maybePrice.IsSet()) {
				session.Deduct(maybePrice.GetValue());
			}
		})
	};
}

EMerchantTransactionStatusReason URestockSlots::SuccessReason() const {
	return EMerchantTransactionStatusReason::RESTOCK;
}
