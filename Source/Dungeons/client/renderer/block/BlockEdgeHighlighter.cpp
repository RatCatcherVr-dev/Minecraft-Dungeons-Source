#include "Dungeons.h"

#include "BlockEdgeHighlighter.h"
#include "client/renderer/block/BlockTessellatorCache.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "world/level/block/SlabBlock.h"
#include "world/level/material/Material.h"

TAutoConsoleVariable<int32> CVarHighlightEdges(
	TEXT("Dungeons.Level.EdgeHighlight"),
	3,
	TEXT("Set edge highlights")
	TEXT("= 0: None")
	TEXT("= 1: Camera facing")
	TEXT("= 2: Away facing")
	TEXT("= 3: Full square (default)"),
	ECVF_Cheat);

BlockEdgeHighlighter::BlockEdgeHighlighter(
	BlockTessellatorCache& cache,
	const Block& block,
	const BlockPos& blockPos,
	const AABB& shape,
	const Facing::FacingIDList& faces
) : mBlockCache(cache), mBlock(block) {
	for (auto& face : faces) {
		_updateRenderFace(block, blockPos, shape, face);
	}
}

void BlockEdgeHighlighter::_setIsFacingEdge(FacingID face, bool isEdge) {
	mFacingIsEdge[face] = isEdge;
}

bool BlockEdgeHighlighter::isFacingEdge(FacingID face) {
	return CVarHighlightEdges.GetValueOnAnyThread() > 0 && mFacingIsEdge[face];
}

EdgeHighlight BlockEdgeHighlighter::getHighlightType()
{
	return static_cast<EdgeHighlight>(CVarHighlightEdges.GetValueOnAnyThread());
}

bool BlockEdgeHighlighter::isFaceFacingEdge(FacingID face, FacingID facing) {
	return mFacesFacingEdge[face][facing];
}

void BlockEdgeHighlighter::_updateRenderFace(
	const Block& block,
	const BlockPos& p,
	const AABB& shape,
	FacingID face
) {
	_setIsFacingEdge(face, _checkIsEdge(block, face, shape, p));
}

const std::array<bool, 90> shouldBlockShapeBeHighlighted = []{
  std::array<bool, 90> shapes;
  shapes[enum_cast(BlockShape::BLOCK)] = true;
  shapes[enum_cast(BlockShape::TREE)] = true;
  shapes[enum_cast(BlockShape::STAIRS)] = true;
  shapes[enum_cast(BlockShape::TOP_SNOW)] = true;
  shapes[enum_cast(BlockShape::BLOCK_HALF)] = true;
  return shapes;
}();

const std::array<bool, 90> isBlockShapeNotEdge = []{
  std::array<bool, 90> shapes;
  shapes[enum_cast(BlockShape::BLOCK)] = true;
  shapes[enum_cast(BlockShape::TREE)] = true;
  shapes[enum_cast(BlockShape::STAIRS)] = true;
  shapes[enum_cast(BlockShape::BLOCK_HALF)] = true;
  return shapes;
}();

bool BlockEdgeHighlighter::_checkIsEdge(
	const Block& block,
	FacingID face,
	const AABB& aabb,
	const BlockPos& pos
) {
	// We need to exclude the wool carpets
	if (face == Facing::UP && block.isType(Block::mWoolCarpet)) {
		return false;
	}

	const BlockGraphics& blockGraphics = mBlockCache.getBlockGraphicsPack().get(block.getId());
	BlockShape blockShape = blockGraphics.getBlockShape();

	if (!shouldBlockShapeBeHighlighted[enum_cast(blockShape)]) {
		return false;
	}

	// Check block "above" the face, if this is a solid block we have not found a side and can return
	if (_checkIsNotSide(face, pos)) {
		return false;
	}

	// We have found a side and need to see if this side is at an edge
	std::bitset<6> facesFacingEdge;

	for (auto& sideface : Facing::ALL_EXCEPT_OPPOSITE_FACING_AND[face]) {
		BlockPos neighbourPos = pos.relative(sideface);

		const Block& sideblock = mBlockCache.getBlock(neighbourPos);
		const BlockGraphics& sideblockGraphics = mBlockCache.getBlockGraphicsPack().get(sideblock.getId());
		BlockShape sideblockShape = sideblockGraphics.getBlockShape();

		auto isFacingEdge = [&] {
			return !isBlockShapeNotEdge[enum_cast(sideblockShape)];
		};

		auto isFacingPath = [&] {
			return (
				!block.isType(Block::mGrassPathBlock) &&
				sideblock.isType(Block::mGrassPathBlock)
			);
		};

		auto isFacingLowerGround = [&] {
			if (face == Facing::UP && sideblock.isType(Block::mTopSnow)) {
				AABB neighborAABB = sideblock.getVisualShape(mBlockCache.getData(neighbourPos), neighborAABB);
				return (neighborAABB.max.y < aabb.max.y);
			}
			return false;
		};

		facesFacingEdge[sideface] = isFacingEdge() || isFacingPath() || isFacingLowerGround();
	}

	mFacesFacingEdge[face] = facesFacingEdge;
	return facesFacingEdge.any();
}

bool BlockEdgeHighlighter::_checkIsNotSide(FacingID face, const BlockPos& pos) {
	return _getNeighbourBlock(face, pos).isSolid();
}

Block BlockEdgeHighlighter::_getNeighbourBlock(FacingID face, const BlockPos& pos) {
	BlockPos neighbourPos = pos.relative(face);
	return mBlockCache.getBlock(neighbourPos);
}
