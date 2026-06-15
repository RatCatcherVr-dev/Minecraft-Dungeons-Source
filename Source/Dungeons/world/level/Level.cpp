/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "chunk/LevelChunk.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "util/Random.h"

Level::Level() {}

Level::~Level() {
	UE_LOG(LogDungeons, Log, TEXT("Destroying the Level..."));

	////remove chunk memory
	LevelChunk::trimMemoryPool();
	LevelChunk::SubChunk::trimMemoryPool();
}

bool Level::isDayCycleActive() {
	return false;
}

int Level::setTime(int time) {
	return 0;
}

int Level::setStopTime(int time) {
	return 0;
}

RandomSeed Level::getSeed() {
	return 0;
}

int Level::getTime() const {
	return 0;
}

bool Level::isClientSide() const {
 	return mIsClientSide;
}

Random& Level::getRandom() const {
	return mRandom;
}

EntityUniqueID Level::getNewUniqueID() {
	DEBUG_ASSERT_MAIN_THREAD;
	DEBUG_ASSERT(!mIsClientSide, "Cannot do this on clients, you should feel bad!");

	return ++mLastUniqueID;
}

