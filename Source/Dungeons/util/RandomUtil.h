#pragma once

#include "Random.h"
#include "util/Algo.h"

namespace Util {

std::vector<size_t> randomIndices(size_t total, size_t take, Random* = nullptr);
std::vector<size_t> randomIndices(size_t, Random* = nullptr);

TArray<size_t> weightedIndexShuffle(const TArray<float>& weights, Random* = nullptr);

template <typename It>
It randomChoice(It begin, It end, Random* rnd = nullptr) {
	std::advance(begin, thisOrSharedRandom(rnd)->nextInt(static_cast<int>(std::distance(begin, end))));
	return begin;
}

template <typename T>
auto randomChoice(T& collection, Random* rnd = nullptr) -> decltype(std::begin(collection)) {
	return randomChoice(std::begin(collection), std::end(collection), rnd);
}

template <typename T>
auto randomChoice(const T& collection, Random* rnd = nullptr) -> decltype(std::begin(collection)) {
	return randomChoice(std::begin(collection), std::end(collection), rnd);
}

template <typename It>
static auto randomChoiceOrEmpty(It begin, It end, Random* rnd = nullptr) {
	using T = remove_cvref_t<decltype(*begin)>;
	if (const auto size = static_cast<int>(std::distance(begin, end))) {
		std::advance(begin, thisOrSharedRandom(rnd)->nextInt(size));
		return TOptional<T>(*begin);
	}
	return TOptional<T>{};
}

template <typename Collection>
static auto randomChoiceOrEmpty(const Collection& collection, Random* rnd = nullptr) {
	return randomChoiceOrEmpty(algo::copyable_begin(collection), algo::copyable_end(collection), rnd);
}

int randomWeightedItemIndex(const TArray<float>& weights, Random* = nullptr);
int randomWeightedItemIndex(const TArray<float>& weights, float totalWeight, Random* = nullptr);
int randomWeightedItemIndex(const std::vector<float>& weights, Random* = nullptr);
int randomWeightedItemIndex(const std::vector<float>& weights, float totalWeight, Random* = nullptr);

template <typename T>
T randomWeightedItem(const std::vector<std::pair<float, T>>& items, Random* rnd = nullptr) {
	float totalWeight = 0;
	const auto weights = algo::map_tarray(items, RETLAMBDA((totalWeight += it.first, it.first)));
	return items[randomWeightedItemIndex(weights, totalWeight, rnd)].second;
}

}
