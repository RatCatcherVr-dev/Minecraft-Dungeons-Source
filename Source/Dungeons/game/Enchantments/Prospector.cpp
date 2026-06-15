#include "Dungeons.h"
#include "Prospector.h"
#include "game/Enchantments/EnchantmentUtil.h"

UProspector::UProspector() {
	TypeId = EEnchantmentTypeID::Prospector;
	DropGameplayCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Prospector");
	DropCategory = EDropCategory::Emerald;
}

