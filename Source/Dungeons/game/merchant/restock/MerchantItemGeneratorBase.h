#pragma once
#include "game/merchant/MerchantContext.h"
#include "game/item/InventoryItemData.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "MerchantItemGeneratorBase.generated.h"


USTRUCT()
struct DUNGEONS_API FMerchantItemData {
	GENERATED_BODY()
	FInventoryItemData mItem;
	float mPriceMultiplier = 1.0f;
	float mRebateFraction = 0.0f;
};

UCLASS()
class DUNGEONS_API UMerchantItemGeneratorBase : public UMerchantSubobjectBase {
	GENERATED_BODY()
public:	
	virtual TOptional<FMerchantItemData> generate() const { return {}; };
};