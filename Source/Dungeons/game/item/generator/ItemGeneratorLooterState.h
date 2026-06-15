#pragma once

#include "game/item/InventoryItemData.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/difficulty/DifficultyRecommendation.h"
#include <Set.h>

enum class ELevelNames : uint8;
class APlayerCharacter;

namespace game { namespace item { namespace generator {

struct Looter {
	const TArray<FInventoryItemData>& inventoryItems;
	const TSet<ELevelNames>& unlockedLevels;
	const game::DifficultyRecommendation* difficultyRecommendation;
	const TOptional<game::DifficultyStats>& highestCompletedDifficultyStats;

	float highestItemPowerOf(const FItemId&) const;
};

namespace looters {
	Looter Unknown();
	Looter FromPlayerCharacter(const APlayerCharacter&);
	Looter FromConsolePlayerCharacter(const APlayerCharacter& playerCharacter);
}

}}}
