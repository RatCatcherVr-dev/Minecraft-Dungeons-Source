#pragma once
#include "InventoryItemSlotTransactionBase.h"
#include "SelectedInventoryItemTransactionBase.generated.h"

UCLASS()
class DUNGEONS_API USelectedInventoryItemTransactionBase : public UInventoryItemSlotTransactionBase {
	GENERATED_BODY()

protected:
	TSubclassOf<USelectInventorySlotItem> GetInventorySlotSelectionClass() const override;	
};