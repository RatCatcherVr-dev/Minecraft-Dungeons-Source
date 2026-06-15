#pragma once
#include "SelectInventorySlotItem.h"
#include "SelectInventorySlotItemArtifact.generated.h"

class UInventoryItemSlot;

UCLASS(BlueprintType)
class DUNGEONS_API USelectInventorySlotItemArtifact : public USelectInventorySlotItem {
	GENERATED_BODY()
	USelectInventorySlotItemArtifact();
};
