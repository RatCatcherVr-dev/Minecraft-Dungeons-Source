#pragma once

#include <limits>

struct IntChanged {
	IntChanged(int initialValue)
		: value(initialValue) {
	}
	
	IntChanged()
		: value(std::numeric_limits<int>::min()) {
	}

	bool update(int newValue) {
		int old = value;
		value = newValue;
		return newValue != old;
	}
	int value;
};
