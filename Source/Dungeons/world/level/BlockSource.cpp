/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/BlockSource.h"

#include "world/entity/Entity.h"
#include "world/entity/EntityClassTree.h"
#include "world/entity/player/Player.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/Block.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/block/FireBlock.h"
#include "world/level/block/LiquidBlock.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/ChunkPos.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/phys/Vec3.h"
#include "util/Bounds.h"
#include "dimension/Dimension.h"

#ifndef PUBLISH
#define DEBUG_ASSERT_OWNER_THREAD DEBUG_ASSERT(isOwnerThread(), "ERROR, this is running on the wrong thread");
#else
#define DEBUG_ASSERT_OWNER_THREAD {}
#endif

BlockSource::BlockSource()
	: mAllowUnpopulatedChunks(false)
	, mMaxHeight(LEVEL_HEIGHT_DEPRECATED)
	, mPublicSource(false)
	, mLevel(nullptr)
	, mDimension(nullptr) {
	// Mock
}

Level& BlockSource::getLevel() const {
	DEBUG_ASSERT_MAIN_THREAD;	//if someone gets the level on another thread he's up to no good
	DEBUG_ASSERT(mPublicSource, "Cannot get the level on a temporary tileSource");
	return *mLevel;
}

Level& BlockSource::getLevel() {
	return *mLevel;
}

//also checks the given blockPos permission since we're about to destroy that block
bool BlockSource::checkBlockDestroyPermissions(Entity& entity, const BlockPos& block, bool generateParticle) {
	return false;
}

const Level& BlockSource::getLevelConst() const {
	//this one can actually be called from other threads
	return *mLevel;
}

Dimension& BlockSource::getDimension() const {
	DEBUG_ASSERT_MAIN_THREAD;	//if someone gets the dimensino on another thread he's up to no good
	DEBUG_ASSERT(mPublicSource, "Cannot get the dimension on a temporary tileSource");
	return *mDimension;
}

Dimension& BlockSource::getDimension() {
	return *mDimension;
}

DimensionId BlockSource::getDimensionId() const {
	return mDimension->getId();
}

const Dimension& BlockSource::getDimensionConst() const {
	//this one can actually be called from other threads
	return *mDimension;
}

LevelChunk* BlockSource::getChunk(int x, int z) {
	return getChunk(ChunkPos(x, z));
}

LevelChunk* BlockSource::getChunk(const ChunkPos& pos){
	return nullptr;
}

LevelChunk* BlockSource::getWritableChunk(const ChunkPos& pos){
	auto lc = getChunk(pos);
	return (!lc || lc->isReadOnly()) ? nullptr : lc;

}

bool BlockSource::shouldFireEvents(LevelChunk& c) const {
	return !mAllowUnpopulatedChunks && c.getState() == ChunkState::Loaded;
}

bool BlockSource::hasBlock(const BlockPos& pos) {
	LevelChunk* lc = getChunkAt(pos);
	// Returns false for chunks outside in limited worlds
	return lc != nullptr && lc->getPosition() != ChunkPos::INVALID && !lc->isReadOnly();
}

bool BlockSource::hasChunksAt(const Bounds& bounds) {

	auto itr = bounds.begin(), end = bounds.end();

	//check if all the chunks are existing & loaded
	for (; itr != end; ++itr) {
		if (!getChunk( ChunkPos(itr.x, itr.z))) {
			return false;
		}
	}
	return true;
}

bool BlockSource::hasChunksAt(const AABB& bb) {
	//note: we squash all the chunks bounds, it makes no sense to request a span over y
	return hasChunksAt(Bounds(bb.min, bb.max, CHUNK_WIDTH));
}

bool BlockSource::hasChunksAt(const BlockPos& pos, int r){
	return hasChunksAt(pos.x, pos.y, pos.z, r);
}

