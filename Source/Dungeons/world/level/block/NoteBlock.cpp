/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "NoteBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"

static const std::vector<std::string> INSTRUMENTS = { "harp", "bd", "snare", "hat", "bassattack" };

NoteBlock::NoteBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood)) {
	mBlockEntityType = BlockEntityType::Music;
}

void NoteBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);
	onLoaded(region, pos);
}

bool NoteBlock::use(Player& player, const BlockPos& pos) const {
	return true;
}

bool NoteBlock::attack(Player* player, const BlockPos& pos) const {
	return true;
}

std::string NoteBlock::getSoundName(int index) const {
	if (index < 0 || index >= (int)INSTRUMENTS.size()) {
		index = 0;
	}

	return "note." + INSTRUMENTS[index];
}

float NoteBlock::getPitch(int note) const {
	return powf(2, (note - 12) / 12.0f);
}
