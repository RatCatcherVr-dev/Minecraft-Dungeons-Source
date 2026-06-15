#pragma once

#include "world/level/BlockPos.h"

struct IntRect {
	IntRect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
	int x1() const { return x + w; }
	int y1() const { return y + h; }
	int x, y, w, h;
};

template <typename T>
class Grid2 {
public:
	Grid2(int w, int h)
		: Grid2(0, 0, w, h) {
	}

	Grid2(int w, int h, T init)
		: Grid2(0, 0, w, h, std::move(init)) {
	}

	Grid2(int x, int y, int w, int h)
		: mRect(x, y, w, h)
		, m(w * h) {
	}

	Grid2(int x, int y, int w, int h, T init)
		: mRect(x, y, w, h)
		, m(w * h, std::move(init)) {
	}

	T get(BlockPos p) const {
		return m[index(p.x, p.z)];
	}
	T get(int x, int y) const {
		return m[index(x, y)];
	}

	T getOrDefault(int x, int y, T orDefault) const {
		return inRange(x, y) ? m[index(x, y)] : orDefault;
	}
	T getOrDefault(BlockPos p, T orDefault) const {
		return getOrDefault(p.x, p.z, orDefault);
	}

	T& ref(int x, int y) {
		return m[index(x, y)];
	}
	T& ref(BlockPos p) {
		return m[index(p.x, p.z)];
	}
	const T& ref(int x, int y) const {
		return m[index(x, y)];
	}
	const T& ref(BlockPos p) const {
		return m[index(p.x, p.z)];
	}

	T& refOrDefault(int x, int y, T& orDefault) {
		return inRange(x, y) ? m[index(x, y)] : orDefault;
	}
	T& refOrDefault(BlockPos p, T& orDefault) {
		return refOrDefault(p.x, p.z, orDefault);
	}
	const T& refOrDefault(int x, int y, const T& orDefault) const {
		return inRange(x, y) ? m[index(x, y)] : orDefault;
	}
	const T& refOrDefault(BlockPos p, const T& orDefault) const {
		return refOrDefault(p.x, p.z, orDefault);
	}

	void set(int x, int y, T value) {
		m[index(x, y)] = value;
	}

	bool inRange(int x, int y) const {
		x = _x(x);
		y = _y(y);
		return x >= 0 && x < mRect.w && y >= 0 && y < mRect.h;
	}

	int width() const { return mRect.w; }
	int height() const { return mRect.h; }

	IntRect rect() const { return mRect; }

	void setOffset(int x, int y) {
		mRect.x = x;
		mRect.y = y;
	}
	int offsetX() const { return mRect.x; }
	int offsetY() const { return mRect.y; }

	std::vector<T> m;
private:
	inline int _x(int x) const { return x - mRect.x; }
	inline int _y(int y) const { return y - mRect.y; }
	int index(int x, int y) const { return _x(x) + _y(y) * mRect.w; }

	IntRect mRect;
};
