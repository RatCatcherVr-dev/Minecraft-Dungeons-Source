#pragma once

#include "game/level/TilePreparer.h"
#include "game/mobspawn/MobSpawnTypes.h"

class UWorld;
namespace io { struct StretchMobs; }

namespace game {
struct DifficultyStats;
	
namespace mobspawn {

struct CalculatedMobs {
	CalculatedMobs() = default;
	CalculatedMobs(std::vector<io::MobGroup>, int count);

	std::vector<io::MobGroup> groups;
	int count = 0;
};

class AlphaSpawner {
public:
	AlphaSpawner(UWorld&, const DifficultyStats&, Config spawnConfig);
	virtual ~AlphaSpawner() = default;

	void operator()(tile::TilePreparationState);
protected:
	virtual CalculatedMobs calculateMobs(tile::TilePreparationState) const;

	int calculateDefaultMobCountForTile(tile::TilePreparationState, float countMultiplier = 1) const;
private:
	UWorld& mWorld;
	const DifficultyStats& mDifficulty;
	Config mSpawnConfig;
};

}}
