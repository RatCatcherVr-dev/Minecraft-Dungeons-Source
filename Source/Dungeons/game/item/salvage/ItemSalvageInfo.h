#pragma once
#include "game/item/SerializableItemId.h"
#include "ItemSalvageInfo.generated.h"

USTRUCT(BlueprintType)
struct FItemSalvageInfo {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TMap<FSerializableItemId, int> currencies;

	UPROPERTY(BlueprintReadOnly)
	int enchantmentPoints;	
};
