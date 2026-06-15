/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/GrassBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/block/TallGrass.h"
#include "world/level/block/DirtBlock.h"
#include "world/entity/Mob.h"
#include "world/level/biome/Biome.h"
#include "world/level/FoliageColor.h"

// REFACTOR: Just need to know about ParsedAtlasNode. Yuck. 
#include "client/renderer/texture/TextureAtlas.h"

const Brightness GrassBlock::MIN_BRIGHTNESS(4);

GrassBlock::GrassBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Dirt))
{
	setTicking(true);

	mRenderLayer = RENDERLAYER_OPAQUE;
}

void GrassBlock::tick(BlockSource& region, const BlockPos& pos, Random& random ) const{
	if (region.getLevel().isClientSide()) {
		return;
	}

	BlockPos above = pos.above();

	auto topBrightness = region.getRawBrightness(above);
	if (topBrightness < MIN_BRIGHTNESS) {
		auto& aboveMat = region.getMaterial(above);
		if (aboveMat.getTranslucency() < 0.2f && random.nextInt(4) == 0) {
			region.setBlockAndData(pos, Block::mDirt->mID, Block::UPDATE_CLIENTS);
		}
	} else {
		if (topBrightness >= Brightness::MAX - 6) {

			int randZ = pos.z + random.nextInt(3) - 1;
			int randY = pos.y + random.nextInt(5) - 3;
			int randX = pos.x + random.nextInt(3) - 1;
			BlockPos t = BlockPos(randX, randY, randZ);

			auto block = region.getBlockAndData(t);
			if (block.id == Block::mDirt->mID) {
				int type = Block::mDirt->getBlockState(BlockState::MappedType).get<int>(block.data);
				if (type == DirtBlock::TYPE_NORMAL) {
					auto aboveBlock = region.getBlockAndData(t.above());
					auto& material = region.getMaterial(t.above());
					if (region.getRawBrightness(t.above()) >= MIN_BRIGHTNESS && !Block::mBlocks[aboveBlock.id]->isSolid() && !material.isLiquid()) {
						region.setBlockAndData(t, Block::mGrass->mID, Block::UPDATE_CLIENTS);
					}
				}
			}
		}
	}
}

int GrassBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->getResource(random, 0, bonusLootLevel);
}

bool GrassBlock::_randomWalk(BlockSource& region, BlockPos& pos, int j) const {
	auto& random = region.getLevel().getRandom();

	for (int i = 0; i < j / 16; i++) {
		pos.x += random.nextInt(3) - 1;
		int randY0 = random.nextInt(3) - 1;
		pos.y += randY0 * random.nextInt(3) / 2;
		pos.z += random.nextInt(3) - 1;

		if (!region.getBlock(pos.below()).isType(Block::mGrass) || region.isSolidBlockingBlock(pos)) {
			return false;
		}
	}

	return j / 16 > 0;
}

bool GrassBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	auto& random = region.getLevel().getRandom();

	for (int j = 16; j < 64; j++) {
		BlockPos abovePos = pos.above();

		//see if there is a path to this block
		if ( !_randomWalk(region, abovePos, j )) {
			continue;
		}

		auto block = region.getBlockID(abovePos);
		if (block == BlockID::AIR) {
			FullBlock toPlant = FullBlock::AIR;

			int randomInt = random.nextInt(8);
			if (randomInt == 0) {
				toPlant = region.getBiome(abovePos).getRandomFlowerTypeAndData(random, abovePos);
			}
			else {
				toPlant.id = Block::mTallgrass->mID;
				int tallGrassType = (randomInt == 2) ? enum_cast(TallGrassType::Fern) : enum_cast(TallGrassType::Tall);
				Block::mTallgrass->getBlockState(BlockState::MappedType).set(toPlant.data, tallGrassType);
			}

			DEBUG_ASSERT(Block::mBlocks[toPlant.id] != nullptr, "Invalid flower/grass block returned!");

			//try to plant it
			if (Block::mBlocks[toPlant.id]->canSurvive(region, abovePos)) {
				region.setBlockAndData(abovePos, toPlant, Block::UPDATE_ALL);
			}
		}
	}

	return true;
}
