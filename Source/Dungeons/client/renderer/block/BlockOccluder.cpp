#include "Dungeons.h"

#include "BlockOccluder.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "world/level/block/SlabBlock.h"
#include "world/level/material/Material.h"

BlockOccluder::BlockOccluder(BlockTessellatorCache& cache, const Block& block, const BlockPos& blockPos, const AABB& shape, const Facing::FacingIDList& checkFace, bool renderingGUI) 
	: mBlockCache(cache)
	, mBlock(block)
{
	if (!renderingGUI) {
		mFacingOccluded.flip();	//set all to true

		for (auto& face : checkFace) {
			_updateRenderFace(block, blockPos, shape, face);
		}
	}
}

bool BlockOccluder::occludes(FacingID face) const {
	DEBUG_ASSERT(face >= 0 && face <= Facing::EAST, "Wrong face id!");
	return mFacingOccluded[face];
}

void BlockOccluder::_setOccluded(FacingID face, bool occluded) {
	DEBUG_ASSERT(face >= 0 && face <= Facing::EAST, "Wrong face id!");
	mFacingOccluded[face] = occluded;
}

void BlockOccluder::_updateRenderFace(const Block& block, const BlockPos& p, const AABB& shape, FacingID face) {
	// If it is the bottom of the world, cull of the faces
	if(p.y < 0 && face != Facing::UP) {
		_setOccluded(face, true);
		return;
	}

	// REFACTOR TODO: This code should check render/visual 
	// shape rather than property flags.
	if (block.hasProperty(BlockProperty::HalfSlab))
		_setOccluded(face, _shouldOccludeSlab(face, shape, p));
	else if (block.isStairBlock())
		_setOccluded(face, _shouldOccludeStairs(face, shape, p));
	else if (block.hasProperty(BlockProperty::Leaf))
		_setOccluded(face, _shouldOccludeLeaves(face, shape, p));
	else if (block.mID == Block::mGlass->mID)
		_setOccluded(face, _shouldOccludeGlass(face, shape, p));
	else if (block.hasProperty(BlockProperty::ThinConnects2D))
		_setOccluded(face, _shouldOccludeThinFence(face, shape, p));
	else if (block.hasProperty(BlockProperty::Connects2D))
		_setOccluded(face, _shouldOccludeFence(face, shape, p));
	else if (block.getMaterial().isLiquid())
		_setOccluded(face, _shouldOccludeLiquid(face, shape, p));
	else if (block.hasProperty(BlockProperty::Portal))
		_setOccluded(face, _shouldOccludePortal(face, shape, p));
	else if (block.hasProperty(BlockProperty::Slime))
		_setOccluded(face, _shouldOccludeSlime(face, shape, p));
	else
		_setOccluded(face, _shouldOcclude(face, shape, p));
}

bool BlockOccluder::_shouldOcclude(FacingID face, const AABB& shape, const BlockPos& p) {
	return !_shouldRenderFace(p.relative(face), face, shape);
}

bool BlockOccluder::_shouldOccludeSlab(FacingID face, const AABB& shape, const BlockPos& pos) {
	BlockPos neighbourPos = pos.relative(face);
	if(face != Facing::UP && face != Facing::DOWN && !_shouldRenderFace(neighbourPos, face, shape)) {
		return true;
	}

	bool isUpper = !SlabBlock::isBottomSlab(mBlockCache.getData(pos));
	// Check top and bottom sides
	if(isUpper) {
		if(face == Facing::DOWN) {
			return false;
		}

		if(face == Facing::UP && _shouldRenderFace(neighbourPos, face, shape)) {
			return false;
		}
	} else {
		if(face == Facing::UP) {
			return false;
		}

		if(face == Facing::DOWN && _shouldRenderFace(neighbourPos, face, shape)) {
			return false;
		}
	}

	// Check if neighbour is a slab
	const Block& neighbourBlock = mBlockCache.getBlock(neighbourPos);
	if (neighbourBlock.hasProperty(BlockProperty::HalfSlab)) {
		if (isUpper != SlabBlock::isBottomSlab(mBlockCache.getData(neighbourPos))) {
			return true;
		}
	}

	return false;
}

namespace blockoccluderinternal {

bool isStairsUpsideDown(DataID data) {
	return Block::mStoneStairs->getBlockState(BlockState::TopSlotBit).getBool(data);
}
int getStairsDirection(DataID data) {
	return Block::mStoneStairs->getBlockState(BlockState::Direction).get<int>(data);
}
int getStairsFacing(DataID data) {
	constexpr std::array<FacingID, 4> directionFacings { { Facing::EAST, Facing::WEST, Facing::SOUTH, Facing::NORTH } };
	return directionFacings[getStairsDirection(data)];
}

}

