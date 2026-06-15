#pragma once

#include "CoreMinimal.h"
#include "AItemInstance.h"
#include "util/FloatRange.h"
#include "game/item/SerializableItemId.h"
#include "game/item/ItemUtil.h"
#include "StorableDropBaseItemInstance.generated.h"



UCLASS()
class DUNGEONS_API AStorableDropBaseItemInstance : public AItemInstance {
	GENERATED_BODY()
protected:
	int DropItems(int dropAmount = 1, bool forceDuplicate = false, bool preventDuplicate = false);

	TArray<game::item::util::FSpecifiedStoreCountItemDrop> PossibleItemsToDrop;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "100"), Category = "Dungeons")
	int duplicateDropChance = 10;
};