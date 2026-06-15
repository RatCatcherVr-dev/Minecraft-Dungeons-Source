#pragma once
#include "SelectInventorySlot.h"
#include "game/inventory/InventoryItem.h"
#include "SelectInventorySlotItem.generated.h"

class UInventoryItemSlot;

UCLASS(BlueprintType)
class DUNGEONS_API USelectInventorySlotItem : public USelectInventorySlot {
	GENERATED_BODY()
public:

	USelectInventorySlotItem();
	using ItemPredicate = Pred<const UInventoryItem*>;	
	USelectInventorySlotItem(ItemPredicate, ESlotType = ESlotType::Any);

	template <class T>
	static T* CreateSubobject(UObject* object, const FName& name, ItemPredicate predicate) {
		auto selection = object->CreateDefaultSubobject<T>(name);
		auto defaultPredicate = selection->mItemPredicate;
		selection->mItemPredicate = [predicate, defaultPredicate](const UInventoryItem* item) {
			return predicate(item) && defaultPredicate(item);
		};
		return selection;
	}
	
protected:
	ItemPredicate mItemPredicate;

	bool IsInventorySlotSelectable(UInventoryItemSlot*) const override;
};
