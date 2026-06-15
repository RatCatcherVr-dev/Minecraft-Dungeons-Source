/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/chunk/LevelChunk.h"
#include "world/level/LightLayer.h"
#include "world/level/block/Block.h"
#include "world/level/Level.h"
#include "world/phys/AABB.h"
#include "world/entity/Entity.h"
#include "world/level/BlockPos.h"
#include "world/level/block/EntityBlock.h"
#include "world/level/LevelConstants.h"
#include "world/Facing.h"
#include "util/PoolAllocator.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/FoliageColor.h"

//TODO move this to another serializer/deserializer class?
#include "util/StringByteInput.h"
#include "util/KeyValueInput.h"

#include "world/entity/player/Player.h"
#include "world/level/material/Material.h"
#include "world/entity/EntityClassTree.h"
#include "util/StringUtils.h"

//the weird value of this comes from the fact that the original value was 20 for a height of 128
//this just divides it by the original number of subchunks
const float RANDOM_TICKS_PER_TICK_PER_SUBCHUNK = 20.f / (128.f / 16.f); 

static PoolAllocator levelChunkPool(sizeof(LevelChunk));
static PoolAllocator levelSubChunkPool(sizeof(LevelChunk::SubChunk));

const int LevelChunk::UPDATE_MAP_BIT_SHIFT = 4;

void* LevelChunk::operator new(size_t size) {
	DEBUG_ASSERT_MAIN_THREAD;
	DEBUG_ASSERT(size == levelChunkPool.BLOCK_SIZE, "Wrong allocation size!");
	UNUSED_PARAMETER(size);
	return levelChunkPool.get();
}

void LevelChunk::operator delete(void* p) {
	levelChunkPool.release(p);
}

bool LevelChunk::trimMemoryPool() {   
	return levelChunkPool.trim();
}

void* LevelChunk::SubChunk::operator new(size_t size) {
	DEBUG_ASSERT(size == levelSubChunkPool.BLOCK_SIZE, "Wrong allocation size!");
	UNUSED_PARAMETER(size);
	return levelSubChunkPool.get();
}

void LevelChunk::SubChunk::operator delete(void* p) {
	levelSubChunkPool.release(p);
}

bool LevelChunk::SubChunk::trimMemoryPool() {
	return levelSubChunkPool.trim();
}

LevelChunk::SubChunk::SubChunk(bool maxSkyLight) {
	memset(mBlocks.data(), 0, sizeof(mBlocks));
	memset(mData.data(), 0, sizeof(mData));
	memset(mSkyLight.data(), maxSkyLight ? INT_MAX : 0, sizeof(mSkyLight));
	memset(mBlockLight.data(), 0, sizeof(mBlockLight));
}

LevelChunk::SubChunk::SubChunk(IDataInput& stream) {
	auto format = static_cast<SubLevelChunkFormat>(stream.readByte());
	UNUSED_VARIABLE(format); //not used yet

	stream.readBytes(mBlocks.data(), sizeof(mBlocks));
	stream.readBytes(mData.data(), sizeof(mData));
	stream.readBytes(mSkyLight.data(), sizeof(mSkyLight));
	stream.readBytes(mBlockLight.data(), sizeof(mBlockLight));
}

void LevelChunk::SubChunk::serialize(IDataOutput& stream) {
	stream.writeByte(enum_cast(SharedConstants::CurrentSubLevelChunkFormat));

	//write all the data as is
	stream.writeBytes(mBlocks.data(), sizeof(mBlocks));
	stream.writeBytes(mData.data(), sizeof(mData));
	stream.writeBytes(mSkyLight.data(), sizeof(mSkyLight));
	stream.writeBytes(mBlockLight.data(), sizeof(mBlockLight));
}

//template glue because VS doesn't seem to be able to convert automatically
template<size_t S>
bool isAllOne(const DataLayer<S>& buf) {
	return Util::isAllOne(buf.span());
}
template<size_t S>
bool isAllZero(const DataLayer<S>& buf) {
	return Util::isAllZero(buf.span());
}
template<typename T, size_t S>
bool isAllZero(const std::array<T, S>& buf) {
	return Util::isAllZero(buffer_span<T>(buf));
}

