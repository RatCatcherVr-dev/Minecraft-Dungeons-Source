#include "Dungeons.h"
#include "BlockRegionUtil.h"
#include "BlockCuboid.h"
#include "BlockRegionTransform.h"
#include "BlockPosTransform.h"
#include "OrientationDataCalculator.h"

Unique<BlockRegion> BlockRegionUtil::cloneWith(const BlockRegion& region, const BlockRegionTransform& transform) {
	auto blockPosTransform = blockpostransform::create(region.size(), transform);
	auto blockTransform = orientationDataTransform(blockPosTransform);
	auto newRegion = std::make_unique<BlockRegion>(transform.transformSize(region.size()));
	for (auto p : BlockCuboid::fromSize(region.size())) {
		newRegion->setBlock(blockPosTransform(p), blockTransform(region.getBlock(p)));
	}
	return newRegion;
}

void BlockRegionUtil::for_each(const BlockRegion& blocks, const BlockCallback& callback) {
	for (auto pos : BlockPosIteration::range(blocks.size())) {
		callback(pos, blocks.getBlock(pos));
	}
}

void BlockRegionUtil::for_each_matching(const BlockRegion& blocks, const BlockPredicate& predicate, const BlockCallback& callback) {
	return for_each(blocks, [&predicate, &callback](auto pos, auto block) {
		if (predicate(pos, block)) {
			callback(pos, block);
		}
	});
}
