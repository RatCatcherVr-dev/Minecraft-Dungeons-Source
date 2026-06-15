#include "Dungeons.h"
#include "LevelValidation.h"
#include "TileGroup.h"
#include "SourceData.h"
#include "game/levels.h"
#include "game/level/TileDecor.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/alpha/AlphaSpawner.h"
#include "generator/GeneratorUtil.h"
#include "lovika/io/LevelFile.h"
#include "lovika/tile/TilePredicates.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"
#include <tuple>

namespace levelgen {

void testAllPrefabsHaveIds(const std::string& type, const TileGroup& prefabs, Validation& result) {
	for (auto meta : prefabs.tiles()) {
		if (meta.tile().id().empty()) {
			result.warning(std::string("Missing ") + type + " id for prefab with size: " + meta.tile().size().toString());
		}
	}
}

void testAllReferencedChallengesExist(const io::Level& level, Validation& result) {
	for (auto& tileDef : level.tileDefs) {
		if (tileDef.metadata.challengesNotFound.empty()) {
			continue;
		}
		result.error("Couldn't find challenge ids: " + Util::join(tileDef.metadata.challengesNotFound) + " referenced from tile id='" + tileDef.id + '\'');
	}
}

void testAllInitialDungeonsAreUnique(const io::Level& level, Validation& result) {
	for (auto& initialDungeon : level.initialDungeons) {
		const int foundCount = algo::count_if(level.dungeons, RETLAMBDA(it.id == initialDungeon));
		if (foundCount != 1) {
			result.error("[Initial] dungeon '" + initialDungeon.id + "' must exist exactly once. Found " + std::to_string(foundCount) + " times");
		}
	}
}

std::string dungeonInfo(const io::Dungeon& dungeon) {
	return "dungeon(id=" + dungeon.id.id + ", level='" + stringutil::toStdString(dungeon.level.id) + "')";
}

std::string stretchInfo(const io::Dungeon& dungeon, int stretchIndex) {
	auto&& stretch = dungeon.stretches[stretchIndex];
	return "stretch(index=" + std::to_string(stretchIndex) + ", id='" + stretch.id + "'), " + dungeonInfo(dungeon);
}

void validateDungeon(const io::Dungeon& dungeon, Validation& result) {
	if (dungeon.musicLevelOverride && !GetEnumValueFromStringT<ELevelNames>(dungeon.musicLevelOverride.GetValue())) {
		result.warning("Music override isn't a ELevelNames value: " + stringutil::toStdString(dungeon.musicLevelOverride.GetValue()) + " in " + dungeonInfo(dungeon));
	}
}

bool hasDoor(TileRef tile, const std::string& name) {
	if (name.empty()) {
		return true;
	}
	return std::any_of(tile.doors().begin(), tile.doors().end(), [name](auto& door) { return door.nameMatches(name); });
}

TOptional<std::string> probableDoorNotOnEdge(MetaTileRef meta) {
	const auto innerBounds = shrinked(BlockCuboid::fromSize(meta.tile().size()), BlockPos(1, 0, 1));

	for (auto& door : meta.tile().doors()) {
		const bool isProbablyTeleport = !door.name().empty() && !door.nameMatches(meta.metadata.entryDoor) && !generator::util::isExitDoorFor(door, meta); //... let's tag this
		if (isProbablyTeleport) {
			continue;
		}
		if (innerBounds.containsXZ(door.positions())) {
			return "Door(pos=" + door.position().toString() + ", width=" + std::to_string(door.width()) + ')';
		}
	}
	return {};
}

void validateReferencedStretchTile(const TileGroup& tiles, const io::Tile& ioTile, const std::string& referenceId, Validation& result) {
	auto meta = tiles.findById(Util::toLower(ioTile.id));
	if (!meta) {
		result.error("Couldn't find tile: " + ioTile.id + " (referenced in " + referenceId + ')');
		return;
	}
	auto& tile = meta->tile();
	auto desc = " on tile : " + tile.id() + "(referenced in " + referenceId + ')';
	validationIssuesIf(result, {
		std::make_tuple(IssueType::Error, tile.doors().empty(), "No doors found" + desc ),
		std::make_tuple(IssueType::Error, !hasDoor(tile, ioTile.metadata.entryDoor), "Entry door: '" + ioTile.metadata.entryDoor + "' not found" + desc),
		// @todo: exit-door? We can check it in the json validator for now
		std::make_tuple(IssueType::Warning, probableDoorNotOnEdge(*meta).IsSet(), "Door not on edge: '" + probableDoorNotOnEdge(*meta).Get("") + "'" + desc)
	});
}

void validateStretch(const TileGroup& tiles, const TileGroup& props, const io::Level& level, const io::Dungeon& dungeon, int stretchIndex, Validation& result) {
	auto& stretch = dungeon.stretches[stretchIndex];
	const std::string stretchIdString = stretchInfo(dungeon, stretchIndex);
	const std::string stretchString{ " (referenced in " + stretchIdString + ')' };

	if (stretch.tiles.empty()) {
		result.error("Stretch has no valid tiles at all:" + stretchString);
	}
	for (auto&& ioTile : stretch.tiles) {
		validateReferencedStretchTile(tiles, ioTile, stretchIdString, result);
	}
	for (auto&& propGroup : stretch.propGroups) {
		if (props.filter(tilepredicates::hasTag(propGroup.id)).isEmpty()) {
			result.warning("Couldn't find prop group: " + propGroup.id + stretchString);
			continue;
		}
	}
	const io::StrayPathConfig& strayPathConfig = stretch.overrides.strayPath;
	const bool hasProbability = strayPathConfig.probability.Get(1) > 0;
	const bool hasVariants = strayPathConfig.variants && !strayPathConfig.variants->empty();
	//if (hasProbability && !hasVariants) {
	//	result.warning("No stray path variants defined, but probability > 0 " + stretchString);
	//}
	if (hasProbability && hasVariants) {
		for (const auto& variant : strayPathConfig.variants.GetValue()) {
			if (!variant.tiles || variant.tiles->empty()) {
				result.warning("Side-path variant has no explicitly defined tiles! " + stretchString + ". This will be deprecated soon.");
			}
			if (variant.weight <= 0) {
				result.warning("Side-path variant weight of <= 0 makes no sense: " + std::to_string(variant.weight) + ' ' + stretchString);
			}
		}
	}
}

void validateMobType(const io::MobType& type, const std::string& groupId, Validation& result) {
	auto prefix = "MobType (in group id: '" + groupId + "')";
	auto types = game::mobspawn::evaluateMobExpr(type.expr);

	validationIssuesIf(result, {
		std::make_tuple(IssueType::Error, type.expr.empty(), prefix + " has no mob type expression" ),
		std::make_tuple(IssueType::Error, type.max < type.min, prefix + " has max < min" ),
		std::make_tuple(IssueType::Error, types.empty(), prefix + " evaluates to 0 mobs!: " + type.expr )
	});
}

void validateMobGroup(const io::MobGroup& group, Validation& result) {
	validationIssuesIf(result, {
		std::make_tuple(IssueType::Error, group.id.empty(), "Missing MobGroup id" ),
		std::make_tuple(IssueType::Error, group.types.empty(), "MobGroup is missing mob list. Id: " + group.id)
	});
	for (auto&& type : group.types) {
		validateMobType(type, group.id, result);
	}
}

bool folderExists(std::string folder) {
	const auto path = FPaths::ProjectDir() + "Content/" + folder.c_str();
	return FPaths::DirectoryExists(path);
}

void validateHeader(const io::Level& level, Validation& result) {
	const auto folder = "Decor/Generated/" + level.id;
	validationIssuesIf(result, {
		std::make_tuple(IssueType::Error, !folderExists(folder), "Could not find assets for json mission id: \"" + level.id + "\". Looked in folder: " + folder),
	});
}

using Tiles = std::vector<TilePtr>;

static Tiles tilesForIds(const TileGroup& tiles, const std::vector<io::WeightedTileId>& ids) {
	std::unordered_set<TilePtr> out;
	for (auto& t : ids) {
		if (auto meta = tiles.findById(t.id)) {
			out.insert(&meta->tile());
		}
	}
	return{ out.begin(), out.end() };
};

void approximatelyTestAllStretchesCanBeConnected(const io::Level& level, const TileGroup& tiles, Validation& result) {
	using DoorPred = ::Pred<const DoorDef&>;
	using Sizes = std::unordered_set<int>;
	using SizesAndTiles = Pair<Sizes, Tiles>;
	TOptional<SizesAndTiles> lastDoorSizes;

	const auto exitDoorSizesIfAnyDoorMatches = [](const Tiles& tiles, const DoorPred& doorPred) {
		SizesAndTiles out;
		for (auto tile : tiles) {
			if (algo::any_of(tile->doors(), doorPred)) {
				for (auto door : tile->doors()) {
					out.first.insert(door.width());
				}
				if (!tile->doors().empty()) {
					out.second.push_back(tile);
				}
			}
		}
		return out;
	};

	const auto joinStrings = [](const TArray<FString>& strings) -> std::string {
		return TCHAR_TO_UTF8(*FString::Join(strings, TEXT(", ")));
	};
	const auto joinSizes = [&joinStrings](const Sizes& sizes) {
		return joinStrings(Util::map(RETLAMBDA(FString::FromInt(it)), sizes.begin(), sizes.end()));
	};
	const auto joinTiles = [&joinStrings](const Tiles& tiles) {
		return joinStrings(Util::map(RETLAMBDA(FString(it->id().c_str())), tiles.begin(), tiles.end()));
	};

	const DoorPred always = RETLAMBDA(true);
	const DoorPred hasMatchingDoorSize = [&](const DoorDef& door) {
		return lastDoorSizes.GetValue().first.count(door.width()) > 0;
	};

	const auto writeIssues = [&](const io::Dungeon& dungeon, int stretchIndex, int stretchTileIndex, const TOptional<SizesAndTiles>& lastExits) {
		std::string s = "We couldn't match up door sizes in " + stretchInfo(dungeon, stretchIndex) + ", stretch tile#: " + std::to_string(stretchTileIndex) + ". Tiles: ";
		for (auto tile : tilesForIds(tiles, dungeon.stretches[stretchIndex].tiles)) {
			//s += "\n    '" + tile->id() + "', sizes: " + joinSizes(exitDoorSizesIfAnyDoorMatches({ tile }, always).first);
			s += tile->id() + "' sizes: (" + joinSizes(exitDoorSizesIfAnyDoorMatches({ tile }, always).first) + "), ";
		}
		if (lastExits) {
			s += "\n    Didn't match previous exit sizes: (" + joinSizes(lastExits->first) + ") for tiles: " + joinTiles(lastExits->second);
		}
		result.warning(s);
	};

	for (const auto& dungeon : level.dungeons) {
		if (!dungeon.needsToBeValidated) {
			continue;
		}
		lastDoorSizes.Reset();

		// Only check door sizes for now
		for (int si = 0, end = dungeon.stretches.size(); si < end; ++si) {
			auto& stretch = dungeon.stretches[si];
			auto stretchTiles = tilesForIds(tiles, stretch.tiles);

			for (int i = 0, last = stretch.length - 1; i <= last; ++i) {
				auto lastExits = lastDoorSizes;
				lastDoorSizes = exitDoorSizesIfAnyDoorMatches(stretchTiles, lastDoorSizes ? hasMatchingDoorSize : always);

				if (!(si == end - 1 && i == last) && lastDoorSizes->first.empty()) {
					writeIssues(dungeon, si, i, lastExits);
					lastDoorSizes.Reset(); // Restart the search here, so we don't have to re-run game and re-validate for every mismatch
				}
			}
		}
	}
}


Validation validate(const sourcedata::SourceData& data) {
	Validation result;

	validateHeader(data.level, result);
	testAllPrefabsHaveIds("tile", data.tiles, result);
	testAllPrefabsHaveIds("prop", data.props, result);

	testAllReferencedChallengesExist(data.level, result);
	testAllInitialDungeonsAreUnique(data.level, result);

	for (const auto& dungeon : data.level.dungeons) {
		validateDungeon(dungeon, result);
	}

	for (const auto& dungeon : data.level.dungeons) {
		if (dungeon.needsToBeValidated) {
			for (size_t stretchIndex = 0; stretchIndex < dungeon.stretches.size(); ++stretchIndex) {
				validateStretch(data.tiles, data.props, data.level, dungeon, stretchIndex, result);
			}
		}
	}

	approximatelyTestAllStretchesCanBeConnected(data.level, data.tiles, result);

	for (auto&& mobGroup : data.level.mobGroups) {
		validateMobGroup(mobGroup, result);
	}
	return result;
}

}
