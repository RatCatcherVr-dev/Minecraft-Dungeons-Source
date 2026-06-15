#include "Dungeons.h"
#include "SelectInventorySlotItemMelee.h"

USelectInventorySlotItemMelee::USelectInventorySlotItemMelee() : USelectInventorySlotItem(RETLAMBDA( it->Item.GetItemType().getSlotType() == ESlotType::MeleeWeapon ), ESlotType::MeleeWeapon) {}
