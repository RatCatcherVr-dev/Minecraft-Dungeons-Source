/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "legacy/Core/Utility/UUID.h"
#include "EntityUniqueID.h"
#include "util/Random.h"
#include "world/level/ChunkPos.h"
#include "LevelConstants.h"
#include "world/entity/EntityTypes.h"

#include "world/phys/Vec2.h"

#include "SharedConstants.h"
#include "legacy/Core/Math/Color.h"
#include "world/phys/HitResult.h"
#include "GameType.h"

class Biome;
class BlockSource;
class Dimension;
class Entity;
class LevelChunk;
class LightLayer;
class Material;

class Level {
	friend class Dimension;
public:
	static const int TICKS_PER_DAY = SharedConstants::TicksPerSecond * 60 * 20;

	Level();
	virtual ~Level();

	bool isDayCycleActive();

	int getTime() const;
	int setTime(int time);
	int setStopTime(int time);
	RandomSeed getSeed();

	GameType getGameType() const { return GameType::Creative; }

	EntityUniqueID getNewUniqueID();
public:
	bool isClientSide() const;

	Random& getRandom() const;

	mutable Random mRandom, mAnimateRandom;

	std::string mLevelId;
private:
	bool mIsClientSide = false;
	EntityUniqueID mLastUniqueID;
};
