#pragma once
#include "world/level/BlockPos.h"

template <typename T, typename Updater>
class TerrainGridMag {
public:
	TerrainGridMag(const T& defaultValue)
		: data { defaultValue }
		, mag { defaultValue }
	{}

	const T& get(int x, int y) const {
		return data.get(x, y);
	}

	const T& getMag(int x, int y) const {
		return mag.get(x, y);
	}

	void set(int x, int y, const T& value) {
		data.set(x, y, value);

		Updater::update(data, x, y, mag);
	}

	void clear() {
		data.clear();
		mag.clear();
	}

private:
	TerrainGrid<T> data;
	TerrainGrid<T> mag;
};