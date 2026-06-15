#pragma once
#include "MerchantCurrencyComponent.h"
#include "GoldCurrency.generated.h"

UCLASS()
class DUNGEONS_API UGoldCurrency : public UMerchantCurrencyComponent {
	GENERATED_BODY()
public:
	FSerializableItemId GetCurrencyItemId() const override;
};