bool BlockSource::hasChunksAt(int x, int y, int z, int r) {
	return hasChunksAt(Bounds(BlockPos(x - r, y, z - r), BlockPos(x + r, y, z + r), CHUNK_WIDTH));
}

bool BlockSource::hasChunksAt(const BlockPos& min, const BlockPos& max) {
	return hasChunksAt(Bounds(min, max, CHUNK_WIDTH));
}

BlockID BlockSource::getBlockID(const BlockPos& pos){
	if (pos.y >= 0 && pos.y < mMaxHeight) {
		LevelChunk* c = getChunk( pos );
		return c ? c->getBlock((ChunkBlockPos)pos ) : BlockID::AIR;
	}
	return BlockID::AIR;
}

const Block& BlockSource::getBlock(const BlockPos& pos){
	return *Block::mBlocks[getBlockID(pos)];
}

DataID BlockSource::getData(const BlockPos& p){
	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		return c ? c->getData((ChunkBlockPos)p) : 0;
	}
	return 0;
}

FullBlock BlockSource::getBlockAndData(const BlockPos& p) {

	if (p.y >= 0 && p.y < mMaxHeight) {
		if (LevelChunk* c = getChunk(p)) {
			return c->getBlockAndData((ChunkBlockPos)p);
		}
	}
	return FullBlock();
}

bool BlockSource::setExtraData(const BlockPos& p, uint16_t extraData) {
	if (p.y >= 0 && p.y < mMaxHeight) {
		if (LevelChunk* c = getChunk(p)) {
			c->setBlockExtraData((ChunkBlockPos)p, extraData);
			return true;
		}
	}
	return false;
}

uint16_t BlockSource::getExtraData(const BlockPos& p){
	if (p.y >= 0 && p.y < mMaxHeight) {
		LevelChunk* c = getChunk(p);
		if (c) {
			return c->getBlockExtraData((ChunkBlockPos)p);
		}
	}
	return 0;
}

void BlockSource::setBorderBlock(const BlockPos& pos, bool val) {
	if (LevelChunk* c = getChunkAt(pos)) {
		c->setBorder(ChunkBlockPos(pos), val);
	}
}

bool BlockSource::hasBorderBlock(const BlockPos pos) {
	return false;
}

BrightnessPair BlockSource::getLightColor(const BlockPos& pos, Brightness minBlockLight) {
	return BrightnessPair();
}

float BlockSource::getBrightness(const BlockPos& pos){
	return mDimension->getBrightnessRamp()[getRawBrightness(pos)];
}

Brightness BlockSource::getRawBrightness(const BlockPos& pos, bool propagate){
	auto id = getBlockID(pos);
	if (Block::mLightBlock[id] == Brightness::MAX) {
		return Brightness::MIN;	//no need to follow through with everything!

	}
	//@bounds-check
	if (propagate) {
		if (id == Block::mStoneSlab->mID || id == Block::mFarmland->mID || id == Block::mWoodenSlab->mID) {
			Brightness br = getRawBrightness(pos.above(), false);
			br = std::max(br, getRawBrightness(pos.west(), false));
			br = std::max(br, getRawBrightness(pos.east(), false));
			br = std::max(br, getRawBrightness(pos.south(), false));
			br = std::max(br, getRawBrightness(pos.north(), false));
			return br;
		}
	}

	if (pos.y < 0) {
		return Brightness::MIN;
	}
	else if (pos.y >= mMaxHeight) {
		auto br = Brightness::MAX;
		br -= _getSkyDarken();	//TODO is this evil?
		return br > Brightness::MAX ? Brightness::MIN : br;
	}

	LevelChunk* c = getChunk(pos);
	return c ? c->getRawBrightness((ChunkBlockPos)pos, _getSkyDarken()) : Brightness::MAX;
}

