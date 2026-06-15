#include "Dungeons.h"
#include "RandomUtil.h"
#include "Random.h"
#include "SharedRandom.h"
#include <numeric>

namespace Util {

std::vector<size_t> randomIndices(size_t total, size_t take, Random* rnd /* = nullptr */) {
	if (take == 0) {
		return {};
	}
	auto r = thisOrSharedRandom(rnd);

	if (take == 1) {
		return { static_cast<size_t>(r->nextInt(total)) };
	}

	std::vector<size_t> indices(total);
	std::iota(begin(indices), end(indices), 0);

	for (size_t i = 0; i < take; ++i) {
		size_t j = r->nextInt(i, total);
		std::swap(indices[i], indices[j]);
	}
	return std::vector<size_t>(begin(indices), begin(indices) + take);
}

std::vector<size_t> randomIndices(size_t total, Random* rnd /* = nullptr */) {
	return randomIndices(total, total, rnd);
}

TArray<size_t> weightedIndexShuffle(const TArray<float>& weights, Random* rnd /*= nullptr*/) {
	rnd = thisOrSharedRandom(rnd);
	const size_t size = weights.Num();

	struct Item {
		Item(size_t index, float sortValue) : index(index), sortValue(sortValue) {}
		size_t index;
		float sortValue;
		bool operator<(const Item& rhs) const { return sortValue < rhs.sortValue; }
	};
	std::vector<Item> items;
	items.reserve(size);

	for (size_t i = 0; i < size; ++i) {
		const auto weight = weights[i] > 0 ? -std::pow(rnd->nextFloat(), 1.f / weights[i]) : 0;
		items.emplace_back(i, weight);
	}
	std::sort(begin(items), end(items));

	TArray<size_t> indices;
	indices.Reserve(size);
	algo::map_to(items, RETLAMBDA(it.index), indices);
	return indices;
}

int randomWeightedItemIndex(const TArray<float>& weights, Random* rnd /*= nullptr*/) {
	return randomWeightedItemIndex(weights, algo::sum(weights), rnd);
}

int randomWeightedItemIndex(const TArray<float>& weights, float totalWeight, Random* rnd /*= nullptr*/) {
	if (totalWeight > 0) {
		auto selection = thisOrSharedRandom(rnd)->nextFloat(totalWeight);

		for (int i = 0; i < weights.Num(); ++i) {
			selection -= weights[i];
			if (selection < 0) {
				return i;
			}
		}
	}
	return -1;
}

int randomWeightedItemIndex(const std::vector<float>& weights, Random* rnd /*= nullptr*/) {
	return randomWeightedItemIndex(weights, algo::sum(weights), rnd);
}

int randomWeightedItemIndex(const std::vector<float>& weights, float totalWeight, Random* rnd /*= nullptr*/) {
	if (totalWeight > 0) {
		auto selection = thisOrSharedRandom(rnd)->nextFloat(totalWeight);

		for (int i = 0; i < weights.size(); ++i) {
			selection -= weights[i];
			if (selection < 0) {
				return i;
			}
		}
	}
	return -1;
}

}
