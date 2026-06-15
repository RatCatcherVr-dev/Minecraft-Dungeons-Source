#include "Dungeons.h"
#include "game/component/WalletComponent.h"
#include "MerchantCurrencyComponent.h"

int UMerchantCurrencyComponent::GetBalance() const {
	if (auto wallet = GetSession().GetWalletComponent()) {
		return wallet->Balance(GetCurrencyItemId());
	}
	return 0;
}

void UMerchantCurrencyComponent::Deduct(int amount) const {
	if (auto wallet = GetSession().GetWalletComponentMutable()) {
		wallet->Deduct(GetCurrencyItemId(), amount);
	}
}

FSerializableItemId UMerchantCurrencyComponent::GetCurrencyItemId() const{
	return {};	
}

int UMerchantCurrencyComponent::GetCurrencyBalance() const {
	return GetBalance();
}

FMerchantDisplayPrice UMerchantCurrencyComponent::AsMerchantDisplayPrice(TOptional<FMerchantPricing> optionalPrice) const {
	return { optionalPrice.Get({0,0.0f}), GetCurrencyItemId() };
}

