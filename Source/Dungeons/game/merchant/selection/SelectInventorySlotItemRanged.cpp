#include "Dungeons.h"
#include "SelectInventorySlotItemRanged.h"

USelectInventorySlotItemRanged::USelectInventorySlotItemRanged() : USelectInventorySlotItem(RETLAMBDA( it->Item.GetItemType().getSlotType() == ESlotType::RangedWeapon ), ESlotType::RangedWeapon) {}
