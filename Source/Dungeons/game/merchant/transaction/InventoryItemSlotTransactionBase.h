#pragma once
#include "MerchantSlotTransactionBase.h"
#include "game/merchant/slot/MerchantItemSlotBase.h"
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "InventoryItemSlotTransactionBase.generated.h"

class UInventoryItemSlot;

UCLASS(Abstract)
class DUNGEONS_API UInventoryItemSlotTransactionBase : public UMerchantSlotTransactionBase {
	GENERATED_BODY()	
protected:
	void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const override;

	virtual TSubclassOf<USelectInventorySlotItem> GetInventorySlotSelectionClass() const;
public:

	template <typename T = USelectInventorySlotItem>
	UInventoryItemSlot* GetSelectedInventoryItemSlotByClass() const {
		if (auto inventorySlotSelection = Cast<T>(GetContext().GetMerchant().GetSelectionByClass(T::StaticClass()))) {
			return inventorySlotSelection->GetInventorySlot();
		}
		return nullptr;
	}

	UInventoryItemSlot* GetSelectedInventoryItemSlot() const;
};