Height BlockSource::getAboveTopSolidBlock(const BlockPos& pos, bool includeWater /* = false */, bool includeLeaves /* = false */) {
	LevelChunk* levelChunk = getChunkAt(pos);

	return levelChunk ? levelChunk->getAboveTopSolidBlock((ChunkBlockPos)pos, mDimension->showSky(), includeWater, includeLeaves) : getDimension().getHeight();
}

bool BlockSource::canSeeSky(const BlockPos& pos){
	if (pos.y >= mMaxHeight) {
		return true;
	}
	else if (pos.y < 0) {
		return false;
	}
	else {
		auto lc = getChunk(pos);
		return lc ? lc->isSkyLit((ChunkBlockPos)pos) : true;
	}
}

const Material& BlockSource::getMaterial(const BlockPos& pos) {
	return getBlock(pos).getMaterial();
}

Height BlockSource::getHeightmap(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);

	return c ? c->getHeightmap(ChunkBlockPos(pos)) : 0;
}

BlockPos BlockSource::getHeightmapPos(const BlockPos& xzPos) {
	return BlockPos(xzPos.x, getHeightmap(xzPos), xzPos.z);
}

Brightness BlockSource::_getSkyDarken() {
	return mPublicSource ? mDimension->getSkyDarken() : Brightness::MIN;
}

bool BlockSource::isSolidBlockingBlock(int x, int y, int z){
	auto& block = getBlock({x, y, z});
	return block.getMaterial().isSolidBlocking() && (block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking));
}

bool BlockSource::isSolidBlockingBlock(const BlockPos& p){
	return isSolidBlockingBlock(p.x, p.y, p.z);
}

bool BlockSource::isConsideredSolidBlock(int x, int y, int z) {
	return getBlock({ x, y, z }).isSolid();
}

bool BlockSource::isConsideredSolidBlock(const BlockPos& pos) {
	return isConsideredSolidBlock(pos.x, pos.y, pos.z);
}

bool BlockSource::mayPlace(BlockID blockId, const BlockPos& pos, FacingID face, Entity* placer, bool ignoreEntitites /*= false*/, Entity* ignoreEntity /*= nullptr*/){
	const auto& targetBlock = getBlock(pos);
	const Block* block = Block::mBlocks[blockId];

	AABB tmpAABB;
	DataID placementData = placer != nullptr ? block->getPlacementDataValue(*placer, pos, face, Vec3::ZERO, 0) : 0;
	auto& aabb = ignoreEntitites ? AABB::EMPTY : block->getAABB(*this, pos, tmpAABB, placementData);

	if (pos.y < 0 || pos.y >= mMaxHeight) {
		return false;
	}

	if (!aabb.isEmpty() && !isUnobstructedByEntities(aabb, ignoreEntity)) {
		return false;
	}

	bool canBeBuiltOver = targetBlock.canBeBuiltOver(*this, pos);

	bool topSnowPlaceable = targetBlock.hasProperty(BlockProperty::Snow | BlockProperty::TopSnow) || 
		(block->mID == Block::mTopSnow->mID && TopSnowBlock::checkIsRecoverableBlock(targetBlock.mID));

	if (blockId > BlockID::AIR && 
		(canBeBuiltOver || topSnowPlaceable)) {
		if (block->mayPlace(*this, pos, face)) {
			return true;
		}
	}

	return false;
}

bool BlockSource::setBlock(int x, int y, int z, BlockID block, int updateFlags) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(block, 0), updateFlags);
}

bool BlockSource::setBlock(const BlockPos& pos, BlockID block, int updateFlags) {
	return setBlockAndData(pos, FullBlock(block, 0), updateFlags);
}

void BlockSource::updateNeighborsAt(const BlockPos& pos){
	neighborChanged(pos.west(), pos);
	neighborChanged(pos.east(), pos);
	neighborChanged(pos.south(), pos);
	neighborChanged(pos.north(), pos);
	neighborChanged(pos.above(), pos);
	neighborChanged(pos.below(), pos);
}

