/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/entity/BlockEntity.h"

#include "world/level/BlockSource.h"

BlockEntity::MapIdType BlockEntity::idClassMap;
BlockEntity::MapTypeId BlockEntity::classIdMap;

int BlockEntity::_runningId = 0;

void BlockEntity::setId(BlockEntityType type, const std::string& id) {
	//make class things, but only once pls (was a static)
	MapIdType::const_iterator cit = idClassMap.find(id);
	if (cit == idClassMap.end()) {
		idClassMap.insert(std::make_pair(id, type));
		classIdMap.insert(std::make_pair(type, id));
	}
	else{
		DEBUG_FAIL("Pair added twice");
	}
}

void BlockEntity::initBlockEntities() {
	setId(BlockEntityType::Furnace, "Furnace");
	setId(BlockEntityType::Chest, "Chest");
	setId(BlockEntityType::NetherReactor, "NetherReactor");
	setId(BlockEntityType::Sign, "Sign");
	setId(BlockEntityType::MobSpawner, "MobSpawner");
	setId(BlockEntityType::EnchantingTable, "EnchantTable");
	setId(BlockEntityType::Skull, "Skull");
	setId(BlockEntityType::FlowerPot, "FlowerPot");
	setId(BlockEntityType::BrewingStand, "BrewingStand");
	setId(BlockEntityType::DaylightDetector, "DaylightDetector");
	setId(BlockEntityType::Music, "Music");
	setId(BlockEntityType::Comparator, "Comparator");
	setId(BlockEntityType::Dispenser, "Dispenser");
	setId(BlockEntityType::Dropper, "Dropper");
	setId(BlockEntityType::Hopper, "Hopper");
	setId(BlockEntityType::Cauldron, "Cauldron");
	setId(BlockEntityType::ItemFrame, "ItemFrame");
	setId(BlockEntityType::PistonArm, "PistonArm");
	setId(BlockEntityType::Beacon, "Beacon");
	setId(BlockEntityType::MovingBlock, "MovingBlock");
	setId(BlockEntityType::EndPortal, "EndPortal");
	setId(BlockEntityType::EnderChest, "EnderChest");
	setId(BlockEntityType::EndGateway, "EndGateway");
	setId(BlockEntityType::StructureBlock, "StructureBlock");
	setId(BlockEntityType::Chalkboard, "ChalkboardBlock");
}

//
// BlockEntity
//

BlockEntity::BlockEntity(BlockEntityType type, const BlockPos& pos, const std::string& id)
	: mBlock(nullptr)
	, mDestroyTimer(0)
	, mDestroyProgress(0.0f)
	, mPosition(pos)
	, mBB(pos, pos + Vec3::ONE)
	, mData(-1)
	, mType(type)
	, mRunningId(++_runningId)
	, mClientSideOnly(false)
	, mRendererId(TR_DEFAULT_RENDERER) {

	//init position stuff
	_resetAABB();
}

BlockEntity::~BlockEntity() {
#ifdef ENABLE_DEBUG_RENDERING
	if (this == DebugRenderer::getDebugBlockEntity()) {
		DebugRenderer::clearDebugBlock();
	}
#endif
}

void BlockEntity::tick(BlockSource& region ) {
	++mTickCount;

	if (!mBlock) {//first tick
		mBlock = Block::mBlocks[region.getBlockID(mPosition)];
	}

	if (mChanged) {
		region.fireBlockEntityChanged(*this);
		onChanged(region);
		mChanged = false;
	}
}

bool BlockEntity::isFinished() {
	return false;
}

void BlockEntity::setChanged() {
	mChanged = true;
}

void BlockEntity::onChanged(BlockSource& region) {

}

void BlockEntity::setMovable(bool canMove) { 
	mIsMovable = canMove; 
}

bool BlockEntity::isMovable() { 
	return mIsMovable; 
}

float BlockEntity::distanceToSqr(const Vec3& to) {
	return to.distanceToSqr(mPosition + Vec3(0.5f));
}

void BlockEntity::clearCache() {
	mBlock = nullptr;
	mData = -1;
}

bool BlockEntity::isType( BlockEntityType type ) {
	return mType == type;
}

bool BlockEntity::isType(BlockEntity& te, BlockEntityType type) {
	return te.isType(type);
}

bool BlockEntity::isInWorld() const {
	return mPosition != BlockPos::MIN;
}

void BlockEntity::onNeighborChanged(BlockSource& region, const BlockPos& position) {

}

float BlockEntity::getShadowRadius(BlockSource& region) const {	//todo WHYYYYYY ISNT THIS INHERITING ENTITY
	return 0;
}

bool BlockEntity::hasAlphaLayer() const {
	return false;
}

void BlockEntity::stopDestroy() {
	mDestroyProgress = 0;
	mDestroyTimer = 0;
}

BlockEntity& BlockEntity::getCrackEntity(BlockSource& region, const BlockPos& pos) {
	return *this;
}

const AABB& BlockEntity::getAABB() const {
	return mBB;
}

void BlockEntity::setBB(AABB value) {
	mBB = value;
}

const BlockPos& BlockEntity::getPosition() const {
	return mPosition;
}

int BlockEntity::getData() const {
	return mData;
}

void BlockEntity::setData(int value) {
	mData = value;
}

void BlockEntity::moveTo(const BlockPos& newPos) {
	mPosition = newPos;
}

const BlockEntityType& BlockEntity::getType() const {
	return mType;
}

int BlockEntity::getRunningId() const {
	return mRunningId;
}

void BlockEntity::setRunningId(int value) {
	mRunningId = value;
}

bool BlockEntity::isClientSideOnly() const {
	return mClientSideOnly;
}

void BlockEntity::setClientSideOnly(bool value) {
	mClientSideOnly = value;
}

const BlockEntityRendererId& BlockEntity::getRendererId() const {
	return mRendererId;
}

void BlockEntity::setRendererId(BlockEntityRendererId value) {
	mRendererId = value;
}

void BlockEntity::getDebugText(std::vector<std::string>& outputInfo) {
	if (mBlock) {
		mBlock->getDebugText(outputInfo);
		outputInfo.push_back("");
	}
	else {
		outputInfo.push_back("No valid block");
	}
	outputInfo.push_back("Type: " + classIdMap.at(getType()));
	outputInfo.push_back("Position: " + getPosition().toString());
	outputInfo.push_back("Data: " + Util::toString(mData));
}

const Block* BlockEntity::getBlock() {
	return mBlock;
}

void BlockEntity::_resetAABB() {
	Vec3 min = mPosition;
	Vec3 max = min + Vec3::ONE;
	mBB.set(min, max);
}

void BlockEntity::_destructionWobble(float& x, float& y, float& z) {
	if (mDestroyProgress > 0 ) {
		mDestroyTimer += 1.f / 60.f;

		if (mDestroyTimer > 0.2f) {
			const static float a = 0.15f;
			float s = 0.01f + 0.05f * mDestroyProgress;
			Vec3 o = mDestroyDirection * s;
			o.xRot((Math::random() - 0.5f) * Math::TAU * a);
			o.yRot((Math::random() - 0.5f) * Math::TAU * a);

			x += o.x;
			y += o.y;
			z += o.z;

			mDestroyTimer = 0;
		}
	}
}
