/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/FireBlock.h"
#include "world/level/block/PortalBlock.h"

#include "util/Random.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"

FireBlock::FireBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Fire)) {
	for (auto i : range(256)) {
		mFlameOdds[i] = mBurnOdds[i] = 0;
	}

	mAnimatedTexture = true;

	// TODO: this should be part of the Material, not the fire. (wood->isFlamable();)
	// @Todo: Abstract out this information to the base block and allow blocks to set how flammable and burnable they are.
	setFlammable(Block::mWoodPlanks->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mDoubleWoodenSlab->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mWoodenSlab->mID, FLAME_HARD, BURN_MEDIUM);

	setFlammable(Block::mFence->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mFenceGateOak->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mSpuceFenceGate->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mBirchFenceGate->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mJungleFenceGate->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mAcaciaFenceGate->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mDarkOakFenceGate->mID, FLAME_HARD, BURN_MEDIUM);

	setFlammable(Block::mSign->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mWallSign->mID, FLAME_HARD, BURN_MEDIUM);

	setFlammable(Block::mOakStairs->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mBirchStairs->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mSpruceStairs->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mJungleStairs->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mAcaciaStairs->mID, FLAME_HARD, BURN_MEDIUM);
	setFlammable(Block::mDarkOakStairs->mID, FLAME_HARD, BURN_MEDIUM);

	setFlammable(Block::mLog->mID, FLAME_HARD, BURN_HARD);
	setFlammable(Block::mLog2->mID, FLAME_HARD, BURN_HARD);

	setFlammable(Block::mLeaves->mID, FLAME_EASY, BURN_EASY);
	setFlammable(Block::mLeaves2->mID, FLAME_EASY, BURN_EASY);

	setFlammable(Block::mBookshelf->mID, FLAME_EASY, BURN_MEDIUM);
	setFlammable(Block::mTNT->mID, FLAME_MEDIUM, BURN_INSTANT);
	setFlammable(Block::mTallgrass->mID, FLAME_INSTANT, BURN_INSTANT);
	setFlammable(Block::mDoublePlant->mID, FLAME_INSTANT, BURN_INSTANT);
	setFlammable(Block::mYellowFlower->mID, FLAME_EASY, BURN_INSTANT);
	setFlammable(Block::mRedFlower->mID, FLAME_EASY, BURN_INSTANT);
	setFlammable(Block::mWool->mID, FLAME_EASY, BURN_EASY);
	setFlammable(Block::mWoolCarpet->mID, FLAME_EASY, BURN_EASY);
	setFlammable(Block::mVine->mID, FLAME_MEDIUM, BURN_INSTANT);
	setFlammable(Block::mCoalBlock->mID, FLAME_HARD, BURN_HARD);
	setFlammable(Block::mHayBlock->mID, FLAME_INSTANT, BURN_MEDIUM);

	setSolid(false);
	setTicking(true);
	mRenderLayer = RENDERLAYER_FIRE;
	mProperties = BlockProperty::BreakOnPush;

	mCanBuildOver = true;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& FireBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	return mAabb;
}

int FireBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int FireBlock::getTickDelay() const {
	return 30;
}

void FireBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	auto belowBlock = region.getBlockAndData(pos.below());
	bool infiniBurn = belowBlock.getBlock().isInfiniburnBlock(belowBlock.data);

	if (!mayPlace(region, pos)) {
		region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
	}

	//it's useless to set age data if the fire is infiniburning
	auto t = region.getBlockAndData(pos);
	auto& blockAgeState = getBlockState(BlockState::Age);
	int fireAge = blockAgeState.get<int>(t.data);
	if (!infiniBurn && fireAge < 15) {
		fireAge += random.nextInt(3) / 2;
		blockAgeState.set(t.data, fireAge);
		region.setBlockAndData(pos, t, Block::UPDATE_NONE);
	}

	if (!infiniBurn && !isValidFireLocation(region, pos)) {
		if (!region.isSolidBlockingBlock(pos.below()) || fireAge> 3) {
			region.setBlockAndData(pos, BlockID::AIR, Block::UPDATE_ALL);
		}

		return;
	}

	if (!infiniBurn && !canBurn(region, pos.below())) {
		if (fireAge == 15 && random.nextInt(4) == 0) {
			region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
			return;
		}
	}

	bool isHumid = region.isHumidAt(pos);
	int extra = 0;
	if (isHumid) {
		extra = -50;
	}

	checkBurn(region, pos.east(), 300 + extra, random, fireAge);
	checkBurn(region, pos.west(), 300 + extra, random, fireAge);
	checkBurn(region, pos.below(), 250 + extra, random, fireAge);
	checkBurn(region, pos.above(), 250 + extra, random, fireAge);
	checkBurn(region, pos.north(), 300 + extra, random, fireAge);
	checkBurn(region, pos.south(), 300 + extra, random, fireAge);

	for (int xx = -1; xx <= 1; xx++) {
		for (int zz = -1; zz <= 1; zz++) {
			for (int yy = -1; yy <= 4; yy++) {
				if (xx == 0 && yy == 0 && zz == 0) {
					continue;
				}

				int rate = 100;
				if (yy > 1) {
					rate += ((yy - 1) * 100);
				}

				BlockPos testPos(pos.offset(xx, yy, zz));
				int fireOdds = getFireOdds(region, testPos);
				if (fireOdds <= 0) {
					continue;
				}

// 				int difficultyMultiplier = DifficultyUtils::getMultiplier(region.getLevel().getDifficulty());
				int difficultyMultiplier = 1;

				int odds = (fireOdds + 40 + difficultyMultiplier * 7) / (fireAge + 30);
				if (isHumid) {
					odds /= 2;
				}
				if (odds > 0 && random.nextInt(rate) <= odds) {
					region.setBlockAndData(testPos, t, Block::UPDATE_ALL);
				}
			}
		}
	}
}

