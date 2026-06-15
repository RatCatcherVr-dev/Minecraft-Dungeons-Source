#include "HeavyHarpoonQuiver.h"
#include "Dungeons.h"
#include "game/item/power/ItemPowerEffectDefs.h"


AHeavyHarpoonQuiver::AHeavyHarpoonQuiver() {
	craftedArrowItemType = game::item::type::HeavyHarpoon.getId();
	craftedArrowCount = 5;
	PowerEffects = { UItemRangedDamageIncrease::StaticClass() };
}

float AHeavyHarpoonQuiver::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage: {
		float duplicatedDamageValueFromBP_PlayerHeavyHarpoonQuiver = 200.0f;
		return duplicatedDamageValueFromBP_PlayerHeavyHarpoonQuiver;
	}
	default:
		return -1;
	}
}