/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class PodzolBlock : public Block {
public:
	PodzolBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
};