bool LevelChunk::SubChunk::hasBlocks() const {
	return !isAllZero(mBlocks);
}

bool LevelChunk::SubChunk::scanIsEmpty(bool hasNoSkyLight) const {
	return 
		(hasNoSkyLight ? isAllZero(mSkyLight) : isAllOne(mSkyLight)) && 
		isAllZero(mBlockLight) && 
		!hasBlocks() && 
		isAllZero(mData);
}

DataLayer<CUBIC_CHUNK_VOLUME>& LevelChunk::SubChunk::getLayer(const LightLayer& layer) {
	if (layer.isSky()) {
		return mSkyLight;
	}
	DEBUG_ASSERT(layer == LightLayer::BLOCK, "Invalid layer");
	
	return mBlockLight;
}

LevelChunk::LevelChunk(Level& level, Dimension& dimension, const ChunkPos& cp, bool readOnly)
	: mLevel(level)
	, mDimension(dimension)
	, mPosition(cp)
	, mMin(cp)
	, mMax(BlockPos(cp) + BlockPos(CHUNK_WIDTH - 1, dimension.getHeight() - 1, CHUNK_DEPTH - 1))
	, mLoadState(ChunkState::Unloaded)
	, mDebugDisplaySavedState(ChunkDebugDisplaySavedState::Generated) // generated until overridden
	, mReadOnly(readOnly)
	, mLoadedFormat(SharedConstants::CurrentLevelChunkFormat)
	, mDefaultSkyBrightness(dimension.getSkyLight()) {
	memset(mHeightmap, 0, sizeof(mHeightmap));
	memset(mBiomes, 0, sizeof(mBiomes));
	memset(mBorderBlockMap, 0, sizeof(mBorderBlockMap));

	for (auto i : range(CHUNK_WIDTH * CHUNK_WIDTH)) {
		mRainHeights[i] = -999;
	}

	//register all dirty tick counters
	for (auto&& counter : mFullChunkDirtyTicksCounters) {
		mDirtyTickCounters.push_back(&counter);
	}
}

LevelChunk::~LevelChunk() {}

void LevelChunk::_createSubChunk(size_t idx, bool initSkyLight) {
	while (mSubChunks.size() <= idx) {
		// SubChunks are unique pointers, but get allocated through a PoolAllocator
		mSubChunks.push_back(make_unique<SubChunk>(initSkyLight && mDimension.getSkyLight() > Brightness::MIN));
		mDirtyTickCounters.push_back(&mSubChunks.back()->mDirtyTicksCounter);
	}
}

void LevelChunk::_setLight(const ChunkBlockPos& pos, Brightness br) {

	//TODO //OPTIMIZE since 99% of the brightness updates come from LightUpdates,
	//we can remove this check here and enforce that LightUpdates cause the chunks to be created
	//if needed? not that this if is that expensive since it's false 99% of the time
	size_t idx = pos.y >> 4;
	if (mSubChunks.size() <= idx) {
		if (br == mDefaultSkyBrightness) {
			return; //just do nothing, setting default on a null chunk is a waste
		}
		_createSubChunk(idx, true);
	}

	auto& sc = mSubChunks[idx];
	sc->mSkyLight.set(SubChunkBlockPos(pos).index(), br);
}

void LevelChunk::deferLightEmitter(const BlockPos& pos) {
	//TODO let's make this a lockless stack will we?
	std::lock_guard<SpinLock> lock(mLightListLock);
	mBlockLightSources.emplace_back(pos);
}

void LevelChunk::_lightGaps(BlockSource& source, const ChunkBlockPos& pos) {
	BlockPos start = pos + mMin;
	start.y = getHeightmap(pos);
}

