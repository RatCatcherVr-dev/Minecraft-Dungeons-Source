#pragma once

#include "EnchantmentGeneratorTypes.h"
#include "CoreMinimal.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/item/ItemType.h"
#include "EnchantmentGeneratorConfigs.h"

enum class EItemType : uint8;
class Random;

namespace game {
struct FDifficulty;

namespace enchantment { namespace generator {

using Enchantments = TArray<const EnchantmentType*>;

struct AvailableEnchantments {
	Enchantments powerful;
	Enchantments nonPowerful;
};

AvailableEnchantments getAvailableEnchantmentsForType(const FItemId&);

TArray<FEnchantmentData> generate(const FItemId&, Counts, Random* = nullptr);
TArray<FEnchantmentData> generate(const AvailableEnchantments&, Counts, Random* = nullptr);

}}}
