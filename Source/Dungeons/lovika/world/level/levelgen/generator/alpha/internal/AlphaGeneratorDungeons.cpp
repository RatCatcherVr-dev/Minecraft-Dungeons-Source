#include "Dungeons.h"
#include "AlphaGeneratorDungeons.h"
#include "AlphaGeneratorHelpers.h"
#include "AlphaGeneratorTypes.h"
#include "AlphaGeneratorTeleports.h"
#include "game/level/doors/DoorUtil.h"
#include "game/objective/TargetFinder.h"
#include "lovika/world/level/LevelGenRandom.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "util/Algo.h"
#include "util/FloatWeighedRandom.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "util/StringUtil.h"

namespace generator { namespace alpha {

static const io::Dungeon* selectDungeon(const std::vector<io::Dungeon>& dungeons, const DungeonSelector& dungeonSelector, const io::Teleport& teleport, LevelGenRandom& rnd) {
	if (const auto definedId = FloatWeighedRandom::getRandomItem(&rnd, teleport.dungeons.GetValue())) {
		const auto selectedId = dungeonSelector(DungeonSelectorState{ *definedId, teleport, rnd }).Get(*definedId);
		return util::getDungeonFromId(dungeons, selectedId);
	}
	return nullptr;
}

static std::string getDoorNameFromNameOrLocator(const TOptional<std::string>& nameOrLocator) {
	// @todo: doesn't support a real locator for now -- only the current tile is evaluated
	const auto s = nameOrLocator.Get("");

	if (const auto loc = TargetLoc::fromRegionLocator(s)) {
		return loc->region;
	}
	return s;
}

static TOptional<DoorDef> selectEntranceInDungeon(TArray<TeleportDoorDef> teleportDoors, TOptional<std::string> name, LevelGenRandom& rnd) {
	if (teleportDoors.Num() == 0) {
		return {};
	}
	algo::random::shuffle(teleportDoors, rnd);
	const auto lowerName = Util::toLower(getDoorNameFromNameOrLocator(name));
	const int index = name ? algo::index_of_if(teleportDoors, RETLAMBDA(it.door.nameMatches(lowerName))).Get(0) : 0; // Since it's shuffled, element at index 0 is random
	return teleportDoors[index].merged();
}

static TArray<TeleportDoorDef> getTeleportDoors(const TileGroup& tileGroup, const TilePlacement& tp, bool allowUnlistedTeleports) {
	return getTeleportDoors(tp, tileGroup.findById(tp.metaIdHACK())->metadata.teleports, allowUnlistedTeleports);
}

static TOptional<TeleportDoorDef> getRandomUnconnectedTeleportDoor(const PlacedTiles& placed, const TilePlacement& tp, const io::Teleport& teleport, LevelGenRandom& rnd) {
	const auto all = getTeleportDoors(tp, { teleport }, false);
	const auto unconnected = algo::copy_if(all, RETLAMBDA(!placed.isConnected(it.door)));
	return Util::randomChoiceOrEmpty(unconnected, &rnd);
}

static FString approximateTeleportDoorDefName(const TeleportDoorDef& doorDef) {
	return stringutil::toFString('(' + doorDef.def.door + " : " + doorDef.door.name() + ')');
}

bool generateDungeon(GenState& state, const TeleportDoorDef& enterTeleportDoor, const std::vector<io::Stretch>& stretches) {
	const auto result = generateSafe(state.env, stretches, state.rnd.getSeed() + state.placed.size(), 50, 500); //@seed
	if (result.success) {
		const auto& exitTeleportDoors = getTeleportDoors(state.env.tileGroup, result.success->placed[0], true);
		if (const auto& entrance = selectEntranceInDungeon(exitTeleportDoors, enterTeleportDoor.def.target, state.rnd)) {
			const DoorPair doors{ enterTeleportDoor.merged(),  entrance.GetValue() };
			state.placeOffsite(doors, *result.success);
			return true;
		}
		UE_LOG(LogLevelGeneration, Error, TEXT("Couldn't find dungeon entrance for tile '%s'. Wanted door name: '%s'. Found doors: {%s}."),
			*stringutil::toFString(result.success->placed[0].metaIdHACK()),
			*stringutil::toFString(enterTeleportDoor.def.target.Get("*")),
			*FString::JoinBy(exitTeleportDoors, TEXT(", "), approximateTeleportDoorDefName)
		);
	}
	if (result.failed) {
		append(state.issues, generateValidationIssuesFor(state.env, result.failed.GetValue()));
	}
	return false;
}

static void handleDungeonStrayPaths(GenState& state, size_t i) {
	// @attn: These are not (const) references as the collections may be re-allocated
	const auto tilePlacement = state.placed[i];
	const auto dungeonSelector = state.stretchDefForTileIndex(i)->dungeonSelector;

	const auto& metaTile = state.env.tileGroup.findById(tilePlacement.metaIdHACK());
	const auto teleports = metaTile->metadata.teleports;
	const auto teleportIndices = Util::randomIndices(teleports.size(), &state.rnd);

	for (auto j : teleportIndices) {
		const auto& teleport = teleports[j];

		if (!teleport.isJumpToNewDungeon()) {
			continue;
		}
		const auto teleportDoor = getRandomUnconnectedTeleportDoor(state.placed, tilePlacement, teleport, state.rnd);
		if (!teleportDoor) {
			continue;
		}
		if (state.placed.isConnected(teleportDoor->door)) {
			ensure(false); // "ensureNoEntry()";
			continue;
		}
		const auto dungeon = selectDungeon(state.env.dungeons, dungeonSelector, teleport, state.rnd);
		if (!dungeon) {
			continue;
		}
		generateDungeon(state, teleportDoor.GetValue(), dungeon->stretches);
	}
}

void generateDungeons(GenState& state) {
	if (state.env.dungeons.empty()) {
		return;
	}
	const int originalTilesCount = state.placed.size();
	for (int i = 0; i < originalTilesCount; ++i) {
		handleDungeonStrayPaths(state, i);
	}
}


void generateDungeonsBatchHACK(GenState& state) {
	const auto* initialStretchData = state.stretchDefForTileIndex(0);
	const auto dungeonBatchSelector = initialStretchData ? initialStretchData->dungeonBatchSelector : DungeonBatchSelector{};

	if (state.env.dungeons.empty() || !dungeonBatchSelector) {
		return;
	}

	const TArray<TeleportDoorDef> allTeleportDoors = algo::flatmap(state.placed, [&](const TilePlacement& tilePlacement) {
		const auto& teleportDoorsDef = getTeleportDoors(state.env.tileGroup, tilePlacement, false);
		return algo::copy_if(teleportDoorsDef, RETLAMBDA(it.def.isJumpToNewDungeon() && !state.placed.isConnected(it.door)));
	});

	for (auto indexId : dungeonBatchSelector({ allTeleportDoors, state.rnd })) {
		if (const auto* dungeon = util::getDungeonFromId(state.env.dungeons, indexId.second)) {
			generateDungeon(state, allTeleportDoors[indexId.first], dungeon->stretches);
		} else {
			ensureMsgf(false, TEXT("Couldn't find dungeon with id: %s"), *stringutil::toFString(indexId.second.id));
		}
	}
}

}}
