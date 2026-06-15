#pragma once

#include "util/NewType.h"
#include <cstdint>

template <typename T>
struct WalkableHeightT : public NewType<T> {
	using NewType<T>::NewType;

	WalkableHeightT<T> withOffset(int8_t offset) const {
		return this->value != 0 ? WalkableHeightT<T>(this->value + offset) : *this;
	}

	TOptional<T> toHeightThreshold() const {
		return this->value > 0 ? this->value : TOptional<T>{};
	}

	static WalkableHeightT<T> Unreachable() { return WalkableHeightT<T>(0); }
	static WalkableHeightT<T> Min()         { return WalkableHeightT<T>(1); }
	static WalkableHeightT<T> Max()         { return WalkableHeightT<T>(std::numeric_limits<T>::max() / 2); }
};

using WalkableHeight = WalkableHeightT<int16_t>;
