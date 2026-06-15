/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "CommonTypes.h"
#include "world/level/chunk/DataLayer.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/ChunkPos.h"
#include "world/level/SubChunkBlockPos.h"
#include "world/level/Tick.h"
#include "world/level/ChunkBlockPos.h"
#include "world/level/chunk/ChunkState.h"
#include "world/level/chunk/LevelChunkFormat.h"
#include "world/entity/EntityTypes.h"
#include "util/SpinLock.h"
#include "legacy/Core/Utility/buffer_span.h"
#include "util/NibblePair.h"
#include "legacy/Core/Utility/static_vector.h"
#include "world/entity/Entity.h"
#include "util/SmallSet.h"

typedef SmallSet<Unique<Entity>> OwnedEntitySet;

class Level;
class Dimension;
class LightLayer;
class AABB;
class BlockSource;
class IDataOutput;
class IDataInput;
class KeyValueInput;
class Biome;

const auto CUBIC_CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_WIDTH *CHUNK_WIDTH;

const uint32_t MAX_CHUNK_HEIGHT = 256; //TODO this is only needed to preallocate subchunk slots inline... if there was another way it wouldn't be needed

struct BiomeChunkData {
	unsigned char biome;
};

struct BiomeChunkDataLegacy {
	unsigned char biome, r, g, b;
};

// Not serialized
struct ColumnCachedData {
	int grassColor;
};

//the types of the data that is serialized in the underlying Key-value storage as different keys
enum class LevelChunkDataField {
	BiomeState,
	BlockEntity,
	BlockExtraData,
	Entity,
	PendingTicks,

	_count
};

class LevelChunk {
public:

	struct DirtyTicksCounter {
		static DirtyTicksCounter max();

		DirtyTicksCounter();

		void touch();

		DirtyTicksCounter& operator++();

		int getTicksSinceLastChange() const;
		int getTotalDirtyTicks() const;

		bool isDirty() const {
			return totalTime >= 0;
		}

	protected:
		int totalTime, lastChange;
	};

	struct SubChunk {
		std::array<BlockID, CUBIC_CHUNK_VOLUME> mBlocks;
		DataLayer<CUBIC_CHUNK_VOLUME>
			mData,
			mSkyLight,
			mBlockLight;

		DirtyTicksCounter mDirtyTicksCounter;

		SubChunk(bool maxSkyLight);
		explicit SubChunk(IDataInput& stream);
		SubChunk(const SubChunk&) = delete;
		SubChunk& operator=(const SubChunk&) = delete;

		DataLayer<CUBIC_CHUNK_VOLUME>& getLayer(const LightLayer& layer);

		bool scanIsEmpty(bool hasNoSkyLight) const;
		bool hasBlocks() const;

		void serialize(IDataOutput& stream);

		static void* operator new(size_t size);
		static void operator delete (void* p);
		static bool trimMemoryPool();
	};

	typedef std::vector<BlockPos> LightList;
	typedef std::vector<BlockPos> BlockList;

	typedef std::unordered_map<ChunkBlockPos, Unique<BlockEntity> > OwnedBlockEntityMap;

	static void* operator new(size_t size);
	static void operator delete (void* p);
	static bool trimMemoryPool();

	//WARNING: SERIALIZED
	// the numeric order of the tags is important as tags are loaded in that order.
	// Tags have dependencies on each other so for example, Data2D and SubChunks need to be loaded
	// before entities.
	enum class Tag : char {
		Data2D = 45,
		Data2DLegacy,
		SubChunkPrefix,
		LegacyTerrain,
		BlockEntity,
		Entity,
		PendingTicks,
		BlockExtraData,
		BiomeState,
		FinalizedState,
		Version = 118
	};

	LevelChunk(Level& level, Dimension& dimension, const ChunkPos& cp, bool readOnly = false);
	LevelChunk(const LevelChunk&) = delete;

	~LevelChunk();

	bool isBlockInChunk(const BlockPos& block) const;

	///tells if the chunk needs to/can be saved
	bool needsSaving(int wait, int maxWait) const;
	bool isDirty() const;

	//Entities are assumed dirty
	void onLoaded(BlockSource &region);

	Height getHeightmap(const ChunkBlockPos& pos) const;

	void setBorder(const ChunkBlockPos& pos, bool val);

	void recalcHeightmap();

	Brightness getBrightness(const LightLayer& layer, const ChunkBlockPos& pos);
	bool setBrightness(const LightLayer& layer, const ChunkBlockPos& pos, Brightness brightness);
	Brightness getRawBrightness(const ChunkBlockPos& pos, Brightness skyDampen);
	void addEntity(Unique<Entity> e);
	bool hasEntity(Entity& e);

	Unique<Entity> removeEntity(Entity& e);

	LightList && moveLightSources();

	//
	// BlockExtraData
	//
	bool setBlockExtraData(const ChunkBlockPos& localPos, uint16_t extraData);
	uint16_t getBlockExtraData(const ChunkBlockPos& localPos);

	bool hasAnyBlockExtraData() const;