void BlockSource::updateNeighborsAt(const BlockPos& destPos, const BlockPos& srcPos) {
	neighborChanged(destPos.west(), srcPos);
	neighborChanged(destPos.east(), srcPos);
	neighborChanged(destPos.south(), srcPos);
	neighborChanged(destPos.north(), srcPos);
	neighborChanged(destPos.above(), srcPos);
	neighborChanged(destPos.below(), srcPos);
}

void BlockSource::updateNeighborsAtExceptFromFacing(const BlockPos& pos, const BlockPos& neighborPos, int skipFacing) {
	if (skipFacing != Facing::WEST) {
		neighborChanged(pos.west(), neighborPos);
	}
	if (skipFacing != Facing::EAST) {
		neighborChanged(pos.east(), neighborPos);
	}
	if (skipFacing != Facing::DOWN) {
		neighborChanged(pos.below(), neighborPos);
	}
	if (skipFacing != Facing::UP) {
		neighborChanged(pos.above(), neighborPos);
	}
	if (skipFacing != Facing::NORTH) {
		neighborChanged(pos.north(), neighborPos);
	}
	if (skipFacing != Facing::SOUTH) {
		neighborChanged(pos.south(), neighborPos);
	}
}


void BlockSource::_blockChanged(const BlockPos& pos, FullBlock block, FullBlock previousBlock, int updateFlags, Entity* changer) {
	if (updateFlags & Block::UPDATE_NEIGHBORS) {
		updateNeighborsAt(pos);
	}

	//let's not call events over sources used for loading
	if ((updateFlags& Block::UPDATE_CLIENTS) != 0 && !(getLevelConst().isClientSide() && (updateFlags& Block::UPDATE_INVISIBLE) != 0)) {
		fireBlockChanged(pos, block, previousBlock, updateFlags, changer);
	}
}

bool BlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer, Unique<BlockEntity> blockEntity) {
	DEBUG_ASSERT(block.data >= 0 && block.data <= 0xf, "Invalid data");

	if (pos.y < 0 || pos.y >= mMaxHeight) {
		return false;
	}

	LevelChunk* levelChunk = getWritableChunk(pos);
	if (!levelChunk) {
		return false;
	}

	bool fireEvent = shouldFireEvents(*levelChunk);
	auto previousBlock = levelChunk->setBlockAndData((ChunkBlockPos)pos, block, fireEvent ? this : nullptr, std::move(blockEntity));

	BlockID oldBlockId = block.id;
	// When setBlockAndData is called some blocks actually change multiple times. (es: rails) (or SnowGolem blocks turning
	// into AIR)
	// Because of this we need to call getBlockAndData() to capture the latest block data.
	block = levelChunk->getBlockAndData((ChunkBlockPos)pos);

	const bool wasGolemSpawned = (oldBlockId == Block::mPumpkin->mID || oldBlockId == Block::mLitPumpkin->mID) && (block == BlockID::AIR);

	if ((previousBlock != block && fireEvent) || (updateFlags& Block::UPDATE_ITEM_DATA) != 0) {
		_blockChanged(pos, block, previousBlock, updateFlags, placer);
	}

	return (previousBlock.id != block.id) || (previousBlock.data != block.data) || wasGolemSpawned;

}

bool BlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer) {
	return setBlockAndData(pos, block, updateFlags, placer, nullptr);
}

bool BlockSource::setBlockAndData(const BlockPos& pos, BlockID block, DataID data, int updateFlags, Entity* placer /* = nullptr */) {
	return setBlockAndData(pos, { block, data }, updateFlags, placer, nullptr);
}

bool BlockSource::setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags) {
	return setBlockAndData(x, y, z, FullBlock(id, data), updateFlags);
}

bool BlockSource::setBlockNoUpdate(int x, int y, int z, BlockID block) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(block, 0), Block::UPDATE_NONE);
}

