#include "Dungeons.h"
#include "FlamingQuiver.h"
#include "game/item/power/ItemPowerEffectDefs.h"


AFlamingQuiver::AFlamingQuiver() {
	craftedArrowItemType = game::item::type::BurningArrow.getId();
	craftedArrowCount = 7;
	PowerEffects = { UItemRangedDamagePerSecondIncrease::StaticClass() };
	Effect = UArrowCraftingRangedWeaponPowerGameplayEffect::StaticClass();
}

float AFlamingQuiver::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::DamagePerSecond:	
		//Stolen from
		//BP_BurningArrowGameplayEffect.uasset
		//We should move the gameplay effect to c++.
		return 6.f / 0.2f;
	default:
		return -1;
	}	
}

