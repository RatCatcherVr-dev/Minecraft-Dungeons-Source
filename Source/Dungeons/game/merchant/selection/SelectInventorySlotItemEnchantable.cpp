#include "Dungeons.h"
#include "SelectInventorySlotItemEnchantable.h"

USelectInventorySlotItemEnchantable::USelectInventorySlotItemEnchantable() :
	USelectInventorySlotItem(
		RETLAMBDA(
			it->GetTotalEnchantmentRows() > 0
		)
	)
{
}
