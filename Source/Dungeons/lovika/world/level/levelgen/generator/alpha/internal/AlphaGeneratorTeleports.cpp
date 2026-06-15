#include "Dungeons.h"
#include "AlphaGeneratorTeleports.h"
#include "AlphaGeneratorTypes.h"
#include "AlphaGeneratorHelpers.h"
#include "game/level/GameTiles.h"
#include "game/level/doors/DoorUtil.h"
#include "lovika/tile/TilePlacement.h"
#include "lovika/world/level/levelgen/LevelGen.h"
#include "lovika/world/level/levelgen/generator/GraphUtil.h"
#include "lovika/world/level/levelgen/generator/Generator.h"
#include "lovika/world/level/levelgen/generator/GeneratorUtil.h"
#include "lovika/world/level/levelgen/generator/GeneratorGraphUtil.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/SplitGroups.h"

namespace generator { namespace alpha {

//
// TeleportDoorFinder
//
bool TeleportDoorFilter(game::TileRef tile, const std::string& target, const DoorDef& door) {
	if (!tile.tilePlacement().isPotentialTeleportDoor(door)) {
		return false;
	}
	return TargetLoc::matches(Util::toLower(target), Util::toLower(door.name()));
}

TeleportDoorFinder createTeleportDoorFinder(std::vector<game::TilePtr> tiles) {
	return TeleportDoorFinder(std::move(tiles), RETLAMBDA(it.tilePlacement().doors()), TeleportDoorFilter);
}

DoorDef TeleportDoorDef::merged() const {
	return game::door::withPrefab(door, def.object);
}

TArray<TeleportDoorDef> getTeleportDoors(const TilePlacement& tilePlacement, const std::vector<io::Teleport>& teleports, bool allowUnlistedTeleports) {
	TArray<TeleportDoorDef> out;
	for (auto& door : tilePlacement.doors()) {
		if (!tilePlacement.isPotentialTeleportDoor(door)) {
			continue;
		}
		const auto lowerDoorName = Util::toLower(door.name());
		if (const auto index = algo::index_of_if(teleports, RETLAMBDA(TargetLoc::matches(Util::toLower(it.door), lowerDoorName)))) {
			out.Add({ teleports[index.GetValue()], door });
		} else if (allowUnlistedTeleports) {
			out.Add({ io::Teleport{}, door });
		}
	}
	return out;
}

//
// Functions
//
TOptional<TeleportDoorDef> findTeleportDoor(const GenState& state, const TeleportDoorFinder& teleportFinder, const io::RegionLocator& loc) {
	const auto loc2 = TargetLoc::fromRegionLocator(loc);

	for (auto& teleportDoor : algo::random::shuffledCopy(teleportFinder.get(loc), state.rnd)) {
		if (!state.placed.isConnected(teleportDoor)) {
			const auto tileIndex = algo::index_of_if(state.placed, RETLAMBDA(it.bounds().containsXZ(teleportDoor.position()))).GetValue();
			const auto& p = state.placed[tileIndex];
			const auto& teleports = state.env.tileGroup.findById(p.metaIdHACK())->metadata.teleports;
			const auto index = algo::index_of_if(teleports, RETLAMBDA(loc2->region == Util::toLower(it.door)));
			return TeleportDoorDef{ index? teleports[index.GetValue()] : io::Teleport{}, teleportDoor };
		}
	}
	return {};
}

bool stretchHasOffsiteJump(const TileGroup& tileGroup, const io::Stretch& stretch) {
	const auto hasMainPathOffsiteJump = [&tileGroup](const io::WeightedTileId& s) {
		const auto& teleports = tileGroup.findById(s.id)->metadata.teleports;
		return algo::any_of(teleports, RETLAMBDA(it.isJumpToExistingPoint()));
	};
	return algo::any_of(stretch.tiles, hasMainPathOffsiteJump);
}

std::vector<std::vector<io::Stretch>> splitStretchesIntoJumpConnectedGroups(const TileGroup& tileGroup, const std::vector<io::Stretch>& ioStretches) {
	return splitIntoGroups<io::Stretch>(ioStretches, RETLAMBDA(splitAfterIf(stretchHasOffsiteJump(tileGroup, it.second))));
}

Result connectJumpDoorsAndMakeResult(const GenState& state, std::vector<graph::Edge> temporaryJumpEdges) {
	Result result = state.makeResult();
	//I'm not happy over having to do and call this, but...
	const graph::Graph temporaryGraph = graph::util::withEdgesAdded(result.graph, std::move(temporaryJumpEdges));
	const auto progresses = graph::util::createProgress(temporaryGraph, result.tileInfos);

	const levelgen::LevelDef ld{ "", result.stretches, result.tileInfos, progresses, result.graph,{} };
	game::LevelDef gameLevelDef{ ld, 0 };
	gameLevelDef.tileAreas.resize(ld.tiles.size(), TileArea{ 0,0,0,{ 100,100 } });

	const game::Tiles gameTiles(gameLevelDef);
	const TeleportDoorFinder teleportFinder = createTeleportDoorFinder(gameTiles.getTiles());

	for (size_t i = 0; i < result.tileInfos.size(); ++i) {
		const auto& tile = result.tileInfos[i];
		const bool isMainPath = progresses[i].strayPathIndex == 0;

		for (auto& teleport : tile.metaTile.metadata.teleports) {
			if (!teleport.isJumpToExistingPoint()) {
				continue;
			}
			const auto srcDoors = getTeleportDoors(tile.placeResult.tilePlacement, { teleport }, false);
			const auto maybeDstDoor = findTeleportDoor(state, teleportFinder, teleport.target.GetValue());

			// It was too big of a disconnect/easy to miss things when doing the validation in another function, so I'm doing it inline now.
			const auto tileInfo = "For teleport def in tile id: " + tile.metaTile.id + ". ";
			// @note: this is way too internal knowledge, but it's ok for now since the error output is valuable
			const auto dstDoorCount = teleportFinder.get(teleport.target.GetValue()).size();
			Validation issues = validationErrorsIf({
				{ srcDoors.Num() == 0, tileInfo + "Teleport entry door not found: " + teleport.door },
				{ srcDoors.Num() >= 2, tileInfo + "Multiple(" + std::to_string(srcDoors.Num()) + ") teleport entry doors found: " + teleport.door },
				{ srcDoors.Num() == 1 && state.placed.isConnected(srcDoors[0].door), tileInfo + "Teleport entry door is already taken: " + teleport.door },
				{ !maybeDstDoor && dstDoorCount == 0, tileInfo + "Teleport exit door not found: " + teleport.target.GetValue() },
				{ !maybeDstDoor && dstDoorCount, tileInfo + "Teleport exit doors found but all(" + std::to_string(dstDoorCount) + ") are already taken: " + teleport.target.GetValue() },
				{ maybeDstDoor && state.placed.isConnected(maybeDstDoor->door), tileInfo + "Teleport exit door is already taken-2: " + teleport.target.GetValue() },
			});
			if (issues.isSuccess()) {
				graph::Edge edge{
					{ i, srcDoors[0].merged() },
					{ util::findIntersectingIndex(result.tileInfos, maybeDstDoor->door.position()).GetValue(), maybeDstDoor->merged() }
				};
				result.graph = graph::util::withEdgesAdded(result.graph, { edge });
			} else if (isMainPath) {
				return Result({ issues });
			}
		}
	}
	return result;
}

}}
