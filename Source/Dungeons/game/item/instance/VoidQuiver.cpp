#include "Dungeons.h"
#include "VoidQuiver.h"
#include "game/item/power/ItemPowerEffectDefs.h"


AVoidQuiver::AVoidQuiver() {
	craftedArrowItemType = game::item::type::VoidArrow.getId();
	craftedArrowCount = 5;
	PowerEffects = { UItemRangedDamageIncrease::StaticClass() };
}

float AVoidQuiver::GetStats(EItemStats stat) const
{
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return 50.f;
	}
	return -1;
}