bool BlockOccluder::_shouldOccludeStairs(FacingID face, const AABB& shape, const BlockPos& pos) {
	const BlockPos neighbourPos = pos.relative(face);
	if(face != Facing::UP && face != Facing::DOWN && !_shouldRenderFace(neighbourPos, face, shape)) {
		return true;
	}

	const auto data = mBlockCache.getData(pos);
	const bool isUpper = blockoccluderinternal::isStairsUpsideDown(data);

	// Check top and bottom sides
	if(isUpper) {
		if(face == Facing::DOWN) {
			return false;
		}

		if(face == Facing::UP && _shouldRenderFace(neighbourPos, face, shape)) {
			return false;
		}
	} else {
		if(face == Facing::UP) {
			return false;
		}

		if(face == Facing::DOWN && _shouldRenderFace(neighbourPos, face, shape)) {
			return false;
		}
	}

	// Check if neighbour is a stair
	const Block& neighbourBlock = mBlockCache.getBlock(neighbourPos);
	if (neighbourBlock.isStairBlock()) {
		const auto oppositeFace = Facing::OPPOSITE_FACING[face];
		const auto neighbourData = mBlockCache.getData(neighbourPos);
		const auto neighbourFacing = blockoccluderinternal::getStairsFacing(neighbourData);

		if (const bool neighbourIsFacingItsBack = (neighbourFacing == oppositeFace)) {
			return true;
		}
		const auto stairsFacing = blockoccluderinternal::getStairsFacing(data);
		const bool isSide = stairsFacing != face && stairsFacing != oppositeFace;
		const bool hasSameUpDownBit = isUpper == blockoccluderinternal::isStairsUpsideDown(neighbourData);
		return hasSameUpDownBit && stairsFacing == (isSide ? neighbourFacing : Facing::OPPOSITE_FACING[neighbourFacing]);
	}

	return false;
}

bool BlockOccluder::_shouldOccludeLeaves(FacingID face, const AABB& shape, const BlockPos& p) {
	//leaves are now opaque, just occlude like a normal block
	return _shouldOcclude(face, shape, p);
}

bool isVerticalOccluder(const AABB& shape) {
	return shape.min.x <= 0 && shape.max.x >= 1 && shape.min.z <= 0 && shape.max.z >= 1;
}

bool BlockOccluder::_shouldRenderFace(const BlockPos& pos, FacingID face, const AABB& shape) {

	const float Epsilon = 0.0005f;

	if((face == Facing::DOWN   && shape.min.y > Epsilon) ||
		(face == Facing::UP    && shape.max.y < 1 - Epsilon) ||
		(face == Facing::NORTH && shape.min.z > Epsilon) ||
		(face == Facing::SOUTH && shape.max.z < 1 - Epsilon) ||
		(face == Facing::WEST  && shape.min.x > Epsilon) ||
		(face == Facing::EAST  && shape.max.x < 1 - Epsilon)) {
		return true;
	}

	const Block& block = mBlockCache.getBlock(pos);

	//HACK this code most definitely doesn't belong here
	if (block.hasProperty(BlockProperty::Leaf)) {
		return !mBlock.hasProperty(BlockProperty::Leaf);
	}

	if (face == Facing::UP && block.isType(Block::mWoolCarpet)) {
		return false;
	}

	if (_isHalfCubeOpaque(block)) {
		// If we have a neighbor, and we are testing a n,s,e,w face,
		// then lets check our topSnow height vs neighbor height for occlusion.
		// If our height is >= neighbor height, then we occlude the neighbor's side.
		AABB neighborShape = block.getVisualShape(mBlockCache.getData(pos), neighborShape);
		if (isVerticalOccluder(neighborShape)) {
			if (face == Facing::UP) {
				return neighborShape.min.y > 0;
			}
			else if (face == Facing::DOWN) {
				return neighborShape.max.y < 1.f;
			}
			else if (shape.max.y <= neighborShape.max.y && neighborShape.min.y == 0) {
				return false;
			}
		}
		return true;
	}

	return !mBlockCache.getBlockGraphicsPack().get(block.getId()).isFullAndOpaque();
}

bool BlockOccluder::_isTransparent(const BlockPos& pos) {
	const Block& block = mBlockCache.getBlock(pos);
	return !mBlockCache.getBlockGraphicsPack().get(block.getId()).isFullAndOpaque();
}

bool BlockOccluder::_isHalfCubeOpaque(const Block& block) {
	auto layer = block.getRenderLayer();
	auto shape = mBlockCache.getBlockGraphicsPack().get(block.getId()).getBlockShape();

	return (layer == BlockRenderLayer::RENDERLAYER_OPAQUE || layer == BlockRenderLayer::RENDERLAYER_OPAQUE_FILL || layer == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE) &&
		(shape == BlockShape::BLOCK_HALF || shape == BlockShape::TOP_SNOW || (shape == BlockShape::BLOCK && !block.isSolid()));
}

