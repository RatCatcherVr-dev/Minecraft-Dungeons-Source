/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class NoteBlock : public EntityBlock {
	//private static final List<String> INSTRUMENTS = Lists.newArrayList("harp", "bd", "snare", "hat", "bassattack");

public:
	NoteBlock(const std::string& nameId, int id);

	virtual bool isInteractiveBlock() const override {
		return true;
	}
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual bool use(Player& player, const BlockPos& pos) const override;
	virtual bool attack(Player* player, const BlockPos& pos) const override;
	std::string getSoundName(int index) const;
	float getPitch(int note) const;
};
