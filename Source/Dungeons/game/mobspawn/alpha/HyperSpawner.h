#pragma once

#include "AlphaSpawner.h"

struct FMissionState;

namespace game { class Game; }

namespace game { namespace mobspawn {

class HyperSpawner : public AlphaSpawner {
public:
	HyperSpawner(const game::Game&, const DifficultyStats&, const FMissionState&, Config spawnConfig);

	const std::vector<io::MobGroup>& mobGroups() const { return mMobGroups; }
private:
	virtual CalculatedMobs calculateMobs(tile::TilePreparationState) const;

	std::vector<io::MobGroup> calculateSubMissionMobGroups(tile::TilePreparationState) const;
	std::vector<io::MobGroup> mMobGroups;
	mutable int mMobGroupIndex = 0;
	mutable TMap<FString, TOptional<std::vector<io::MobGroup>>> mSubMissionMobGroups;
};

}}
