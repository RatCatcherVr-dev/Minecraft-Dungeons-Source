#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "ItemSalvageInfo.h"
#include "game/item/InventoryItemData.h"
#include "ItemSalvageUtil.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UItemSalvageUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

private:
	static const float SALVAGE_GOLD_POWER_FACTOR;
	static const float SALVAGE_EMERALDS_POWER_FACTOR;
	static int32 GetSalvageRarityMultiplier(const FInventoryItemData& item);
public:	
	static FItemSalvageInfo GetSalvageItemInfo(const FInventoryItemData& item, const float valueMultiplier = 1.0f);
};
