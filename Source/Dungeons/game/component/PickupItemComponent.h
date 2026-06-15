#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"
#include "game/item/InventoryItemData.h"
#include "PickupItemComponent.generated.h"

class ItemType;
class APlayerCharacter;

UCLASS()
class DUNGEONS_API UPickupItemComponent : public UActorComponent
{	
	GENERATED_BODY()
public:	
	bool Pickup(const FInventoryItemData&) const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanPickup(const FInventoryItemData& itemData) const;
private:
	APlayerCharacter* GetPlayer() const;
	bool HandleStorage(const ItemType&) const;	
	bool HandleInstantItem(const FInventoryItemData& itemData) const;
	bool HandleActivateInstantItemInstance(AItemInstance* instance) const;
	bool HandleActivateInstantItem(const FInventoryItemData& itemData, APlayerCharacter* player) const;
	bool HandleActivateInstantItem(const ItemType&, APlayerCharacter* player) const;
	bool HandleStoreInInventory(const FInventoryItemData&) const;
	bool HandleStoreCurrency(const ItemType&, int stackCount) const;
	bool HandleStoreConsumable(const ItemType&, int stackCount) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void HandleActivateInstantItemServer(FSerializableItemId itemType) const;	
};