FullBlock LevelChunk::setBlockAndData(const ChunkBlockPos& pos, FullBlock block, BlockSource* issuingSource, Unique<BlockEntity> blockEntity) {
	FullBlock old = getBlockAndData(pos);

	if (old != block) {
		int slot = pos.index2D();
		if (pos.y >= mRainHeights[slot] - 1) {
			mRainHeights[slot] = -999;
		}

		auto blockIndex = SubChunkBlockPos(pos).index();
		size_t subChunkIndex = pos.y >> 4;

		//TODO don't create the chunk if setting air
		_createSubChunk(subChunkIndex, true);
		
		auto& sc = mSubChunks[subChunkIndex];

		if (old.id != block.id) {
			sc->mBlocks[blockIndex] = block.id;
		}

		sc->mData.set(blockIndex, block.data);

		if (issuingSource) {
			sc->mDirtyTicksCounter.touch();
		}
	}

	return old;
}

/*public?*/
void LevelChunk::recalcHeightmap() {
	ChunkBlockPos p(0, 0, 0);

	for (p.x = 0; p.x < CHUNK_WIDTH; p.x++) {
		for (p.z = 0; p.z < CHUNK_WIDTH; p.z++) {
			p.y = getAllocatedHeight();

			mRainHeights[p.x + (p.z << 4)] = -999;

			while (p.y > 0 && Block::mLightBlock[getBlock(p.below())] == 0) {
				p.y--;
			}
			mHeightmap[p.index2D()] = p.y;

			//also recompute light
			//TODO //LIGHT optimize
			//TODO skip empty subchunks here
			if (mDefaultSkyBrightness > Brightness::MIN) {
				auto br = Brightness::MAX;
				ChunkBlockPos cur(p.x, getAllocatedHeight(), p.z);

				do {
					auto opaque = Block::mLightBlock[getBlock(cur)];
					if (br <= opaque) { //the next block will be black, stop descending
						break;
					}
					br -= opaque;
					
					//TODO //OPTIMIZE
					_setLight(cur, br);
					--cur.y;
				} while (cur.y > 0);
			}
		}
	}
}

bool LevelChunk::isBlockInChunk(const BlockPos& block) const {
	//only actually check x and z
	return block.x >= mMin.x && block.z >= mMin.z &&
		block.x <= mMax.x &&  block.z <= mMax.z;
}

void LevelChunk::setBorder(const ChunkBlockPos& pos, bool val) {
#ifdef MCPE_EDU
	if (!val) {
		//checks if there are any other border blocks
		//if we have any we can't turn this off yet
		for (int i = 0; i <= getAllocatedHeight(); ++i) {
			if (i == pos.y) {
				continue;
			}

			if (getBlock(ChunkBlockPos(pos.x, i, pos.z)) == Block::mBorder->getId()) {
				return;
			}
		}

		mBorderBlockMap[pos.index2D()] = false;
	}
	else {
		mBorderBlockMap[pos.index2D()] = true;
	}
#else
	UNUSED_PARAMETER(val,pos);
#endif
}

Height LevelChunk::getHeightmap(const ChunkBlockPos& pos) const {
	return mHeightmap[pos.index2D()];
}

void LevelChunk::recalcBlockLights() {
}

Brightness LevelChunk::getBrightness(const LightLayer& layer, const ChunkBlockPos& pos) {
	if (auto l = _lightLayer(layer, pos)) {
		return (Brightness)l->get(SubChunkBlockPos(pos).index());
	}
	return layer.getSurrounding();
}

DataLayer<CUBIC_CHUNK_VOLUME>* LevelChunk::_lightLayer(const LightLayer& layer, const ChunkBlockPos& pos) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) {
		return nullptr;
	}

	auto& sc = mSubChunks[idx];

	if (layer.isSky()) {
		return &sc->mSkyLight;
	}
	else if (layer == LightLayer::BLOCK) {
		return &sc->mBlockLight;
	}
	
	DEBUG_FAIL("Invalid light layer");
	return nullptr;
}

