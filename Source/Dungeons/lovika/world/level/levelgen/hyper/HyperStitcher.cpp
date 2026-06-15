#include "Dungeons.h"
#include "HyperStitcher.h"
#include "HyperStitcherUtil.h"
#include "HyperPortal.h"
#include "game/mission/hyper/HyperMissionUtil.h"
#include "lovika/world/level/levelgen/LevelValidation.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "util/Algo.hpp"
#include "util/CollectionUtils.h"
#include "util/Random.h"
#include "util/RandomUtil.h"
#include "util/StringUtil.h"

namespace levelgen { namespace hajper {

template <typename Collection>
Collection weightedSample(const Collection& elements, int wantedCount, Random& rnd) {
	const auto pickedWeightedIndices = [&] {
		const auto weights = algo::map_tarray(elements, RETLAMBDA(it.getWeight()));
		const auto allWeightedIndices = Util::weightedIndexShuffle(weights, &rnd);
		const auto maxCount = static_cast<int>(elements.size());
		return Util::subsequence(allWeightedIndices, 0, FMath::Min(maxCount, wantedCount));
	}();
	return algo::map_as<Collection>(pickedWeightedIndices, RETLAMBDA(elements[it]));
}

StitchInput generateRandomStitchInput(int subMissionCount, RandomSeed seed, const io::HyperLevelDef& hyperLevel, int startAtIndex) {
	if (startAtIndex >= subMissionCount) {
		return {};
	}
	// weightedSample only handles "pick N elements" and is _sorted_ in a weighted
	// fashion. This shuffle makes sure the order of the picked missions is random.
	auto rnd = std::make_unique<Random>(seed);
	const auto levelIds = weightedSample(hyperLevel.levelIds, subMissionCount, *rnd);
	return StitchInput {
		Util::subsequence(
			algo::map_tarray(algo::random::shuffledCopy(levelIds, *rnd), RETLAMBDA(stringutil::toFString(it.id))),
			startAtIndex),
		algo::map_tarray(hyperLevel.definitionLevelIds, RETLAMBDA(stringutil::toFString(it.id)))
	};
}

StitchResult failWith(const std::string& msg) {
	return { {}, validationErrors({msg}) };
}

bool rewritePortalUnlockKey(io::Tile& tile, const io::Level& currentLevel, const io::Level* prevLevel, const io::Level* nextLevel) {
	const bool isOutgoing = portal::isPortalOfType(tile.metadata.unlockKeys, portal::outgoingPortalKeys());
	const bool isIncoming = !isOutgoing && portal::isPortalOfType(tile.metadata.unlockKeys, portal::incomingPortalKeys());

	if (isOutgoing || isIncoming) {
		const auto portalKey = isOutgoing ?
			portal::getPortalKey(portal::outgoingPortalKeys(), currentLevel, nextLevel) :
			portal::getPortalKey(portal::incomingPortalKeys(), currentLevel, prevLevel);

		if (portal::isPortalWithKey(tile.metadata.unlockKeys, portalKey)) {
			if (portalKey == portal::outgoingPortalKeys().none()) {
				tile.metadata.isGoal = true;
			}
			tile.metadata.unlockKeys = {};
		} else {
			tile.metadata.unlockKeys = { "unused_hypermissiontile" };
			return false;
		}
	}
	return true;
}

void rewritePortalDungeonForNextLevel(io::Tile& tile, const io::Level* nextLevel, bool isTileEnabled) {
	for (auto& teleport : tile.metadata.teleports) {
		if (!teleport.hasDungeonId(ids::HyperLevel())) {
			continue;
		}
		ensure(teleport.dungeons.Get({}).size() == 1);
		ensure(nextLevel || !isTileEnabled);

		if (nextLevel) {
			teleport.dungeons = nextLevel->initialDungeons;
		} else {
			teleport.dungeons.Reset();
		}
	}
}

io::Tile* getTile(std::vector<io::Tile>& tileDefs, const CaseInsensitiveId& id) {
	if (const auto i = algo::index_of(tileDefs, id)) {
		return &tileDefs[i.GetValue()];
	}
	return nullptr;
}

std::vector<io::WeightedTileId> collectTileIdsForDungeons(const std::vector<const io::Dungeon*>& dungeons) {
	std::vector<io::WeightedTileId> out;
	for (const auto& dungeon : dungeons) {
		for (const auto& stretch : dungeon->stretches) {
			algo::append_all(out, stretch.tiles);
		}
	}
	return algo::unique_by_less_of(out, RETLAMBDA(it.lowerId));
}

std::vector<const io::Dungeon*> getTileDungeons(const io::Level& level, const io::Tile& tile) {
	std::unordered_set<const io::Dungeon*> uniqueDungeons;

	for (auto& teleport : tile.metadata.teleports) {
		algo::map_if(teleport.dungeons.Get({}),
			RETLAMBDA(generator::util::getDungeonFromId(level.dungeons, it)),
			RETLAMBDA(it),
			uniqueDungeons);
	}
	return algo::map_cast<std::vector<const io::Dungeon*>>(uniqueDungeons);
}

void addTilesReachableFromDungeons(
	const io::Level& level,
	const std::vector<const io::Dungeon*>& dungeons,
	std::unordered_set<const io::Dungeon*>& visitedDungeons,
	std::vector<io::Tile>& tileRepository,
	std::unordered_set<std::string>& collectedReachableLowerTileIds)
{
	for (auto* dungeon : dungeons) {
		if (visitedDungeons.insert(dungeon).second) {
			for (auto& dungeonTileId : collectTileIdsForDungeons({ dungeon })) {
				if (collectedReachableLowerTileIds.insert(dungeonTileId.lowerId).second) {
					if (auto* metaTile = getTile(tileRepository, dungeonTileId)) {
						addTilesReachableFromDungeons(level, getTileDungeons(level, *metaTile), visitedDungeons, tileRepository, collectedReachableLowerTileIds);
					}
				}
			}
		}
	}
}

std::vector<io::Tile*> getTilesReachableFromDungeonIds(std::vector<io::Tile>& tileRepository, const io::Level& level, const std::vector<WeightedId>& dungeonIds) {
	std::unordered_set<const io::Dungeon*> visitedDungeons;
	std::unordered_set<std::string> collectedLowerTileIds;
	addTilesReachableFromDungeons(
		level,
		algo::map_vector(dungeonIds, RETLAMBDA(generator::util::getDungeonFromId(level.dungeons, it))),
		visitedDungeons,
		tileRepository,
		collectedLowerTileIds
	);
	return algo::map_if_vector(collectedLowerTileIds,
		RETLAMBDA(getTile(tileRepository, it)),
		RETLAMBDA(it)
	);
}

void handlePortalsInSourceData(std::vector<io::Tile>& tileRepository, const io::Level& currentLevel, const io::Level* prevLevel, const io::Level* nextLevel, int sourceDataIndex) {
	for (auto* tile : getTilesReachableFromDungeonIds(tileRepository, currentLevel, currentLevel.initialDungeons)) {
		const bool isTileEnabled = rewritePortalUnlockKey(*tile, currentLevel, prevLevel, nextLevel);
		rewritePortalDungeonForNextLevel(*tile, nextLevel, isTileEnabled);
	}
}

StitchResult stitch(const StitchInput& in) {
	if (algo::unique_by_less_of(in.orderedLevelIds, RETLAMBDA(it.ToLower())).Num() != in.orderedLevelIds.Num()) {
		return failWith("All mission filenames needs to be unique");
	}

	const auto stitchSourceDatas = algo::map_vector(in.orderedLevelIds, RETLAMBDA(sourcedata::fromFileSystem(it)));
	const auto definitionSourceDatas = algo::map_vector(in.definitionLevelIds, RETLAMBDA(sourcedata::fromFileSystem(it)));

	const auto stitchAndDefinitionSourceDataPointers = [&] {
		std::vector<sourcedata::SourceData*> sourceDatas;
		algo::map_to(stitchSourceDatas, RETLAMBDA(it.get()), sourceDatas);
		algo::map_to(definitionSourceDatas, RETLAMBDA(it.get()), sourceDatas);
		return sourceDatas;
	}();
	const auto isStitchData = [&stitchSourceDatas](const sourcedata::SourceData& data) {
		return algo::any_of(stitchSourceDatas, RETLAMBDA(it.get() == &data));
	};

	if (algo::contains(stitchAndDefinitionSourceDataPointers, nullptr)) {
		return failWith("Couldn't read all json files for all hyper-missions");
	}

	for (const auto* sourceData : stitchAndDefinitionSourceDataPointers) {
		const Validation levelValidation = levelgen::validate(*sourceData);
		printValidation(levelValidation, stringutil::toFString("Hyper-mission level validation " + sourceData->level.id + ": "));
		if (isStitchData(*sourceData) && !levelValidation.isSuccess()) {
			return failWith("Failed validating hyper-mission level: " + sourceData->level.id);
		}
	}

	io::Level level;
	sourcedata::Prefabs prefabs;

	std::vector<io::Tile> tileDefs = algo::unique_by_less_of(
		algo::flatmap(stitchAndDefinitionSourceDataPointers, RETLAMBDA(it->level.tileDefs)),
		RETLAMBDA(it.lowerId));

	for (int i = 0; i < stitchAndDefinitionSourceDataPointers.size(); ++i) {
		auto& currentData = *stitchAndDefinitionSourceDataPointers[i];
		const bool isPlayedLevel = isStitchData(*stitchAndDefinitionSourceDataPointers[i]); // the opposite is "definition level"

		if (isPlayedLevel) {
			handlePortalsInSourceData(tileDefs, currentData.level,
				Util::isValidIndex(stitchSourceDatas, i - 1) ? &stitchSourceDatas[i - 1]->level : nullptr,
				Util::isValidIndex(stitchSourceDatas, i + 1) ? &stitchSourceDatas[i + 1]->level : nullptr, i);
		}
		if (i == 0) {
			// Temporarily copying all values from the first level
			// - resourcePack needs to be fixed
			// - rest (see older commit to see what values are copied) might be moved into dungeons structures, or read from the "hyper" file
			level = currentData.level;
		}
		else {
			appendLevel(level, currentData.level, isPlayedLevel);
		}

		sourcedata::Prefabs sourceDataPrefabs = currentData.stealPrefabs();
		prefabs.tileSet.feed(std::move(sourceDataPrefabs.tileSet));
		prefabs.propSet.feed(std::move(sourceDataPrefabs.propSet));
	}

	// As we might overwrite some tile-defs during the above
	// loop, we make sure the new values are used instead.
	level.tileDefs = tileDefs;
	return { std::make_unique<sourcedata::SourceData>(std::move(prefabs), std::move(level)) };
}

}}
