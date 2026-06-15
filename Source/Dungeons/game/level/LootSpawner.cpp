#include "Dungeons.h"
#include "LootSpawner.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/ItemUtil.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/level/chests/ChestSelector.h"
#include "game/objective/ObjectiveUtil.h"
#include "Engine/AssetManager.h"
#include "lovika/RegionPredicates.h"
#include "util/RandomUtil.h"

namespace DungeonsQA {
	extern TAutoConsoleVariable<int32> CVAllLootChests;
}

namespace game { namespace tile {

const std::hash<BlockCuboid> regionHasher;

const LootSpawner::Config LootSpawner::DefaultConfig {
	0.3f,                       // region loot probability
	0.02f,                      // region chest probability
	FloatRange { 0.2f, 0.4f },	// stray-path chest probability
	FloatRange { 0.3f, 0.7f }	// stray-path likelihood that any chest found is of the "guaranteed item chest type" config
};

const chest::ChestTypeProbabilityConfig LootSpawner::DefaultChestTypeProbabilityConfig({
	{EChestType::Wooden, 100.0f} // @todo: Add urns here as well
});

const chest::ChestTypeProbabilityConfig LootSpawner::GuaranteedItemChestTypeProbabilityConfig({
	{EChestType::Fancy, 100.0f}
});

LootSpawner::LootSpawner(Game& game, bool delayedItemCreation)
	: mGame(game)
	, mDelayedItemCreation(delayedItemCreation)
{
	_placeStrayPathChests(game.settings().randomSeed);

	if (!mDelayedItemCreation) {
		Random rnd(game.settings().randomSeed);
		for (auto& tile : game.tiles().getTiles()) {
			const TilePreparationState state{ rnd, *tile };
			_placeChest(state);
			_spawnLoot(state);
		}
	}
}

void LootSpawner::operator()(TilePreparationState state) {
	if (mDelayedItemCreation) {
		_placeChest(state);
		_spawnLoot(state);
	}
}

//
// Chests
//
void LootSpawner::_placeStrayPathChests(RandomSeed seed) {
	TMap<int, TArray<TilePtr>> strayTilesPerPath;

	for (auto tile : mGame.tiles().getTiles()) {
		if (tile->progress().isOnStrayPath()) {
			strayTilesPerPath.FindOrAdd(tile->progress().strayPathSubId()).Add(tile);
		}
	}
	const auto chestProbabilityMultiplier = mGame.affectors().GetChestProbabilityMultiplier();
	Random rnd(seed);
	for (auto& kv : strayTilesPerPath) {
		// Place a chest as late in the stray path as possible
		kv.Value.Sort([](TileRef a, TileRef b) { return b.isFurtherThan(a); });

		const int strayPathLength = kv.Value.Top()->progress().strayPath().length();
		const float spRewardFraction = FloatRange(1, 5).clampedFractionAt(strayPathLength);
		const float spRewardProbability = strayPathLength >= 5 ? 1 : chestProbabilityMultiplier * mConfig.strayPathChestProbability.clampedLerp(spRewardFraction);

		if (!rnd.testProbability(spRewardProbability)) {
			continue;
		}
		for (auto tile : kv.Value) {
			const auto regions = tile->tilePlacement().filterRegions(regionpredicates::isLoot());
			if (regions.empty()) {
				continue;
			}

			const auto spItemProbability = mConfig.strayPathChestItemProbability.clampedLerp(spRewardFraction);
			auto chestTypeProbabilityConfig = rnd.testProbability(spItemProbability) ? GuaranteedItemChestTypeProbabilityConfig : DefaultChestTypeProbabilityConfig;

			if (_placeChest(*tile, *Util::randomChoice(regions, &rnd), rnd, chestTypeProbabilityConfig)) {
				break; // Succeeded
			}
		}
	}
}

void LootSpawner::_placeChest(TilePreparationState state) {
	const auto regionChestProbability = (DungeonsQA::CVAllLootChests.GetValueOnGameThread() > 0) ? 1 :
		mGame.affectors().GetChestProbabilityMultiplier() * mConfig.regionChestProbability;

	for (const auto& region : state.tile.tilePlacement().filterRegions(regionpredicates::isLoot())) {
		if (_canSpawnAt(region) && state.rnd.testProbability(regionChestProbability)) {
			_placeChest(state.tile, region, state.rnd, DefaultChestTypeProbabilityConfig);
		}
	}
}

QuadrantAngle calculateChestRotation(TileRef& tile, const BlockCuboid& area, Vec3 pos) {
	QuadrantAngle outwardFace = quadrantFromFacing(objective::outwardFace(pos, area));

	const auto doors = tile.tilePlacement().doors();
	if (doors.size() == 1) {
		const auto directionDelta = doors.front().position() - pos;
		const auto directionToDoor = quadrantFromDelta(directionDelta.x, directionDelta.z);
		if (directionToDoor == outwardFace) {
			return directionToDoor;
		}
	}
	return outwardFace + QuadrantAngle::D180;
}

std::vector<QuadrantAngle> getPossibleOrientations(const BlockCuboid& area, int sizeMin) {
	const auto delta = area.maxExclusive - area.minInclusive;
	
	if (delta.x < delta.z) {
		if (delta.x < sizeMin) {
			return { QuadrantAngle::D90, QuadrantAngle::D270 };
		}
	} else if (delta.x > delta.z) {
		if (delta.z < sizeMin) {
			return { QuadrantAngle::D0, QuadrantAngle::D180 };
		}
	}

	return { QuadrantAngle::D0, QuadrantAngle::D90, QuadrantAngle::D180, QuadrantAngle::D270 };
}

QuadrantAngle getBestOrientation(TileRef tile, const BlockCuboid& area, const std::vector<QuadrantAngle>& angles) {
	const auto areaCenter = (area.maxExclusive + area.minInclusive) / 2;

	const auto doors = tile.tilePlacement().doors();
	if (doors.size() == 1) {
		const auto doorFrame = doors[0].doorFrame();
		const auto doorCenter = (doorFrame.first + doorFrame.second) / 2;

		const auto distanceToDoor = [&](QuadrantAngle angle) {
			switch (angle) {
			case QuadrantAngle::D0: return doorCenter.x - areaCenter.x;
			case QuadrantAngle::D90: return areaCenter.z - doorCenter.z;
			case QuadrantAngle::D180: return areaCenter.x - doorCenter.x;
			default: return doorCenter.z - areaCenter.z;
			}
		};

		return *std::max_element(angles.begin(), angles.end(), [&](auto a, auto b) { return distanceToDoor(a) < distanceToDoor(b); });
	} else {
		const auto tileArea = tile.bounds();
		const auto tileCenter = (tileArea.maxExclusive + tileArea.minInclusive) / 2;

		const auto distanceToTile = [&](QuadrantAngle angle) {
			switch (angle) {
			case QuadrantAngle::D0: return tileCenter.x - areaCenter.x;
			case QuadrantAngle::D90: return areaCenter.z - tileCenter.z;
			case QuadrantAngle::D180: return areaCenter.x - tileCenter.x;
			default: return tileCenter.z - areaCenter.z;
			}
		};

		return *std::max_element(angles.begin(), angles.end(), [&](auto a, auto b) { return distanceToTile(a) < distanceToTile(b); });
	}
}

BlockPos getRandomPosition(const BlockCuboid& area, QuadrantAngle angle, Random& rnd, int margin) {
	const auto delta = area.maxExclusive - area.minInclusive;
	const BlockPos allowedMargin {
		FMath::Min(margin, delta.x - 1),
		0,
		FMath::Min(margin, delta.z - 1)
	};

	const auto paddedArea = [&]() -> BlockCuboid {
		switch (angle) {
		case QuadrantAngle::D0: return { area.minInclusive, area.maxExclusive - BlockPos { allowedMargin.x, 0, 0 } };
		case QuadrantAngle::D90: return { area.minInclusive + BlockPos { 0, 0, allowedMargin.z }, area.maxExclusive };
		case QuadrantAngle::D180: return { area.minInclusive + BlockPos { allowedMargin.x, 0, 0 }, area.maxExclusive };
		default: return { area.minInclusive, area.maxExclusive - BlockPos { 0, 0, allowedMargin.z } };
		}
	}();

	return mobspawn::randomPos(paddedArea, rnd);
}

bool LootSpawner::_placeChest(TileRef tile, const BlockCuboid& area, Random& rnd, const chest::ChestTypeProbabilityConfig& config) {
	const int launchDistanceMax { 3 };
	const auto possibleOrientations = getPossibleOrientations(area, launchDistanceMax);
	const auto bestOrientation = getBestOrientation(tile, area, possibleOrientations);
	const auto position = getRandomPosition(area, bestOrientation, rnd, launchDistanceMax).center();
	const auto ueDegrees = conversion::degreesToUe(quadrantToDegrees(bestOrientation));

	const auto chestType = selectRandomizedChestFor(config);
	if (objective::placeObject(mGame.world(), chestType.mBlueprintPath, position, ueDegrees)) {
		mChestRegionHashes.Add(regionHasher(area));
		return true;
	}
	return false;
}

//
// Loot
//
void LootSpawner::_spawnLoot(TilePreparationState state) const {
	for (const auto& region : state.tile.tilePlacement().filterRegions(regionpredicates::isLoot())) {
		if (_canSpawnAt(region) && state.rnd.testProbability(mConfig.regionLootProbability)) {
			_spawnLoot(conversion::posToUe(mobspawn::randomPos(region, state.rnd, 0.5f)), state.rnd);
		}
	}
}

void LootSpawner::_spawnLoot(FVector position, Random& rnd) const {
	FSoftObjectPath SoftPath = FString(TEXT("/Game/Decor/Prefabs/_Urns/LootUrnsBlueprints/BP_LootUrn_Red.BP_LootUrn_Red_C"));
	
	TWeakObjectPtr<UWorld> WeakWorld(&mGame.world());
	
	UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath,
		[SoftPath, WeakWorld, position]() {

		UClass* pClass = Cast<UClass>(SoftPath.ResolveObject());
		if (pClass && WeakWorld.IsValid())
		{
			objective::placeObject(*WeakWorld, *pClass, conversion::ueToPos(position), 0);
		}
	}
	, FStreamableManager::AsyncLoadHighPriority);

}

bool LootSpawner::_canSpawnAt(const lovika::Region& region) const {
	return !mChestRegionHashes.Contains(regionHasher(region));
}

}}