bool BlockSource::setBlockAndDataNoUpdate(int x, int y, int z, FullBlock block) {
	return setBlockAndData(BlockPos(x, y, z), block, Block::UPDATE_NONE);
}

bool BlockSource::removeBlock(int x, int y, int z) {
	return setBlockAndData(BlockPos(x, y, z), FullBlock(), Block::UPDATE_ALL);
}

bool BlockSource::removeBlock(const BlockPos& pos) {
	return setBlockAndData(pos, FullBlock(), Block::UPDATE_ALL);
}

 

BlockEntity* BlockSource::getBlockEntity(const BlockPos& pos ) {
	return nullptr;
}

const EntityList& BlockSource::getEntities(Entity* except, const AABB& bb) {
	mTempEntityList.clear();
	return mTempEntityList;
}

const EntityList& BlockSource::getEntities(EntityType entityTypeId, const AABB& bb, Entity* except){
	mTempEntityList.clear();
	return mTempEntityList;
}

bool BlockSource::isUnobstructedByEntities(const AABB& aabb, Entity* ignoreEntity) {
	auto& entities = getEntities(ignoreEntity, aabb);

	for (unsigned int i = 0; i < entities.size(); i++) {
		Entity* e = entities[i];
		if (!e->isRemoved() && e->mBlocksBuilding) {
			return false;
		}
	}
	return true;
}

