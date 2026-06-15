#include "Dungeons.h"
#include "SelectInventorySlotItemArmor.h"

USelectInventorySlotItemArmor::USelectInventorySlotItemArmor() : USelectInventorySlotItem(RETLAMBDA( it->Item.GetItemType().getSlotType() == ESlotType::Armor ), ESlotType::Armor) {}
