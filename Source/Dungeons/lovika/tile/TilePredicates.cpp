#include "Dungeons.h"
#include "TilePredicates.h"
#include "game/util/UnlockKeyUtils.h"
#include "util/Algo.hpp"
#include "util/BooleanOperatorMacros.h"
#include <string>

extern const std::string DeadEndTag = "deadend";

namespace tilepredicates {

GENERATE_BOOLEAN_PRED_OPERATORS(TileRef);

TilePredicate id(const std::string& id) {
	return [lowerId = Util::toLower(id)](TileRef prefab) { return prefab.lowerId() == lowerId || prefab.originalLowerId() == lowerId; };
}

TilePredicate hasTag(const std::string& tag) {
	return [lowerTag = Util::toLower(tag)](TileRef prefab) { return prefab.tags().has_alreadyLowerCase(lowerTag); };
}

MetaTilePredicate unlockedBy(const TArray<FString>& keys) {
	return [unlocker = unlockkey::createPredicate(keys)](MetaTileRef prefab) {
		return unlockkey::unlocksAny(unlocker, prefab.metadata.unlockKeys).Get(true);
	};
}

const TilePredicate& isDeadEnd() {
	static const TilePredicate pred = hasTag(DeadEndTag);
	return pred;
}

TilePredicate isFromObjectGroups(const std::vector<std::string>& objectGroups) {
	return [lowerObjectGroups = algo::map_vector(objectGroups, Util::toLower)](TileRef prefab) {
		return algo::contains(lowerObjectGroups, prefab.lowerObjectGroupId());
	};
}

TilePredicate isDeadEndFromObjectGroups(const std::vector<std::string>& objectGroups) {
	return isDeadEnd() && isFromObjectGroups(objectGroups);
}

}
