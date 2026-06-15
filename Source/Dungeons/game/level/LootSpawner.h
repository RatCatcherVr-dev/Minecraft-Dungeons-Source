#pragma once

#include "game/level/TilePreparer.h"
#include "game/level/chests/ChestSelector.h"
#include "util/FloatRange.h"

namespace lovika {
class Region;
}

namespace game {

class Game;
typedef uint32_t RandomSeed;
	
namespace tile {

class LootSpawner {
	struct Config {
		float regionLootProbability;
		float regionChestProbability;
		FloatRange strayPathChestProbability;
		FloatRange strayPathChestItemProbability;
	};
	static const Config DefaultConfig;
	static const chest::ChestTypeProbabilityConfig DefaultChestTypeProbabilityConfig;
	static const chest::ChestTypeProbabilityConfig GuaranteedItemChestTypeProbabilityConfig;
public:
	LootSpawner(Game&, bool delayedItemCreation = true);

	void operator()(TilePreparationState);
private:
	void _placeStrayPathChests(RandomSeed);
	void _placeChest(TilePreparationState);
	bool _placeChest(TileRef, const BlockCuboid&, Random&, const chest::ChestTypeProbabilityConfig&);

	void _spawnLoot(TilePreparationState) const;
	void _spawnLoot(FVector, Random&) const;

	bool _canSpawnAt(const lovika::Region&) const;

	Game& mGame;
	Config mConfig = DefaultConfig;
	bool mDelayedItemCreation;
	//D11.PS - Changed this to uint32, console didnt like size_t
	//TSet<size_t> mChestRegionHashes;
	TSet<uint32> mChestRegionHashes;
};

}}