bool LevelChunk::setBrightness(const LightLayer& layerID, const ChunkBlockPos& pos, Brightness brightness) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) { 
		if (brightness == layerID.getSurrounding()) {
			return false; //no need to do anything
		}
		_createSubChunk(idx, true);
	}

	auto& sc = *mSubChunks[idx];
	auto& layer = sc.getLayer(layerID);

	auto index = SubChunkBlockPos(pos).index();
	auto old = layer.get(index);
	if (old == brightness) {
		return false;
	}

	layer.set(index, brightness);

	// only save out chunks due to lighting that actually were previously saved, as the generated chunks will
	// get lit correctly on next load
	if (mLoadState == ChunkState::Loaded && mDebugDisplaySavedState == ChunkDebugDisplaySavedState::Saved) {
		sc.mDirtyTicksCounter.touch();
	}
	return true;
}

Brightness LevelChunk::getRawBrightness(const ChunkBlockPos& pos, Brightness skyDampen) {
	size_t idx = pos.y >> 4;
	if (idx >= mSubChunks.size()) {
		return mDefaultSkyBrightness;
	}

	auto& sc = mSubChunks[idx];
	auto blockIdx = SubChunkBlockPos(pos).index();

	auto light = (Brightness)sc->mSkyLight.get(blockIdx);
	if (light > skyDampen) {
		light -= skyDampen;
	}
	else {
		light = Brightness::MIN;
	}

	auto block = (Brightness)sc->mBlockLight.get(blockIdx);

	return std::max(block, light);
}

bool LevelChunk::hasEntity(Entity& e) {
	return OwnedEntitySet::find(mEntities, e) != mEntities.end();
}

void LevelChunk::addEntity(Unique<Entity> e) {
	DEBUG_ASSERT(e, "Cannot add a null entity");
// 	DEBUG_ASSERT(!e->hasCategory(EntityCategory::Player), "Players are managed by the level");
	DEBUG_ASSERT(!hasEntity(*e), "The chunk already contains this entity");

	e->mChunkPos = mPosition;
	mEntities.emplace(std::move(e));

	_dirtyTicksCounter(LevelChunkDataField::Entity).touch();
}

Unique<Entity> LevelChunk::removeEntity(Entity& toRemove) {

	auto itr = OwnedEntitySet::find(mEntities, toRemove);
	if (itr != mEntities.end()) {
		Unique<Entity> temp = std::move(*itr);
		mEntities.erase(itr);

		_dirtyTicksCounter(LevelChunkDataField::Entity).touch();
		return temp;
	}
	else {
		DEBUG_FAIL("The chunk doesn't contain this entity");
		return nullptr;
	}
}

LevelChunk::LightList && LevelChunk::moveLightSources() {
	std::lock_guard<SpinLock> lock(mLightListLock);
	return std::move(mBlockLightSources);
}

bool LevelChunk::isSkyLit(const ChunkBlockPos& pos) {
	return pos.y >= (mHeightmap[pos.index2D()] & 0xff);
}

bool LevelChunk::tryChangeState(ChunkState from, ChunkState to) {
	//try to see if we can execute this transition
// 	bool success = mLoadState.compare_exchange_strong(from, to);

// 	return success;

	mLoadState.exchange(to);
	return true;
}

void LevelChunk::changeState(ChunkState from, ChunkState to) {
#ifdef PUBLISH
	tryChangeState(from, to);
#else
	DEBUG_ASSERT(tryChangeState(from, to), "Cannot change state, when using this call the initial state should be known");
#endif
}

void LevelChunk::getEntities(Entity* except, const AABB& bb, std::vector<Entity*>& es) {
	for (auto& e : mEntities) {
		if (e.get() != except && e->mBB.intersects(bb)) {
			es.push_back(e.get());
		}
	}
}

void LevelChunk::getEntities(EntityType type, const AABB& bb, std::vector<Entity*>& es, bool ignoreTargetType) const {
	for (auto& e : mEntities) {
		bool isOfType = EntityClassTree::isInstanceOf(*e, type);
		if (isOfType != ignoreTargetType && e->mBB.intersects(bb)) {
			es.push_back(e.get());
		}
	}
}

