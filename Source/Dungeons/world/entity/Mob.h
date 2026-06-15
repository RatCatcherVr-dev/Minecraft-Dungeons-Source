/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/Entity.h"
#include "world/item/ItemInstance.h"
#include "SharedConstants.h"

class Sensing;
class MobEffect;

class Mob : public Entity {
public:
	// Constructor for Player classes
	Mob(Level& level) : Entity(level) {}

	static constexpr int PLAYER_HURT_EXPERIENCE_TIME = SharedConstants::TicksPerSecond * 5;
};
