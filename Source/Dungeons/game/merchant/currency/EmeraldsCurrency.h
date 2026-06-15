#pragma once
#include "MerchantCurrencyComponent.h"
#include "EmeraldsCurrency.generated.h"

UCLASS()
class DUNGEONS_API UEmeraldsCurrency : public UMerchantCurrencyComponent {
	GENERATED_BODY()
public:		
	FSerializableItemId GetCurrencyItemId() const override;
};