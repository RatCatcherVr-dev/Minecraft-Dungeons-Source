#pragma once
#include "world/level/BlockPos.h"

template <typename T>
class TerrainGrid {	
public:
	TerrainGrid(const T& defaultValue) : defaultValue { defaultValue } {}

	const T& get(int x, int y) const {
		if (x < 0 || y < 0) {
			return defaultValue;
		}

		if (y >= lines.size()) {
			return defaultValue;
		}

		const auto& line = lines[y];
		if (x >= line.size()) {
			return defaultValue;
		}

		return line[x];
	}

	void set(int x, int y, const T& value) {
		if (x < 0 || y < 0) {
			return;
		}
		
		for (auto i = lines.size(); i <= y; i++) {
			lines.push_back({});			
		}

		auto& line = lines[y];

		if (x >= line.size()) {
			line.resize(x + 1, defaultValue);
		}

		line[x] = value;
	}

	void clear() {
		lines.clear();
	}

private:
	T defaultValue;
	std::vector<std::vector<T>> lines;
};

