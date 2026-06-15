#pragma once
#include "SelectInventorySlotItem.h"
#include "SelectInventorySlotItemMelee.generated.h"

class UInventoryItemSlot;

UCLASS(BlueprintType)
class DUNGEONS_API USelectInventorySlotItemMelee : public USelectInventorySlotItem {
	GENERATED_BODY()
	USelectInventorySlotItemMelee();
};
