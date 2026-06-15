#pragma once

#include "CoreMinimal.h"
#include "MobEnchantmentTypes.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"

enum class EItemType : uint8;
enum class EntityType : uint32;

namespace game { struct FDifficulty; }

namespace game { namespace enchantment { namespace mob {

enchgen::AvailableEnchantments getAvailableEnchantmentsForMob(EntityType, const FDifficulty&);

bool isEnchantmentApplicableFor(EEnchantmentTypeID, const TArray<EEnchantmentCategory>&, const FDifficulty&);

}}}