bool BlockOccluder::isDeepLeafBlock(const BlockPos& pos) {
	if (Block::mLeaves->getRenderLayer() == BlockRenderLayer::RENDERLAYER_OPAQUE) {
		return true;
	}
		
	if (_isTransparent(pos.below())) {
		return false;
	}

	if (_isTransparent(pos.north())) {
		return false;
	}

	if (_isTransparent(pos.south())) {
		return false;
	}

	if (_isTransparent(pos.west())) {
		return false;
	}

	if (_isTransparent(pos.east())) {
		return false;
	}

	//"up" is an important case because we also want to exclude topSnow in forests
	const Block& block = mBlockCache.getBlock(pos.above());
	if (!block.isType(Block::mAir)) {
		if (Block::mTranslucency[block.mID] > 0.f)
		{
			if (!block.hasProperty(BlockProperty::Leaf | BlockProperty::TopSnow))
				return false;
		}
	} else {
		return false;
	}

	return true;
}

bool BlockOccluder::_shouldOccludeGlass(FacingID face, const AABB& shape, const BlockPos& pos) {
	const Block& neighbour = mBlockCache.getBlock(pos.relative(face));
	const Block& block = mBlockCache.getBlock(pos);
	if(!neighbour.isType(Block::mAir)) {
		if(block.mID == neighbour.mID) {
			return true;
		}

		if (mBlockCache.getBlockGraphicsPack().get(neighbour.getId()).isFullAndOpaque()) {
			return true;
		}
	}

	return !_shouldRenderFace(pos.relative(face), face, shape);
}

bool BlockOccluder::_shouldOccludeThinFence(FacingID face, const AABB& shape, const BlockPos& pos) {
	const Block& neighbour = mBlockCache.getBlock(pos.relative(face));
	const Block& block = mBlockCache.getBlock(pos);
	if(!neighbour.isType(Block::mAir)) {
		if(block.mID == neighbour.mID) {
			return false;
		}
	}

	return !_shouldRenderFace(pos.relative(face), face, shape);
}

bool BlockOccluder::_shouldOccludeFence(FacingID face, const AABB& shape, const BlockPos& pos) {
	if(face == Facing::DOWN) {
		return !_shouldRenderFace(pos.relative(face), face, shape);
	}

	return false;
}

bool BlockOccluder::_shouldOccludeLiquid(FacingID face, const AABB& shape, const BlockPos& pos) {
	const Block& neighbour = mBlockCache.getBlock(pos.relative(face));
	const Block& block = mBlockCache.getBlock(pos);

	if (block.isType(Block::mAir)) {
		return false;
	}

	if(!neighbour.isType(Block::mAir)) {
		const Material& neighbourMaterial = neighbour.getMaterial();
		if(neighbourMaterial == block.getMaterial()) {
			return true;
		}

		if(neighbourMaterial.isType(MaterialType::Ice)) {
			return true;
		}
	}

	if (face == Facing::UP) {

		BlockPos blockAbovePos = pos.relative(Facing::UP);
		const Block& blockAbove = mBlockCache.getBlock(blockAbovePos);
		
		if (!mBlockCache.getBlockGraphicsPack().get(blockAbove.getId()).isFullAndOpaque()) {
			return false;
		}

		const std::initializer_list<std::initializer_list<FacingID>> faceList = {
			{ Facing::NORTH },
			{ Facing::SOUTH },
			{ Facing::WEST },
			{ Facing::EAST },
			{ Facing::NORTH, Facing::EAST },
			{ Facing::NORTH, Facing::WEST },
			{ Facing::SOUTH, Facing::EAST },
			{ Facing::SOUTH, Facing::WEST },
		};

		// if any of the blocks neighboring the block above this position are not solid, 
		// then the liquid will be displaced a slight amount, so we need to render the upper
		// face of the liquid.
		for (auto& facingList : faceList) {
			BlockPos blockAboveNeighborPos = blockAbovePos;
			for (auto& facing : facingList) {
				blockAboveNeighborPos = blockAboveNeighborPos.relative(facing);
			}
			const Block& blockAboveNeighbor = mBlockCache.getBlock(blockAboveNeighborPos);
			if (!mBlockCache.getBlockGraphicsPack().get(blockAboveNeighbor.getId()).isFullAndOpaque() 
				&& blockAboveNeighbor.mID != block.mID 
				&& blockAboveNeighbor.mID != Block::mFlowingWater->mID) {
				return false;
			}

		}
		return true;
	}

	return !_shouldRenderFace(pos.relative(face), face, shape);
}

bool BlockOccluder::_shouldOccludePortal(FacingID face, const AABB& shape, const BlockPos& pos) {
	const Block& neighbour = mBlockCache.getBlock(pos.relative(face));
	if (!neighbour.isType(Block::mAir) && neighbour.hasProperty(BlockProperty::Portal)) {
		return true;
	}

	return !_shouldRenderFace(pos.relative(face), face, shape);
}

bool BlockOccluder::_shouldOccludeSlime(FacingID face, const AABB& shape, const BlockPos& pos) {
	const BlockPos& relativePos = pos.relative(face);
	const Block& neighbour = mBlockCache.getBlock(relativePos);
	//	If the neighbor is a slime block and we're testing against the outer shell, occlude these faces.
	if (neighbour.isType(Block::mSlimeBlock) && shape.max.x >= 1) {
		return true;
	}

	return !_shouldRenderFace(relativePos, face, shape); 
}

bool BlockOccluder::anyVisible() const {
	return !mFacingOccluded.all();
}
