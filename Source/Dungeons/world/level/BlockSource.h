/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"
#include "material/MaterialType.h"
#include "world/phys/Vec2.h"
#include "world/level/BlockPos.h"
#include "world/phys/AABB.h"

enum class EntityType : unsigned int;
class Material;
class Biome;
class Dimension;
class BlockEntity;
class Level;
class Entity;
class LevelChunk;
struct Bounds;
class Block;
class BlockPos;
class ChunkPos;
class AABB;
class Vec3;
class HitResult;
class Mob;

enum class BlockSupportType;

class BlockSource {
public:
	BlockSource();

	BlockSource(const BlockSource& source) = delete;
	BlockSource& operator=(const BlockSource& source) = delete;

	virtual ~BlockSource() {}

	Level& getLevel() const;
	const Level& getLevelConst() const;

	Dimension& getDimension() const;
	const Dimension& getDimensionConst() const;
	DimensionId getDimensionId() const;

	bool shouldFireEvents(LevelChunk& c) const;

	LevelChunk* getChunk(int x, int z);
	LevelChunk* getChunk(const ChunkPos& pos);

	LevelChunk* getWritableChunk(const ChunkPos& pos);

	LevelChunk* getChunkAt(const BlockPos& pos);
	LevelChunk* getChunkAt(int x, int y, int z);

	virtual BlockID getBlockID(int x, int y, int z);
	virtual BlockID getBlockID(const BlockPos& pos);

	virtual const Block& getBlock(const BlockPos& pos);

	virtual bool isEmptyBlock(const BlockPos& pos);

	BlockID getTopBlock(int x, int& y, int z);

	Height getAboveTopSolidBlock(const BlockPos& pos, bool includeWater = false, bool includeLeaves = false);

	virtual const Material& getMaterial(const BlockPos& pos);
	Height getHeightmap(const BlockPos& pos);
	BlockPos getHeightmapPos(const BlockPos& xzPos);
	Height getMaxHeight() const {
		return mMaxHeight;
	}

	BrightnessPair getLightColor(const BlockPos& pos, Brightness minBlockLight = Brightness::MIN);
	
	void setBorderBlock(const BlockPos& pos, bool val);
	bool hasBorderBlock(const BlockPos pos);

	float getBrightness(const BlockPos& pos);

	Brightness getRawBrightness(const BlockPos& pos, bool propagate = true);

	bool canSeeSky(const BlockPos& pos);

	virtual DataID getData(const BlockPos& p);

	virtual FullBlock getBlockAndData(const BlockPos& p);

	virtual bool setExtraData(const BlockPos& p, uint16_t value);
	virtual uint16_t getExtraData(const BlockPos& p);

	virtual bool isSolidBlockingBlock(const BlockPos& p);

	bool mayPlace(BlockID blockId, const BlockPos& pos, FacingID face, Entity* placer, bool ignoreEntitites = false, Entity* ignoreEntity = nullptr);

	bool hasBlock(const BlockPos& pos);

	bool isOwnerThread() const;

	///tells if this source can provide this area
	bool hasChunksAt(const Bounds& bounds);
	bool hasChunksAt(const BlockPos& pos, int r);

	bool containsMaterial(const AABB& box, MaterialType material);
	bool containsAny(const BlockPos& min, const BlockPos& max);
	bool containsLiquid(const AABB& box, MaterialType material);

