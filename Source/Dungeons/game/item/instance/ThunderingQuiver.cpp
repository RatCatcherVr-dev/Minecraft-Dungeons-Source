#include "Dungeons.h"
#include "ThunderingQuiver.h"
#include "game/item/power/ItemPowerEffectDefs.h"


AThunderingQuiver::AThunderingQuiver() {
	craftedArrowItemType = game::item::type::ThunderingArrow.getId();
	craftedArrowCount = 5;
	PowerEffects = { UItemRangedDamageIncrease::StaticClass() };
	Effect = UArrowCraftingRangedWeaponPowerGameplayEffect::StaticClass();
}

float AThunderingQuiver::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		float duplicatedDamageValueFromBP_ThunderingQuiverArrow = 37.5f;
		return duplicatedDamageValueFromBP_ThunderingQuiverArrow;
	}
	return -1;
}

