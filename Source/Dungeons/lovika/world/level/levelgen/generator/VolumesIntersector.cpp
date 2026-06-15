#include "Dungeons.h"
#include "VolumesIntersector.h"

VolumesIntersector::VolumesIntersector(bool intersects2d)
	: mCheckXz(intersects2d)
	, mBounds{ BlockCuboid() }{
}

const BlockCuboid& VolumesIntersector::bounds() const {
	return mBounds.back();
}

bool VolumesIntersector::operator()(const BlockCuboid& volume) const {
	if (isEmpty()) {
		return false;
	}
	if (mCheckXz) {
		return bounds().intersectsXZ(volume) && std::any_of(mPlaced.rbegin(), mPlaced.rend(), [volume](const BlockCuboid& b) { return b.intersectsXZ(volume); });
	}
	return bounds().intersects(volume) && std::any_of(mPlaced.rbegin(), mPlaced.rend(), [volume](const BlockCuboid& b) { return b.intersects(volume); });
}

bool VolumesIntersector::isEmpty() const {
	return _size() == 0;
}

void VolumesIntersector::add(BlockCuboid volume) {
	const auto lastBound = mBounds.back();
	mBounds.emplace_back(lastBound);
	safeExpand(mBounds.back(), volume);

	mPlaced.push_back(std::move(volume));
}

int VolumesIntersector::pop_back(int count /* = 1 */) {
	checkf(_size() >= count, TEXT("Can't pop from empty stack!"));
	return _pop(count);
}

int VolumesIntersector::pop_back_safe(int count /*= 1*/) {
	return _pop(count);
}

int VolumesIntersector::_pop(int maxCount) {
	const int c = std::min(maxCount, _size());

	mPlaced.erase(mPlaced.end() - c, mPlaced.end());
	mBounds.erase(mBounds.end() - c, mBounds.end());
	return c;
}

int VolumesIntersector::_size() const {
	return static_cast<int>(mBounds.size()) - 1;
}
