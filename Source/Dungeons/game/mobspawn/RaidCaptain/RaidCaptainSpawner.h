// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "game/mobspawn/MobSpawnTypes.h"
#include "game/level/GameTiles.h"
#include "game/level/TilePreparer.h"

class UWorld;
 
namespace game { namespace mobspawn {

class RaidCaptainSpawner 
{

public:

	RaidCaptainSpawner(UWorld&, Config, const FMissionState&, const Tiles&);

	void operator()(tile::TilePreparationState) noexcept;

	io::MobGroup createMobGroup();
private:

	TWeakObjectPtr<UWorld> mWorld;
	std::vector<TilePtr> mTiles;
	Config mSpawnConfig;
	FMissionState MissionState;
	int32 MaxAmountOfSpawns;
	int32 CurrentAmountOfSpawns;
	int32 MinAmountOfMobToSpawn;
	int32 MaxAmountOfMobToSpawn;
};

}}
