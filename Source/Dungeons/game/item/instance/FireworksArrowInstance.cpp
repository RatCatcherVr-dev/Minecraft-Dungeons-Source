#include "Dungeons.h"
#include "FireworksArrowInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"

AFireworksArrowInstance::AFireworksArrowInstance() {
	craftedArrowItemType = game::item::type::FireworksArrow.getId();
	craftedArrowCount = 1;
	PowerEffects = { UItemRangedDamageIncrease::StaticClass() };
}

float AFireworksArrowInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		//Duplicated from
		//BP_PlayerFireworksArrow.uasset
		float duplicatedDamageValueFromBP_PlayerFireworksArrow = 200.0f;
		return duplicatedDamageValueFromBP_PlayerFireworksArrow;
	}
	return -1;
}