bool FireBlock::mayPick() const {
	return false;
}

bool FireBlock::canBurn(BlockSource& region, const BlockPos& pos) const {
	return mFlameOdds[region.getBlockID(pos)] > 0;
}

int FireBlock::getFlammability(BlockSource& region, const BlockPos& pos, int odds) const{
	int f = mFlameOdds[region.getBlockID(pos)];
	if (f > odds) {
		return f;
	}

	return odds;
}

bool FireBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return region.isSolidBlockingBlock(pos.below()) || isValidFireLocation(region, pos);
}

void FireBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const{
	if (!region.isSolidBlockingBlock(pos.below()) && !isValidFireLocation(region, pos)) {
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
		return;
	}
}

void FireBlock::onPlace(BlockSource& region, const BlockPos& pos) const{
	auto& dimension = region.getDimension();
	if (dimension.getId() == DimensionId::Overworld || dimension.getId() == DimensionId::Nether) {
		if (((const PortalBlock*)Block::mPortal)->trySpawnPortal(region, pos)) {
			return;
		}
	}
	if (!region.isSolidBlockingBlock(pos.below()) && !isValidFireLocation(region, pos)) {
		region.setBlockAndData(pos, BlockID::AIR, Block::UPDATE_ALL);
		return;
	}
}

void FireBlock::ignite(BlockSource& region, const BlockPos& pos) {
	bool lit = false;
	if (!lit) {
		lit = tryIgnite(region, pos.above());
	}
	if (!lit) {
		lit = tryIgnite(region, pos.west());
	}
	if (!lit) {
		lit = tryIgnite(region, pos.east());
	}
	if (!lit) {
		lit = tryIgnite(region, pos.north());
	}
	if (!lit) {
		lit = tryIgnite(region, pos.south());
	}
	if (!lit) {
		lit = tryIgnite(region, pos.below());
	}
	if (!lit) {
		region.setBlock(pos, mID, Block::UPDATE_ALL);
	}
}

bool FireBlock::canBeSilkTouched() const {
	return false;
}

void FireBlock::setFlammable(BlockID id, int flame, int burn) {
	mFlameOdds[id] = flame;
	mBurnOdds[id] = burn;
}

void FireBlock::checkBurn(BlockSource& region, const BlockPos& pos, int chance, Random& random, int age) const {
	int odds = mBurnOdds[region.getBlockID(pos)];
	if (random.nextInt(chance) < odds) {
		bool wasTnt = region.getBlockID(pos) == Block::mTNT->mID;

		if (random.nextInt(age + 10) < 5) {
			int fireAge = age + random.nextInt(5) / 4;
			if (fireAge > 15) {
				fireAge = 15;
			}

			DataID data = 0;
			getBlockState(BlockState::Age).set(data, fireAge);

			region.setBlockAndData(pos, mID, data, Block::UPDATE_ALL);
		} else {
			region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
		}

		if (wasTnt) {
			Block::mTNT->destroy(region, pos, 1, nullptr);
		}
	}
}

bool FireBlock::isValidFireLocation(BlockSource& region, const BlockPos& pos) const {
	return canBurn(region, pos.east()) ||
		   canBurn(region, pos.west()) ||
		   canBurn(region, pos.below()) ||
		   canBurn(region, pos.above()) ||
		   canBurn(region, pos.north()) ||
		   canBurn(region, pos.south());
}

int FireBlock::getFireOdds(BlockSource& region, const BlockPos& pos) const{
	int odds = 0;
	if (!region.isEmptyBlock(pos)) {
		return 0;
	}

	odds = getFlammability(region, pos.east(), odds);
	odds = getFlammability(region, pos.west(), odds);
	odds = getFlammability(region, pos.below(), odds);
	odds = getFlammability(region, pos.above(), odds);
	odds = getFlammability(region, pos.north(), odds);
	odds = getFlammability(region, pos.south(), odds);

	return odds;
}

bool FireBlock::tryIgnite(BlockSource& region, const BlockPos& pos){
	BlockID blockID = region.getBlockID(pos);
	if (blockID == mID) {
		return true;
	}

	if (blockID == BlockID::AIR) {
		region.setBlock(pos, mID, Block::UPDATE_ALL);
		return true;
	}

	return false;
}
