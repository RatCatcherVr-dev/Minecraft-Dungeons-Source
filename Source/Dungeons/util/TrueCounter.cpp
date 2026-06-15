#include "Dungeons.h"
#include "TrueCounter.h"
#include "Algo.h"

size_t TrueCounter::count() const {
	return algo::count(bools, true);
}

size_t TrueCounter::firstIndex() const {
	return bools.IndexOfByKey(true);
}

TArray<size_t> TrueCounter::indices() const {
	TArray<size_t> indices;
	for (int i = 0; i < bools.Num(); ++i) {
		if (bools[i]) indices.Add(i);
	}
	return indices;
}

TrueCounter::TrueCounter(TArray<bool> bools)
	: bools(std::move(bools)) {
}

bool TrueCounter::isEmpty() const {
	return bools.Contains(true) == false;
}
