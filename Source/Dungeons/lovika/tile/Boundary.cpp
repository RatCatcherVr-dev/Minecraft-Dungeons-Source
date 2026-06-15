#include "Dungeons.h"
#include "Boundary.h"
#include "util/Algo.h"

Boundary::Boundary(const BlockPos& position, int height)
	: position(position)
	, height(height) {
}

Boundary transformed(const Boundary& boundary, const BlockPosTransform& transform) {
	return { transform(boundary.position), boundary.height };
}

std::vector<Boundary> transformed(const std::vector<Boundary>& boundaries, const BlockPosTransform& transform) {
	std::vector<Boundary> out;
	out.reserve(boundaries.size());
	for (auto&& boundary : boundaries) {
		out.push_back(transformed(boundary, transform));
	}
	return out;
}

std::vector<BlockCuboid> mergeBoundaries(std::vector<Boundary> boundaries) {
	const auto merge = [](const auto& entries, auto isConsecutive, auto makeCluster) {
		std::vector<BlockCuboid> clusters;
		std::vector<Boundary> leftovers;

		if (!entries.empty()) {
			auto index = 0;
			const auto indexMax = entries.size() - 1;

			auto runLength = 1;

			while (index < indexMax) {
				const auto& current = entries[index];
				const auto& next = entries[index + 1];
				index++;

				if (isConsecutive(current, next)) {
					runLength++;
				}
				else {
					if (runLength > 1) {
						clusters.push_back(makeCluster(entries[index - runLength], runLength));
					}
					else {
						leftovers.push_back(current);
					}

					runLength = 1;
				}
			}

			if (runLength > 1) {
				clusters.push_back(makeCluster(entries[index - runLength + 1], runLength));
			}
			else {
				leftovers.push_back(entries[index]);
			}
		}
		return std::make_pair(clusters, leftovers);
	};

	std::sort(
		boundaries.begin(),
		boundaries.end(),
		[](const auto& a, const auto& b) { return a.position.z == b.position.z ? a.position.x < b.position.x : a.position.z < b.position.z; }
	);

	auto horizontalResult = merge(
		boundaries,
		[](const auto& current, const auto& next) { return next.position.x == current.position.x + 1 && next.position.z == current.position.z; },
		[](const auto& start, auto size) { return BlockCuboid::fromPositionAndSize(start.position, {size, start.height, 1}); }
	);

	std::sort(
		horizontalResult.second.begin(),
		horizontalResult.second.end(),
		[](const auto& a, const auto& b) { return a.position.x == b.position.x ? a.position.z < b.position.z : a.position.x < b.position.x; }
	);

	auto verticalResult = merge(
		horizontalResult.second,
		[](const auto& current, const auto& next) { return next.position.x == current.position.x && next.position.z == current.position.z + 1; },
		[](const auto& start, auto size) { return BlockCuboid::fromPositionAndSize(start.position, {1, start.height, size}); }
	);

	auto& result = horizontalResult.first;
	result.insert(result.end(), verticalResult.first.begin(), verticalResult.first.end());
	algo::map_to(verticalResult.second, RETLAMBDA(BlockCuboid::fromPositionAndSize(it.position, {1, it.height, 1})), result);

	return result;
}
