/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/Mob.h"
#include "Abilities.h"

class Block;
class ChestBlockEntity;
class Dimension;
class Inventory;
class ItemInstance;
enum class CooldownType;
class McItem;

struct Tick;

class Player: public Mob {
public:
	EntityType getEntityTypeId() const override {
		return EntityType::Player;
	}

	ItemInstance* getSelectedItem() const;
	virtual bool isCreative() const override;

	Abilities mAbilities;
	std::string mName;
};