	void setAllBlockIDs(buffer_span<BlockID> ids, Height sourceColumnHeight);
	void setAllSkyLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight);
	void setAllBlockLight(buffer_span<NibblePair> brightness, Height sourceColumnHeight);
	void setAllBlockData(buffer_span<NibblePair> data, Height sourceColumnHeight);

	bool isSkyLit(const ChunkBlockPos& pos);

	void deferLightEmitter(const BlockPos& pos);

	void recalcBlockLights();

	const OwnedEntitySet& getEntities() const;

	void getEntities(Entity* except, const AABB& bb, std::vector<Entity*>& es);
	void getEntities(EntityType type, const AABB& bb, std::vector<Entity*>& es, bool ignoreTargetType = false) const;

	BlockID getBlock(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return BlockID::AIR;
		}
		return mSubChunks[idx]->mBlocks[SubChunkBlockPos(pos).index()];
	}

	DataID getData(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return 0;
		}
		return mSubChunks[idx]->mData.get(SubChunkBlockPos(pos).index());
	}

	FullBlock getBlockAndData(const ChunkBlockPos& pos) const {
		size_t idx = pos.y >> 4;
		if (idx >= mSubChunks.size()) {
			return FullBlock::AIR;
		}
		auto blockIdx = SubChunkBlockPos(pos).index();
		return{
			mSubChunks[idx]->mBlocks[blockIdx],
			mSubChunks[idx]->mData.get(blockIdx)
		};
	}

	FullBlock setBlockAndData(const ChunkBlockPos& pos, FullBlock block, BlockSource* issuingSource = nullptr, Unique<BlockEntity> blockEntity= nullptr);

	const std::atomic<ChunkState>& getState() const;

	bool tryChangeState(ChunkState from, ChunkState to);
	void changeState(ChunkState from, ChunkState to);

	void tick(Player* player, const Tick& tick) {}

	Biome& getBiome(const ChunkBlockPos& pos) const;

	Height getAboveTopSolidBlock(const ChunkBlockPos& pos, bool dimensionShowsSky, bool includeWater = false, bool includeLeaves = false);
	void setGrassColor(int grassColor, const ChunkBlockPos& pos);
	int getGrassColor(const ChunkBlockPos& pos);

	// applied
	void updateLightsAndHeights(BlockSource& source) {}
	const BlockPos getTopRainBlockPos(const ChunkBlockPos& pos);

	LevelChunkFormat getLoadedFormat() const;

	DimensionId getDimensionId() const;

	uint64_t key() const;

	const BlockPos& getMin() const;
	const BlockPos& getMax() const;
	const ChunkPos& getPosition() const;
	const bool isReadOnly() const;

	Height getAllocatedHeight() const {
		return mSubChunks.size() * CHUNK_WIDTH;
	}
protected:

	//statics
	static const int UPDATE_MAP_BIT_SHIFT;

	// constant properties
	Level& mLevel;
	Dimension& mDimension;

	const BlockPos mMin, mMax;
	const ChunkPos mPosition;

	const bool mReadOnly;

	//loading time stuff
	LevelChunkFormat mLoadedFormat;

	SpinLock mLightListLock;
	LightList mBlockLightSources;

	//runtime stuff
	std::atomic<ChunkState> mLoadState;
	ChunkDebugDisplaySavedState mDebugDisplaySavedState; // is this chunk saved to the DB? Used by the debug display for chunks

	Tick mLastTick;
	
	static_vector<std::unique_ptr<SubChunk>, MAX_CHUNK_HEIGHT / CHUNK_WIDTH> mSubChunks;
	const Brightness mDefaultSkyBrightness;

	BiomeChunkData mBiomes[CHUNK_WIDTH * CHUNK_WIDTH];
	ColumnCachedData mCachedData[CHUNK_WIDTH * CHUNK_WIDTH];
	Height mHeightmap[CHUNK_COLUMNS];

	typedef uint8_t BBorder;
	BBorder mBorderBlockMap[CHUNK_COLUMNS];
	bool mIsLoaded = false;

	DirtyTicksCounter mFullChunkDirtyTicksCounters[enum_cast(LevelChunkDataField::_count)];
	static_vector<DirtyTicksCounter*, enum_cast(LevelChunkDataField::_count) + (MAX_CHUNK_HEIGHT / CHUNK_WIDTH)> mDirtyTickCounters;

	Height mRainHeights[CHUNK_WIDTH * CHUNK_WIDTH];

	OwnedEntitySet mEntities;
	std::unordered_map<int, uint16_t> mBlockExtraData;

	void _lightGaps(BlockSource& source, const ChunkBlockPos& pos);

	void _recalcHeight(const ChunkBlockPos& startPos, BlockSource* source = nullptr) {}

	void _createSubChunk(size_t idx, bool initSkyLight);
	
	//TODO //OPTIMIZE remove this and set the light directly on the SubChunk. The lighting algorithms should be subchunk-aware
	void _setLight(const ChunkBlockPos& pos, Brightness br);

	DataLayer<CUBIC_CHUNK_VOLUME>* _lightLayer(const LightLayer& layer, const ChunkBlockPos& pos);

	DirtyTicksCounter& _dirtyTicksCounter(LevelChunkDataField counter) {
		return *mDirtyTickCounters[enum_cast(counter)];
	}
};