const OwnedEntitySet& LevelChunk::getEntities() const {
	return mEntities;
}

bool LevelChunk::setBlockExtraData(const ChunkBlockPos& localPos, uint16_t extraData) {

	int key = localPos.index();
	auto i = mBlockExtraData.find(key);

	if (extraData == 0) {
		if (i == mBlockExtraData.end()) {
			return false;	// nothing has changed

		}
		mBlockExtraData.erase(key);
	}
	else {
		// Value Changed at all?
		if (i == mBlockExtraData.end()) {
			mBlockExtraData[key] = extraData;
		}
		else if (i->second == extraData) {
			return false;	// nothing has changed
		}
		else {
			i->second = extraData;
		}
	}

	if (mLoadState == ChunkState::Loaded) {
		_dirtyTicksCounter(LevelChunkDataField::BlockExtraData).touch();
	}

	return true;
}

uint16_t LevelChunk::getBlockExtraData(const ChunkBlockPos& localPos) {

	auto cit = mBlockExtraData.find(localPos.index());
	if (cit != mBlockExtraData.end()) {
		return cit->second;
	}
	return 0;
}

bool LevelChunk::hasAnyBlockExtraData() const {
	return !mBlockExtraData.empty();
}

void memcpy_strided(void* dst, const void* src, size_t dataLen, size_t srcStride, size_t count) {
	auto d = (uint8_t*)dst;
	auto s = (uint8_t*)src;

	for (auto i : range(count)) {
		UNUSED_VARIABLE(i);

		memcpy(d, s, dataLen);
		d += dataLen;
		s += srcStride;
	}
}

bool isTopSolid(const Material* material, bool includeWater, bool includeLeaves) {
	if (material == nullptr) {
		return false;
	}
	else if (material->isType(MaterialType::Leaves)) {
		// Leaves conditionally count as solid based on includeLeaves
		return includeLeaves;
	}
	else if (includeWater && material->isType(MaterialType::Water)) {
		return true;
	}
	return material->getBlocksMotion();
}

Height LevelChunk::getAboveTopSolidBlock(const ChunkBlockPos& start, bool dimensionShowsSky, bool includeWater, bool includeLeaves) {
	const Material* material = nullptr;
	ChunkBlockPos pos = start;

	pos.y = std::min(pos.y, getAllocatedHeight()); //start from the lowest exitsting height

	if (!dimensionShowsSky) {
		//walk down until it's out of the ceiling (eg. it started inside a topsolid block - happens in the Nether)
		for (; pos.y > 0; --pos.y) {
			material = &Block::mBlocks[getBlock(pos)]->getMaterial();
			if (!isTopSolid(material, includeWater, includeLeaves)) {
				break;
			}
		}
	}

	//now that it's out of the ceiling, run normally
	while (pos.y >= 0) {
		material = &Block::mBlocks[getBlock(pos)]->getMaterial();
		if (isTopSolid(material, includeWater, includeLeaves)) {
			break;
		}
		--pos.y;
	}

	return pos.y >= 0 ? pos.y + 1 : 0;
}

bool LevelChunk::needsSaving(int wait, int maxWait) const {
	if (mLoadState >= ChunkState::Loaded && !mReadOnly) {
		for (auto& counter : mDirtyTickCounters) {
			if (counter->getTicksSinceLastChange() >= wait || counter->getTotalDirtyTicks() > maxWait) {
				return true;
			}
		}
	}
	return false;
}

bool LevelChunk::isDirty() const {
	return needsSaving(0, 0);
}

Biome& LevelChunk::getBiome(const ChunkBlockPos& pos) const {
	auto b = Biome::getBiome(mBiomes[pos.index2D()].biome);
	DEBUG_ASSERT(b, "Corrupted biome stored in a chunk, but it should be fixed at load time!");
	return *b;
}

