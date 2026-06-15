#pragma once

#include "game/level/TilePreparer.h"
#include "game/mobspawn/MobSpawnTypes.h"

class UWorld;

namespace game { namespace mobspawn {

class FixedSpawner {
public:
	FixedSpawner(UWorld&, Config spawnConfig);

	void operator()(tile::TilePreparationState);
private:
	UWorld& mWorld;
	Config mSpawnConfig;
};

}}
