#pragma once

#include "CommonTypes.h"
#include <Array.h>
#include <UnrealString.h>

struct WeightedId;
class TileGroup;

namespace io { struct Level; }

namespace levelgen { namespace sourcedata {

struct SourceData;

struct FilterResult {
	FilterResult(bool passed);
	FilterResult(bool passed, FString description);

	explicit operator bool() const { return passed; }

	bool passed;
	TOptional<FString> description;
};

struct IdFilterState {
	const WeightedId& id;
	bool wantDescription;
};

using IdFilter = std::function<FilterResult(IdFilterState)>;

void filterTilesInStretches(io::Level&, const IdFilter& keep);
void filterTilesInStretchesWithLogging(io::Level&, const IdFilter& keep, bool logPredPassers = false);
void filterByUnlockKeys(SourceData&, const TArray<FString>&);

namespace filters {
      IdFilter  unlockedBy(const TileGroup&, const TArray<FString>& unlockKeys);
}

}}
