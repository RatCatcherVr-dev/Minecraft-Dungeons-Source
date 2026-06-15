#include "Dungeons.h"
#include "SourceData.h"
#include "SourceDataFilter.h"
#include "game/util/UnlockKeyUtils.h"
#include "lovika/io/LevelFile.h"
#include "lovika/tile/TilePredicates.h"
#include "util/Algo.hpp"
#include "util/Validation.h"
#include <LogMacros.h>
#include "util/StringUtil.h"

namespace levelgen { namespace sourcedata {

void filterTilesInStretches(io::Level& level, const IdFilter& keep) {
	for (auto& dungeon : level.dungeons) {
		for (auto& stretch : dungeon.stretches) {
			stretch.tiles = algo::copy_if(stretch.tiles, RETLAMBDA(keep({ it, false })));
		}
	}
}

void filterTilesInStretchesWithLogging(io::Level& level, const IdFilter& keep, bool logPredPassers) {
	for (auto& dungeon : level.dungeons) {
		for (auto& stretch : dungeon.stretches) {
			for (const auto& tile : std::exchange(stretch.tiles, {})) {
				const auto result = keep({ tile, true });

				static const FString defaultReason = "NoReasonGiven";

				if (result) {
					stretch.tiles.push_back(tile);
				} else {
					UE_LOG(LogLevelGeneration, Warning, TEXT("Filtering out id=%s. Reason: %s"), UTF8_TO_TCHAR(tile.id.c_str()), *result.description.Get(defaultReason));
				}
				if (result && logPredPassers) {
					UE_LOG(LogLevelGeneration, Warning, TEXT("Passing id=%s. Reason: %s"), UTF8_TO_TCHAR(tile.id.c_str()), *result.description.Get(defaultReason));
				}
			}
		}
	}
}

void filterByUnlockKeys(SourceData& sourceData, const TArray<FString>& unlockKeys) {
	filterTilesInStretches(sourceData.level, filters::unlockedBy(sourceData.tiles, unlockKeys));
	const auto unlockedByKeys = tilepredicates::unlockedBy(unlockKeys);
	sourceData.tiles = sourceData.tiles.filter(unlockedByKeys);
	sourceData.props = sourceData.props.filter(unlockedByKeys);
}

namespace filters {

IdFilter unlockedBy(const TileGroup& tileGroup, const TArray<FString>& unlockKeys) {
	return [&tileGroup, unlocker = unlockkey::createPredicate(unlockKeys), keyString = FString::Join(unlockKeys, TEXT(", "))](IdFilterState s) -> FilterResult {
		const auto* prefab = tileGroup.findById(s.id.lowerId);
		if (!s.wantDescription) {
			return !prefab || unlockkey::unlocksAny(unlocker, prefab->metadata.unlockKeys).Get(true);
		}

		// Extra description
		if (!prefab) {
			return { true, "No meta-tile" };
		}
		if (const auto maybeResult = unlockkey::unlocksAny(unlocker, prefab->metadata.unlockKeys)) {
			return { maybeResult.GetValue(), "TileUnlockKeys: " + stringutil::toFString(Util::join(prefab->metadata.unlockKeys, ", ")) + ", UnlockKeys: " + keyString };
		}
		return { true, "No unlock keys" };
	};
}

}

//
// FilterResult
//
FilterResult::FilterResult(bool passed)
	: passed(passed) {
}

FilterResult::FilterResult(bool passed, FString description)
	: passed(passed)
	, description(std::move(description)) {
}

}}
