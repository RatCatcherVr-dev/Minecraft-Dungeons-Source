#include "Dungeons.h"
#include "SelectInventorySlotItemArtifact.h"

USelectInventorySlotItemArtifact::USelectInventorySlotItemArtifact() : USelectInventorySlotItem(RETLAMBDA( it->Item.GetItemType().getSlotType() == ESlotType::ActivePermanent ), ESlotType::ActivePermanent) {}
