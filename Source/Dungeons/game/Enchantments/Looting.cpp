#include "Dungeons.h"
#include "Looting.h"
#include "game/actor/item/ItemDropChanceActor.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffectTypes.h>

ULooting::ULooting() {
	TypeId = EEnchantmentTypeID::Looting;
	TypeId = EEnchantmentTypeID::Looting;
	DropGameplayCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Looting");
	DropCategory = EDropCategory::Consumable;
}

void ULooting::AddGameplayCueParameters(FGameplayCueParameters& params) {
	params.NormalizedMagnitude = 3.f / Level;
}