	//setters
	virtual bool setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer = nullptr);
	virtual bool setBlockAndData(const BlockPos& pos, BlockID block, DataID data, int updateFlags, Entity* placer = nullptr);
	virtual bool setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer, Unique<BlockEntity> blockEntity);

	BlockEntity* getBlockEntity(const BlockPos& pos);

	const EntityList& getEntities(Entity* except, const AABB& bb);
	const EntityList& getEntities(EntityType entityTypeId, const AABB& bb, Entity* except = nullptr);

	bool isUnobstructedByEntities(const AABB& aabb, Entity* ignoreEntity = nullptr);

	virtual Biome& getBiome(const BlockPos& pos);
	virtual Biome* tryGetBiome(const BlockPos& pos);
	void setGrassColor(int grassColor, const BlockPos& pos, int flags = 0 );
	virtual int getGrassColor(const BlockPos& pos);

	void neighborChanged(const BlockPos& neighPos, const BlockPos& myPos) {}
	void updateNeighborsAt(const BlockPos& pos);
	void updateNeighborsAt(const BlockPos& destPos, const BlockPos& srcPos);
	void updateNeighborsAtExceptFromFacing(const BlockPos& pos, const BlockPos& neighborPos, int skipFacing);

	HitResult clip(const Vec3& A, const Vec3& b, bool liquid = false, bool solidOnly = false, int maxDistance = 200, bool useBorderBlocks = false);

	bool canProvideSupport(const BlockPos& pos, FacingID face, BlockSupportType type);

	bool isInWall(const Vec3& pos);

	void fireBlocksDirty(const BlockPos& pos0, const BlockPos& pos1);
	void fireBlockChanged(const BlockPos& pos, FullBlock block, FullBlock oldBlock, int flags, Entity* changer);
	void fireBlockEntityChanged(BlockEntity& te);
	void fireEntityChanged(Entity& entity) {}

	const BlockPos getTopRainBlockPos(const BlockPos& pos);
	bool shouldFreezeIgnoreNeighbors(const BlockPos& pos);
	bool shouldFreeze(const BlockPos& pos, bool checkNeighbors);

	bool isHumidAt(const BlockPos& pos);

	void clearCachedLastChunk();

	//LEGACY INTERFACE
	virtual const Block& getBlock(int x, int y, int z);
	Height getAboveTopSolidBlock(int x, int z, bool includeWater = false, bool includeLeaves = false);
	Height getHeightmap(int x, int z);
	const Material& getMaterial(int x, int y, int z);
	float getBrightness(int x, int y, int z);
	Brightness getRawBrightness(int x, int y, int z, bool propagate = true);
	bool canSeeSky(int x, int y, int z);
	DataID getData(int x, int y, int z);
	FullBlock getBlockAndData(int x, int y, int z);
	virtual bool isEmptyBlock(int x, int y, int z);
	virtual bool isConsideredSolidBlock(int x, int i, int z);
	virtual bool isConsideredSolidBlock(const BlockPos& pos);
	virtual bool isSolidBlockingBlock(int x, int i, int z);
	bool hasBlock(int x, int y, int z);

	bool hasChunksAt(const AABB& bb );
	bool hasChunksAt(const BlockPos& min, const BlockPos& max);
	bool hasChunksAt(int x, int y, int z, int r);

	virtual bool setBlock(int x, int y, int z, BlockID block, int updateFlags);
	virtual bool setBlock(const BlockPos& pos, BlockID block, int updateFlags);
	virtual bool setBlockNoUpdate(int x, int y, int z, BlockID block);

	virtual bool setBlockAndDataNoUpdate(int x, int y, int z, FullBlock block);
	virtual bool setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags);
	virtual bool setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags);

	bool removeBlock(int x, int y, int z);
	bool removeBlock(const BlockPos& pos);
	BlockEntity* getBlockEntity(int x, int y, int z);

	Dimension& getDimension();
	Level& getLevel();

	const BlockPos& getPlaceChunkPos() { return mPlaceChunkPos; }
	void setPlaceChunkPos(const BlockPos &pos) { mPlaceChunkPos = pos; }

	bool checkBlockDestroyPermissions(Entity& entity, const BlockPos& block, bool generateParticle = true);
private:
	const std::thread::id mOwnerThreadID;

	const bool mAllowUnpopulatedChunks, mPublicSource;
	Level* mLevel;

	Dimension* mDimension;
	const Height mMaxHeight;

protected:
	BlockPos mPlaceChunkPos;

	LevelChunk* mLastChunk = nullptr;

	EntityList mTempEntityList;
	std::vector<AABB> mTempCubeList;
	void _blockChanged(const BlockPos& pos, FullBlock block, FullBlock previousBlock, int updateFlags, Entity* changer);
	Brightness _getSkyDarken();
};
