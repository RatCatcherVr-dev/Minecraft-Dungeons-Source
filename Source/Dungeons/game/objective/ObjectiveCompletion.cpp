#include "Dungeons.h"
#include "ObjectiveCompletion.h"

namespace game { namespace objective {

Completion::Completion(size_t count, size_t max)
	: mCount(count)
	, mMax(max) {
}

Completion Completion::Failed() {
	return Completion(0, -1);
}

bool Completion::isPending() const {
	return !isFailed() && !isCompleted();
}

bool Completion::isCompleted() const {
	return mCount == mMax;
}

bool Completion::isFailed() const {
	return mMax < 0;
}

float Completion::fraction() const {
	return static_cast<float>(mCount) / mMax;
}

int Completion::current() const {
	return mCount;
}

int Completion::max() const {
	return mMax;
}

bool Completion::operator==(const Completion& rhs) const {
	return mCount == rhs.mCount && mMax == rhs.mMax;
}

bool Completion::operator!=(const Completion& rhs) const {
	return !operator==(rhs);
}

}}
