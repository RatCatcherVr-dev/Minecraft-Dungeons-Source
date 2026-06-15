/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TntBlock.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/McItem.h"

TntBlock::TntBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Explosive)) {

}

int TntBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const{
	return 1;
}

bool TntBlock::use(Player& player, const BlockPos& pos) const {
	ItemInstance* carried = player.getSelectedItem();
// 	if (carried && (carried->isInstance(McItem::mFlintAndSteel) || carried->isInstance(McItem::mFireCharge) || EnchantUtils::hasEnchant(Enchant::WEAPON_FIRE, *carried))) {
	if (carried && (carried->isInstance(McItem::mFlintAndSteel) || carried->isInstance(McItem::mFireCharge))) {
		auto& region = player.getRegion();

		DataID data = 0;
		getBlockState(BlockState::ExplodeBit).set(data, true);

		carried->hurtAndBreak(1, &player);
		region.setBlockAndData(pos, BlockID::AIR, Block::UPDATE_ALL, &player);
		destroy(region, pos, data, &player);

		return true;
	}

	return Block::use(player, pos);
}

bool TntBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return true;
}

void TntBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (strength > 0) {
		DataID data = 0;
		getBlockState(BlockState::ExplodeBit).set(data, true);

		destroy(region, pos, data, nullptr);
		region.setBlock(pos.x, pos.y, pos.z, BlockID::AIR, Block::UPDATE_ALL);
	}
}

void TntBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

bool TntBlock::shouldExplode(const DataID& data) const {
	return getBlockState(BlockState::ExplodeBit).getBool(data);
}
