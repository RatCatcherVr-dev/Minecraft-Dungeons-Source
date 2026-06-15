/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/player/Player.h"
#include "world/item/McItem.h"
#include "world/level/block/RepeaterBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/redstone/Redstone.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"

const float RepeaterBlock::DELAY_RENDER_OFFSETS[4] = { -1.0f / 16.0f, 1.0f / 16.0f, 3.0f / 16.0f, 5.0f / 16.0f };
const int RepeaterBlock::DELAYS[4] = { 1, 2, 3, 4 };

RepeaterBlock::RepeaterBlock(const std::string& nameId, int id, bool on)
	: DiodeBlock(nameId, id, on) {
	mProperties = BlockProperty::BreakOnPush;
	setVisualShape(Vec3::ZERO, Vec3(1, 2.0f / 16.0f, 1));
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}


void RepeaterBlock::updateDelay(BlockSource &region, const BlockPos& pos, bool doIncrement) const {
	FullBlock fb = region.getBlockAndData(pos);
	int delay = getBlockState(BlockState::RepeaterDelay).get<int>(fb.data);
	if (doIncrement) {
		delay = (delay + 1) % 4;
		getBlockState(BlockState::RepeaterDelay).set(fb.data, delay);
	}
	region.setBlockAndData(pos, fb, Block::UPDATE_ALL);
}

bool RepeaterBlock::use(Player& player, const BlockPos& pos) const {
	BlockSource& region = player.getRegion();

	updateDelay(region, pos,true);

	return true;
}

int RepeaterBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return McItem::mRepeater->getId();
}

ItemInstance RepeaterBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(McItem::mRepeater);
}

bool RepeaterBlock::canBeSilkTouched() const {
	return false;
}

bool RepeaterBlock::isLocked(BlockSource& region, const BlockPos& pos, int data) const {
	return getAlternateSignal(region, pos, data) > Redstone::SIGNAL_NONE;
}

bool RepeaterBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return canSurvive(region, pos);
}

bool RepeaterBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}

	return DiodeBlock::canSurvive(region, pos);
}

void RepeaterBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		popResource(region, pos, ItemInstance(McItem::mRepeater));
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

void RepeaterBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	DiodeBlock::onRemove(region, pos);
}

int RepeaterBlock::getTurnOnDelay(int data) {
	int delay = getBlockState(BlockState::RepeaterDelay).get<int>(data);
	return DELAYS[delay] * 2;
}

const DiodeBlock* RepeaterBlock::getOnBlock() const {
	return static_cast<const DiodeBlock*>(Block::mPoweredRepeater);
}

const DiodeBlock* RepeaterBlock::getOffBlock() const {
	return static_cast<const DiodeBlock*>(Block::mUnpoweredRepeater);
}

bool RepeaterBlock::isAlternateInput(const Block& block) const {
	return isDiode(block);
}


bool RepeaterBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	int blockDir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
	if (direction == blockDir || direction == Direction::DIRECTION_OPPOSITE[blockDir]) {
		return true;
	}
	return false;
}