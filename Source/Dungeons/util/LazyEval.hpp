#pragma once

#include "LazyEval.h" // To get around fake intellisense issues

namespace lazyeval { namespace internal {

//
// Base
//
template <typename T, typename Derived>
T& Base<T, Derived>::get() const {
	if (!mHasValue) {
		mHasValue = true;
		static_cast<const Derived*>(this)->generateValue();
	}
	return mValue;
}

template <typename T, typename Derived>
void Base<T, Derived>::reset() const {
	mHasValue = false;
}


}}
