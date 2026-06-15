#pragma once
#include "ItemSalvageInfo.h"
#include "ItemSalvageUndoInfo.generated.h"

class UInventoryItem;
class UInventoryItemSlot;

USTRUCT(BlueprintType)
struct FItemSalvageUndoInfo {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UInventoryItemSlot* slot;

	UPROPERTY(BlueprintReadOnly)
	UInventoryItem* item;

	UPROPERTY(BlueprintReadOnly)
	FItemSalvageInfo salvageInfo;
};
