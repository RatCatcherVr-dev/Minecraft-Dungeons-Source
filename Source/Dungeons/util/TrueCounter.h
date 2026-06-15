#pragma once

#include <Array.h>

struct TrueCounter {
	TrueCounter(TArray<bool>);
	bool isEmpty() const;
	size_t count() const;
	size_t firstIndex() const;
	TArray<size_t> indices() const;
private:
	TArray<bool> bools;
};