void LevelChunk::setGrassColor(int grassColor, const ChunkBlockPos& pos) {
	auto& block = mCachedData[pos.index2D()];
	block.grassColor = grassColor | (255 << 24);
}

int LevelChunk::getGrassColor(const ChunkBlockPos& pos) {
	auto& block = mCachedData[pos.index2D()];
	return block.grassColor;
}

const BlockPos LevelChunk::getTopRainBlockPos(const ChunkBlockPos& pos) {
	const int slot = pos.x | (pos.z << 4);
	ChunkBlockPos highest(pos.x, (Height)mRainHeights[slot], pos.z);

	if (mRainHeights[slot] == -999) {
		const Height y = getAllocatedHeight();
		highest = ChunkBlockPos(pos.x, y, pos.z);

		int h = -1;

		while (highest.y > 0 && h == -1) {
			const auto& m = Block::mBlocks[getBlock(highest)]->getMaterial();
			if (!m.getBlocksMotion() && !m.isLiquid()) {
				highest = highest.below();
			}
			else {
				h = highest.y + 1;
			}
		}
		mRainHeights[slot] = std::min(h, (int)y);
	}
	return BlockPos(mMin.x + pos.x, mRainHeights[slot], mMin.z + pos.z);
}

const std::atomic<ChunkState>& LevelChunk::getState() const {
	return mLoadState;
}

void LevelChunk::onLoaded(BlockSource &resource) {

	BlockPos pos;
	BlockPos levelPos(mMin);
	resource.setPlaceChunkPos(mPosition);

	ChunkBlockPos cp;
	if (!resource.getLevel().isClientSide() && (mIsLoaded == false)) {
		for (cp.x = 0; cp.x < CHUNK_WIDTH; ++cp.x) {
			pos.x = cp.x + levelPos.x;
			for (cp.z = 0; cp.z < CHUNK_DEPTH; ++cp.z) {
				pos.z = cp.z + levelPos.z;
				for (cp.y = 0; cp.y <= getAllocatedHeight(); ++cp.y) {
					pos.y = cp.y + levelPos.y;
					auto id = getBlock(cp);
					if (id != Block::mAir->mID) {
						Block::mBlocks[id]->onLoaded(resource, pos);
					}
				}
			}
		}
	}

	mIsLoaded = true;
}

LevelChunkFormat LevelChunk::getLoadedFormat() const {
	return mLoadedFormat;
}

uint64_t LevelChunk::key() const {
	// unique key, used by containers
	uint64_t key = 0xFFFFFFFF & (uint64_t)mPosition.x;
	key |= (0xFFFFFFFF & (uint64_t)mPosition.z) << 32;
	return key;
}

const BlockPos& LevelChunk::getMin() const {
	return mMin;
}

const BlockPos& LevelChunk::getMax() const {
	return mMax;
}

const ChunkPos& LevelChunk::getPosition() const {
	return mPosition;
}

const bool LevelChunk::isReadOnly() const {
	return mReadOnly;
}

DimensionId LevelChunk::getDimensionId() const {
	return mDimension.getId();
}

LevelChunk::DirtyTicksCounter LevelChunk::DirtyTicksCounter::max() {
	DirtyTicksCounter c;
	c.lastChange = c.totalTime = INT_MAX;
	return c;
}

LevelChunk::DirtyTicksCounter::DirtyTicksCounter()
	: totalTime(INT_MIN)
	, lastChange(0) {

}

void LevelChunk::DirtyTicksCounter::touch() {
	if (totalTime < 0) { //first change
		totalTime = 0;
	}
	lastChange = totalTime;
}

LevelChunk::DirtyTicksCounter& LevelChunk::DirtyTicksCounter::operator++() {
	Math::safeIncrement(totalTime);
	return *this;
}

int LevelChunk::DirtyTicksCounter::getTicksSinceLastChange() const {
	return totalTime - lastChange;
}

int LevelChunk::DirtyTicksCounter::getTotalDirtyTicks() const {
	return totalTime;
}