// Need to make it low enough players wont fall and hit it, displaying the wrong death message
bool BlockSource::containsAny(const BlockPos& min, const BlockPos& max) {
	BlockPos p = min;

	for (; p.x < max.x; ++p.x) {
		for (p.z = min.z; p.z < max.z; ++p.z) {
			for (p.y = min.y; p.y < max.y; ++p.y) {
				if (getBlockID(p) != BlockID::AIR) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsMaterial(const AABB& box, MaterialType material) {
	int x0 = Math::floor(box.min.x);
	int x1 = Math::floor(box.max.x + 1);
	int y0 = Math::floor(box.min.y);
	int y1 = Math::floor(box.max.y + 1);
	int z0 = Math::floor(box.min.z);
	int z1 = Math::floor(box.max.z + 1);

	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			for (int z = z0; z < z1; z++) {
				const Block& block = getBlock(BlockPos(x, y, z));
				if (block.getMaterial().isType(material)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool BlockSource::containsLiquid(const AABB& box, MaterialType material) {
	int x0 = Math::floor(box.min.x);
	int x1 = Math::floor(box.max.x + 1);
	int y0 = Math::floor(box.min.y);
	int y1 = Math::floor(box.max.y + 1);
	int z0 = Math::floor(box.min.z);
	int z1 = Math::floor(box.max.z + 1);

	for (int x = x0; x < x1; x++) {
		for (int y = y0; y < y1; y++) {
			for (int z = z0; z < z1; z++) {
				const Block& block = getBlock(BlockPos(x, y, z));
				if (block.getMaterial().isType(material)) {
					int data = getData(x, y, z);
					float yh1 = (float)(y + 1);
					if (data < 8) {
						yh1 = (float)y + 1.0f - (float)data / 8.0f;
					}
					if (yh1 >= box.min.y) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

HitResult BlockSource::clip(const Vec3& A, const Vec3& B, bool liquid /*= false*/, bool solidOnly /*= false*/, int maxDistance /*= 200*/, bool useBorderBlocks /*= false*/) {

	if (A.isNan()) {
		return HitResult(B);
	}
	if (B.isNan()) {
		return HitResult(B);
	}

	Vec3 a(A);
	Vec3 b(B);

	int xBlock1 = Math::floor(b.x);
	int yBlock1 = Math::floor(b.y);
	int zBlock1 = Math::floor(b.z);

	int xBlock0 = Math::floor(a.x);
	int yBlock0 = Math::floor(a.y);
	int zBlock0 = Math::floor(a.z);

	HitResult liquidHit;
	bool hitLiquid = false;

	// The java version does an initial check on the position you start from,
	// could potentially solve the issue where you can remove blocks through ladders
	const Material* startedInLiquidMaterial = nullptr;
	{
		FullBlock block = getBlockAndData(xBlock0, yBlock0, zBlock0);
		const Block* t = Block::mBlocks[block.id];
		AABB bufferAABB;
		if (t != nullptr)
		{
			// Did we start off inside liquid?
			startedInLiquidMaterial = t->getMaterial().isLiquid() ? &t->getMaterial() : nullptr;
			if (startedInLiquidMaterial != nullptr) {
				// don't hit blocks that have the same liquid material when starting off inside liquid.
				liquid = false;
				hitLiquid = true;
				liquidHit = t->clip(*this, BlockPos(xBlock0, yBlock0, zBlock0), a, b);
			} else {
				if (solidOnly && t->getAABB(*this, BlockPos(xBlock0, yBlock0, zBlock0), bufferAABB).isEmpty()) {
					// No collision
				}
				else if (t->mayPick(*this, block.data, liquid)) {
					HitResult r = t->clip(*this, BlockPos(xBlock0, yBlock0, zBlock0), a, b);
					r.setIsHitLiquid(hitLiquid, liquidHit);
					if (r.isHit()) {
						return r;
					}
				}
			}

		}
	}

	HitResult ret = HitResult(B);

	for (auto i : range(0, maxDistance)) {
		UNUSED_VARIABLE(i);
		if (a.isNan()) {
			ret = HitResult(b);
			break;
		}
		if (xBlock0 == xBlock1 && yBlock0 == yBlock1 && zBlock0 == zBlock1) {
			ret = HitResult(b);
			break;
		}

		float xClip = 0.f;
		float yClip = 0.f;
		float zClip = 0.f;

		bool hasXClip = false;
		bool hasYClip = false;
		bool hasZClip = false;

		// Set clip to be the far boundary of the block we're in relative to the ray direction
		// If the ray is pointing right, put xclip on right boundary
		if (xBlock1 > xBlock0) {
			hasXClip = true;
			xClip = xBlock0 + 1.000f;
		}
		// If the ray is pointing left, put xclip on the left boundary, etc. for y and z
		if (xBlock1 < xBlock0) {
			hasXClip = true;
			xClip = xBlock0 + 0.000f;
		}

		if (yBlock1 > yBlock0) {
			hasYClip = true;
			yClip = yBlock0 + 1.000f;
		}
		if (yBlock1 < yBlock0) {
			hasYClip = true;
			yClip = yBlock0 + 0.000f;
		}

		if (zBlock1 > zBlock0) {
			hasZClip = true;
			zClip = zBlock0 + 1.000f;
		}
		if (zBlock1 < zBlock0) {
			hasZClip = true;
			zClip = zBlock0 + 0.000f;
		}

		float xDist = 999;
		float yDist = 999;
		float zDist = 999;

		float xd = b.x - a.x;
		float yd = b.y - a.y;
		float zd = b.z - a.z;

		// Determine time of intersection of the ray out of this block for each axis
		if (hasXClip) {
			xDist = (xClip - a.x) / xd;
		}
		if (hasYClip) {
			yDist = (yClip - a.y) / yd;
		}
		if (hasZClip) {
			zDist = (zClip - a.z) / zd;
		}

		// Clip to the closest boundary axis of this block
		if (xDist < yDist && xDist < zDist) {
			// March block pos forward into the block we're clipping the ray on. This is more reliable then flooring clip result for when multiple axes of a lie on a boundary
			// It is safe to make a change by one because we are ensuring stepping forward one block by clipping to a block boundary which is unit size and choosing the earliest intersection
			// Same idea in different directions for each branch here
			if (xBlock1 > xBlock0) {
				++xBlock0;
			}
			else{
				--xBlock0;
			}

			// Clip a to the block boundary. While it may seem redundant to have a clip in each branch instead of one clip with xd,yd,zd, explicitly setting xclip instead of xd*xDist helps mitigate precision issues
			a.x = xClip;
			a.y += yd * xDist;
			a.z += zd * xDist;
		}
		else if (yDist < zDist) {
			if (yBlock1 > yBlock0) {
				++yBlock0;
			}
			else{
				--yBlock0;
			}

			a.x += xd * yDist;
			a.y = yClip;
			a.z += zd * yDist;
		}
		else {
			if (zBlock1 > zBlock0) {
				++zBlock0;
			}
			else{
				--zBlock0;
			}

			a.x += xd * zDist;
			a.y += yd * zDist;
			a.z = zClip;
		}

		FullBlock t = getBlockAndData(xBlock0, yBlock0, zBlock0);
		const Block* block = Block::mBlocks[t.id];

		AABB aaBB;
		//border blocks are infinite so the y BlockPos in our check is unwanted
		//Currently the system doesn't support an idea of an infinite block/collision and We've seen this as the least intrusive way to do so.
		bool hasborderBlock = !useBorderBlocks && hasBorderBlock(BlockPos(xBlock0, 0, zBlock0));
		BlockPos blockPos(xBlock0, yBlock0, zBlock0);

		//if border block. worldbuilders can get through
		if (hasborderBlock) {
			block = Block::mBorder;
			block->getAABB(*this, blockPos, aaBB);
			aaBB.min.y = 0;
			aaBB.max.y = mMaxHeight;
		}

		if(block) {

			// Toggle on hitting liquid blocks if we left the type of liquid we started in.
			if (startedInLiquidMaterial && !liquid && (&block->getMaterial() != startedInLiquidMaterial))
				liquid = true;

			if(solidOnly && (!hasborderBlock && block->getAABB(*this, blockPos, aaBB).isEmpty())) {
				// No collision
			}
			else if(block->mayPick(*this, t.data, liquid)) {
				if (liquid && block->getMaterial().isLiquid()) {
					liquid = false;
					hitLiquid = true;
					liquidHit = hasborderBlock ? block->clip(*this, blockPos, a, b, false, 0, aaBB) : block->clip(*this, blockPos, a, b);
					//continue;
				}

				// pass origin A because AABB for block might be out side of target block
				HitResult hit = hasborderBlock ? block->clip(*this, blockPos, A, b, false, t.data, aaBB) : block->clip(*this, blockPos, A, b, false, t.data);
				hit.setIsHitLiquid(hitLiquid, liquidHit);
				if(hit.isHit()) {
					return hit;
				}
			}
		}
	}

	ret.setIsHitLiquid(hitLiquid, liquidHit);
	return ret;
}

bool BlockSource::canProvideSupport(const BlockPos& pos, FacingID face, BlockSupportType type) {
	return getBlock(pos).canProvideSupport(*this, pos, face, type);
}

bool BlockSource::isInWall(const Vec3& pos) {
	BlockPos blockPos(pos);
	return isSolidBlockingBlock(blockPos) && isConsideredSolidBlock(blockPos);
}

void BlockSource::clearCachedLastChunk() {
	mLastChunk = nullptr;
}

void BlockSource::fireBlocksDirty(const BlockPos& pos0, const BlockPos& pos1) {
	DEBUG_ASSERT_MAIN_THREAD;
}

void BlockSource::fireBlockChanged(const BlockPos& pos, FullBlock block, FullBlock oldBlock, int flags, Entity* changer){
	DEBUG_ASSERT_MAIN_THREAD;
}

void BlockSource::fireBlockEntityChanged(BlockEntity& te) {
	DEBUG_ASSERT_MAIN_THREAD;
}

bool BlockSource::isEmptyBlock(const BlockPos& pos) {
	return getBlockID(pos) == 0;
}

bool BlockSource::isEmptyBlock(int x, int y, int z){
	return isEmptyBlock(BlockPos(x, y, z));
}

BlockID BlockSource::getTopBlock(int x, int& y, int z) {
	y = 63;

	while (!isEmptyBlock(x, y + 1, z)) {
		y++;
	}
	return getBlockID(x, y, z);
}

LevelChunk* BlockSource::getChunkAt(const BlockPos& pos) {
	return getChunk(ChunkPos(pos));
}

LevelChunk* BlockSource::getChunkAt(int x, int y, int z){
	return getChunk(BlockPos(x, y, z));
}

Biome* BlockSource::tryGetBiome(const BlockPos& pos) {
	return Biome::ocean.get();
}

Biome& BlockSource::getBiome(const BlockPos& pos){
	Biome* found = tryGetBiome(pos);
	return found ? *found : *Biome::ocean; // default biome
}

void BlockSource::setGrassColor(int grassColor, const BlockPos& pos, int flags ) {
	LevelChunk* c = getWritableChunk(pos);
	if(c != nullptr) {
		c->setGrassColor(grassColor, ChunkBlockPos(pos));

		if (shouldFireEvents(*c)) {
			auto block = getBlockAndData(pos);
			_blockChanged(pos, block, block.id, flags, nullptr);
		}
	}
}

int BlockSource::getGrassColor(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);

	return c ? c->getGrassColor(ChunkBlockPos(pos)) : 0;
}

const BlockPos BlockSource::getTopRainBlockPos(const BlockPos& pos) {
	LevelChunk* c = getChunkAt(pos);
	return c ? c->getTopRainBlockPos(ChunkBlockPos(pos)) : BlockPos(-1, -1, -1);
}

bool BlockSource::shouldFreezeIgnoreNeighbors(const BlockPos& pos) {
	return shouldFreeze(pos, false);
}

bool BlockSource::shouldFreeze(const BlockPos& pos, bool checkNeighbors) {
	return false;
}

bool BlockSource::isHumidAt(const BlockPos& pos){
	return getBiome(pos).isHumid();
}

bool BlockSource::hasBlock(int x, int y, int z) {
	return hasBlock(BlockPos(x, y, z));
}

BlockID BlockSource::getBlockID(int x, int y, int z){
	return getBlockID(BlockPos(x, y, z));
}

DataID BlockSource::getData(int x, int y, int z){
	return getData(BlockPos(x, y, z));
}

FullBlock BlockSource::getBlockAndData(int x, int y, int z){
	return getBlockAndData(BlockPos(x, y, z));
}

float BlockSource::getBrightness(int x, int y, int z){
	return getBrightness(BlockPos(x, y, z));
}

Brightness BlockSource::getRawBrightness(int x, int y, int z, bool propagate){
	return getRawBrightness(BlockPos(x, y, z), propagate);
}

Height BlockSource::getAboveTopSolidBlock(int x, int z, bool includeWater, bool includeLeaves) {
	return getAboveTopSolidBlock(BlockPos(x, mMaxHeight - 1, z), includeWater, includeLeaves);
}

bool BlockSource::canSeeSky(int x, int y, int z){
	return canSeeSky(BlockPos(x, y, z));
}

Height BlockSource::getHeightmap(int x, int z){
	return getHeightmap(BlockPos(x, 0, z));
}

bool BlockSource::setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags){
	return setBlockAndData(BlockPos(x, y, z), block, updateFlags);
}

BlockEntity* BlockSource::getBlockEntity(int x, int y, int z){
	return getBlockEntity(BlockPos(x, y, z));
}

const Block& BlockSource::getBlock(int x, int y, int z) {
	return getBlock(BlockPos(x, y, z));
}

const Material& BlockSource::getMaterial(int x, int y, int z) {
	return getMaterial(BlockPos(x, y, z));
}

bool BlockSource::isOwnerThread() const {
	return std::this_thread::get_id() == mOwnerThreadID;
}
