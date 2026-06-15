/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"
#include "world/Facing.h"

class BlockPos;
class Block;
class AABB;
class BlockTessellatorCache;

enum class EdgeHighlight : int {
	NONE = 0,
	CLOSE_EDGE = 1,
	FAR_EDGE = 2,
	ALL = 3,
};

class BlockEdgeHighlighter {
public:
	BlockEdgeHighlighter(
		BlockTessellatorCache& cache,
		const Block& block,
		const BlockPos& blockPos,
		const AABB& shape,
		const Facing::FacingIDList& faces
	);

	bool isFacingEdge(FacingID face);
	EdgeHighlight getHighlightType();
	bool isFaceFacingEdge(FacingID face, FacingID facing);

private:
	void _updateRenderFace(
		const Block& block,
		const BlockPos& p,
		const AABB& shape,
		FacingID face
	);

	bool _checkIsEdge(const Block& block, FacingID face, const AABB& shape, const BlockPos& pos);
	
	void _setIsFacingEdge(FacingID face, bool isEdge);

	Block _getNeighbourBlock(FacingID face, const BlockPos& pos);
	bool _checkIsNotSide(FacingID face, const BlockPos& pos);

	std::array<std::bitset<6>, 6> mFacesFacingEdge;
	std::bitset<6> mFacingIsEdge;
	BlockTessellatorCache& mBlockCache;
	const Block& mBlock;
};