#pragma once

#include "CommonTypes.h"

template <typename T>
using IndexedPair = Pair<size_t, T>;

enum class SplitPosition { None, Before, After };

SplitPosition splitBeforeIf(bool b) { return b ? SplitPosition::Before : SplitPosition::None; }
SplitPosition splitAfterIf(bool b) { return b ? SplitPosition::After : SplitPosition::None; }

template <typename T>
std::vector<std::vector<T>> splitIntoGroups(const std::vector<T>& vs, const std::function<SplitPosition(IndexedPair<const T&>)> pred) {
	std::vector<std::vector<T>> out;

	size_t last = 0;
	for (size_t i = 0; i < vs.size(); ++i) {
		const auto split = pred(IndexedPair<const T&>(i, vs[i]));
		if (split == SplitPosition::None) {
			continue;
		}
		const size_t next = split == SplitPosition::Before ? i : i + 1;
		out.push_back(std::vector<io::Stretch>(vs.begin() + last, vs.begin() + next));
		last = next;
	}
	if (last < vs.size()) {
		out.push_back(std::vector<io::Stretch>(vs.begin() + last, vs.end()));
	}
	return out;
}
