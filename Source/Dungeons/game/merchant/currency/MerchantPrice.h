#pragma once

#include "game/item/SerializableItemId.h"
#include "MerchantPrice.generated.h"

USTRUCT(BlueprintType)
struct FMerchantPricing {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int Price = 0;

	UPROPERTY(BlueprintReadOnly)
	float RebateApplied = 0.0f;
};
template TOptional<FMerchantPricing>::~TOptional();

USTRUCT(BlueprintType)
struct FMerchantDisplayPrice {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FMerchantPricing Pricing;
	
	UPROPERTY(BlueprintReadOnly)
	FSerializableItemId CurrencyItemId;
};
