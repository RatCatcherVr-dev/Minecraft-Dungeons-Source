#pragma once

#include "BtEvalState.h"
#include "BtTime.h"
#include "BtRef.h"
#include "util/NewType.h"

namespace bt {

using Action = std::function<void(StateRef)>;

template <typename T>
using Action1 = std::function<void(StateRef, T)>;
using Action1Bool = Action1<bool>;

template <typename T>
using Provider = std::function<T(StateRef)>;

using Pred = Provider<bool>;

template <typename T>
Provider<T> value(T value) {
	return [value](StateRef) { return value; };
}

struct Absolute : NewType<float> {
	using NewType<float>::NewType;
};

struct Relative : NewType<float> {
	using NewType<float>::NewType;
};

}
