#pragma once

#include "EnchantmentGeneratorTypes.h"

enum class EItemType : uint8;

namespace game {
struct FDifficulty;

namespace enchantment { namespace generator { namespace configs {

Counts FromDifficulty(const FDifficulty&, bool isBoss = false, bool isUnique = false);
Counts NetheriteFromDifficulty(const FDifficulty&, bool isBoss = false, bool isUnique = false);
Counts FromDifficultyFraction(float combinedDifficultyFraction, bool isBoss = false, bool isUnique = false); // Useful for testing (e.g. by giving fraction via console)
Counts NetheriteFromDifficultyFraction(float combinedDifficultyFraction, bool isBoss = false, bool isUnique = false); // Useful for testing (e.g. by giving fraction via console)
Counts ReEnchantFromDifficulty(const FDifficulty& difficulty, const bool isUnique);
Counts FromItemPower(float itemPower, bool isBoss, bool isUnique);

}}}}
