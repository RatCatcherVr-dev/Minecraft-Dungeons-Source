#pragma once

#include "world/level/block/Block.h"

class Level;

class VoidBlock : public Block {
public:
	VoidBlock(const std::string& nameId, int id);
};
