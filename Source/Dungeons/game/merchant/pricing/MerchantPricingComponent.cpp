#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "MerchantPricingComponent.h"

const FMerchantPricingSaveData& UMerchantPricingComponent::ReadSaveData() const {
	return GetContext().ReadSaveData().ReadPricing();
}

FMerchantPricingSaveData& UMerchantPricingComponent::EditSaveData() const {
	return GetContext().EditSaveData().EditPricing();
}

void UMerchantPricingComponent::EnsureSaveData() const {}

