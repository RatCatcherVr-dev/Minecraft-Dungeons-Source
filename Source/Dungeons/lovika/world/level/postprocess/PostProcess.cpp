#include "Dungeons.h"
#include "PostProcess.h"
#include "PostProcessConfigs.h"
#include "DoorFiller.h"
#include "OutsideFiller.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/world/level/levelgen/LevelDef.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "util/Algo.h"

namespace postprocess {

struct DungeonPlacements {
	int dungeonIndex;
	PlacementVector placements;
};

std::vector<DungeonPlacements> groupByDungeon(const levelgen::LevelDef& levelDef, const PlacementVector& placements) {
	std::map<int, PlacementVector> dungeonInstancePlacements;
	std::unordered_map<int, int> dungeonInstanceDungeonIndex;

	int highestDungeonInstanceId = -1;
	for (auto i = 0; i < static_cast<int>(placements.size()); ++i) {
		const auto& stretchId = levelDef.tiles[i].stretchId;
		const int dungeonInstanceId = stretchId.dungeonInstance;
		ensureMsgf(dungeonInstanceId >= highestDungeonInstanceId, TEXT("Tiles aren't grouped by dungeons! Tell Aron"));
		highestDungeonInstanceId = std::max(highestDungeonInstanceId, dungeonInstanceId);
		dungeonInstancePlacements[dungeonInstanceId].push_back(placements[i]);
		dungeonInstanceDungeonIndex[dungeonInstanceId] = levelDef.stretches[stretchId.index].def.dungeonIndex;
	}
	return algo::map_vector(dungeonInstancePlacements, RETLAMBDA((DungeonPlacements{ dungeonInstanceDungeonIndex[it.first], it.second })));
}

Result run(BlockSource& region, const levelgen::LevelDef& levelDef, const Config& config) {
	const PlacedTiles placedTiles(generator::tilesToPlaceResults(levelDef.tiles));

	const auto getFillConfig = [&](const TOptional<Config>& localConfig) {
		return algo::opt::get_or_compute(localConfig, [&] {
			return Config(config.outsideBlockProvider, config.doorBlockProvider ? config.doorBlockProvider : door::providers::RandomDoorFrameBlock());
		});
	};

	Result out;
	for (const auto& dungeonPlacements : groupByDungeon(levelDef, placedTiles.placements())) {
		const auto& dungeonDef = levelDef.data.dungeons[dungeonPlacements.dungeonIndex];
		const auto& fillConfig = getFillConfig(dungeonDef.fillConfig);

		// Doors needs to be handled first, since outside-level-fill sees these filled doors as proper walls
		algo::append_all(out.filledDoors, door::fillUnconnectedDoors(region, placedTiles, dungeonPlacements.placements, fillConfig.doorBlockProvider));
		algo::append_all(out.tileAreas, worldfill::fill(region, placedTiles, { dungeonPlacements.placements }, fillConfig.outsideBlockProvider));
	}
	return out;
}

}
