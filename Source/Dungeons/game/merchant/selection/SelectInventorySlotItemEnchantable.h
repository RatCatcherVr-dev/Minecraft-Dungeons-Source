#pragma once
#include "SelectInventorySlotItem.h"
#include "SelectInventorySlotItemEnchantable.generated.h"

class UInventoryItemSlot;

UCLASS(BlueprintType)
class DUNGEONS_API USelectInventorySlotItemEnchantable : public USelectInventorySlotItem {
	GENERATED_BODY()
	USelectInventorySlotItemEnchantable();
};
