#pragma once

#include "game/item/enchantment/EnchantmentData.h"
#include "CoreMinimal.h"
#include "MobEnchantmentTypes.h"

enum class EItemType : uint8;
class Random;

namespace game {
struct FDifficulty;

namespace enchantment { namespace mob { namespace generator {

TArray<FEnchantmentData> generate(EntityType, const EnchantmentCounts&, const FDifficulty&, Random* = nullptr);

}}}}

namespace mobenchgen = game::enchantment::mob::generator;