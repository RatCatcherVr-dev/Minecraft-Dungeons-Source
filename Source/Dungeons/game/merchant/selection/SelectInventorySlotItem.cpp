#include "Dungeons.h"
#include "SelectInventorySlotItem.h"
#include "game/component/ItemStashComponent.h"

USelectInventorySlotItem::USelectInventorySlotItem() : mItemPredicate(RETLAMBDA(true))  { }
USelectInventorySlotItem::USelectInventorySlotItem(ItemPredicate pred, ESlotType uiSlotTypeIcon) : mItemPredicate(pred) { 
	mUISlotTypeIcon = uiSlotTypeIcon;
}

bool USelectInventorySlotItem::IsInventorySlotSelectable(UInventoryItemSlot* slot) const{
	return !slot->IsA<UInventoryEquipmentItemSlot>() 
		&& slot->GetItem() != nullptr 
		&& mItemPredicate(slot->GetItem());
}