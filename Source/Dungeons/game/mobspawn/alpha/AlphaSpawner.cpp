#include "Dungeons.h"
#include "AlphaSpawner.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "util/FloatWeighedRandom.h"
#include "lovika/RegionPredicates.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/Conversion.h"
#include "game/level/GameTiles.h"
#include "game/GameBP.h"

namespace game { namespace mobspawn {

//
// CalculatedMobs
//
CalculatedMobs::CalculatedMobs(std::vector<io::MobGroup> groups, int count)
	: groups(std::move(groups))
	, count(count) {
}

//
// SpawnRotations. Putting it in the cpp file because:
//   1) It was only used locally in this file
//   2) It may have some behavioral quirks we want to fix before it's used somewhere else
//
class SpawnRotations {
public:
	SpawnRotations()
		: rotations{ 0 } {
	}
	SpawnRotations(const std::string& string)
		: rotations{ 0 }
	{
		if (!string.empty()) {
			// should go through the chars after the last ":" character
			const auto& lastChar = string.back();
			switch (lastChar) {
			case 'n': rotations |= north; break;
			case 'w': rotations |= west; break;
			case 's': rotations |= south; break;
			case 'e': rotations |= east; break;
			default: rotations = 0;
			}
		}
	}

	std::vector<float> getAngles() const {
		if (rotations == 0) {
			return { 0.f, 90.f, 180.f, 270.f };
		}

		std::vector<float> angles;
		if (rotations & north) { angles.push_back(0.f); }
		if (rotations & west) { angles.push_back(90.f); }
		if (rotations & south) { angles.push_back(180.f); }
		if (rotations & east) { angles.push_back(270.f); }

		return angles;
	}

private:
	char rotations;

	static const char north = 0b00000001;
	static const char west  = 0b00000010;
	static const char south = 0b00000100;
	static const char east  = 0b00001000;
};


//
// Spawners
//
AlphaSpawner::AlphaSpawner(UWorld& world, const DifficultyStats& difficulty, Config spawnConfig)
	: mWorld(world)
	, mDifficulty(difficulty)
	, mSpawnConfig(std::move(spawnConfig)) {
}

void AlphaSpawner::operator()(tile::TilePreparationState state) {
	auto& tile = state.tile;
	auto& regions = tile.spawnRegions();

	if (regions.isEmpty()) {
		return;
	}
	const auto& calculatedMobs = calculateMobs(state);
	AGameBP& game = *actorquery::getFirstActor<AGameBP>(&mWorld);

	for (auto& group : calculateMobsWithEnchantments(calculatedMobs.groups, calculatedMobs.count, mDifficulty, &state.rnd)) {
		auto region = regions.getRandom(state.rnd);
		auto transformProvider = providers::FromProviders(
			providers::position::Region(region.area(), &state.rnd),
			providers::yaw::OneOf(SpawnRotations(region.name()).getAngles(), &state.rnd)
		);
		game.RequestMobGroupSpawn(group, transformProvider, mSpawnConfig, AGameBP::default_PostGroupSpawnFunc, AGameBP::default_PostSpawnFunc, game::mobspawn::MobSpawnPriority::ESpawnPriority_Low);
	}
}

CalculatedMobs AlphaSpawner::calculateMobs(tile::TilePreparationState state) const {
	const auto& mobDefs = state.tile.stretch().def.mobs;

	if (const auto mobCount = calculateDefaultMobCountForTile(state, mobDefs.density)) {
		return CalculatedMobs(mobDefs.groups, mobCount);
	}
	return {};
}

int AlphaSpawner::calculateDefaultMobCountForTile(tile::TilePreparationState state, float countMultiplier) const {
	const float randMultiplier = state.tile.isObjectiveTileHACK() ?
		state.rnd.nextFloat(0.75f, 1.25f) :
		state.rnd.nextFloat(0.5f, 1.5f);

	const auto totalSpawnArea = state.tile.spawnRegions().totalArea();
	const float MobBaseDensity = 1.0f / 8.0f;
	return FMath::Max(0, FMath::RoundToInt(mDifficulty.GetMobCountMultiplier() * randMultiplier * countMultiplier * FMath::Pow(totalSpawnArea, 0.9f) * MobBaseDensity));
}

}}
