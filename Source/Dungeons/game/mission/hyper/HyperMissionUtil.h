#pragma once

#include "lovika/io/LevelFileCommonTypes.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include <UnrealString.h>
#include <set>

namespace game { class Tiles; }

// These are temporarily placed in this file/namespace while
// working with hypermissions, before we know where to put it
namespace game { namespace util {

std::set<FString> getUniqueSubMissionIds(const game::Tiles&);

template <typename C> int collection_size(const C& collection) { return collection.size(); }
template <typename T> int collection_size(const TArray<T>& collection) { return collection.Num(); }

template <typename Collection>
Collection randomSample(const Collection& collection, int maxCount, Random& rnd, bool forceShuffle = false) {
	const auto totalCount = collection_size(collection);
	if (totalCount <= maxCount) {
		return forceShuffle ? algo::random::shuffledCopy(collection, rnd) : collection;
	}
	return algo::map_as<Collection>(Util::randomIndices(totalCount, maxCount, &rnd), RETLAMBDA(collection[it]));
}

}}

namespace levelgen { namespace hajper { namespace ids {

const CaseInsensitiveId& HyperDungeon();
const CaseInsensitiveId& HyperLevel();

}}}
