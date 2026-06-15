#pragma once
#include "game/merchant/MerchantContext.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "game/item/InventoryItemData.h"
#include <Components/ActorComponent.h>
#include "MerchantPrice.h"
#include "MerchantCurrencyComponent.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UMerchantCurrencyComponent : public UMerchantSubobjectBase {
	GENERATED_BODY()
public:	
	int GetBalance() const;
	void Deduct(int amount) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual FSerializableItemId GetCurrencyItemId() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual int GetCurrencyBalance() const;

	FMerchantDisplayPrice AsMerchantDisplayPrice(TOptional<FMerchantPricing> optionalPrice) const;
};