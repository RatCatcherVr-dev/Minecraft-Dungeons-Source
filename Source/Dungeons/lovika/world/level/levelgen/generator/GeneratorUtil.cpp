#include "Dungeons.h"
#include "GeneratorUtil.h"
#include "VolumesIntersector.h"
#include "lovika/BlockRegionTransform.h"
#include "lovika/BlockPosTransform.h"
#include "lovika/tile/PlacedTiles.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "util/Algo.hpp"
#include "util/FloatWeighedRandom.h"

namespace generator { namespace util {

//
// Generator Tile util
//
BlockCuboid bounds(const std::vector<Tile>& tiles) {
	BlockCuboid bounds;
	for (auto&& tile : tiles) {
		safeExpand(bounds, tile.placeResult.tilePlacement.bounds());
	}
	return bounds;
}

TOptional<size_t> findIntersectingIndex(const std::vector<Tile>& tiles, BlockPos pos) {
	for (size_t i = 0; i < tiles.size(); ++i) {
		if (tiles[i].placeResult.tilePlacement.bounds().containsXZ(pos)) {
			return i;
		}
	}
	return {};
}

const io::Dungeon* getDungeonFromId(const std::vector<io::Dungeon>& dungeons, const CaseInsensitiveId& id) {
	if (const auto index = algo::index_of_if(dungeons, RETLAMBDA(it.id == id))) {
		return &dungeons[index.GetValue()];
	}
	return nullptr;
}

const io::Dungeon& initialDungeon(const io::Level& level, LevelGenRandom& rnd){
	if (auto* id = FloatWeighedRandom::getRandomItem(&rnd, level.initialDungeons)) {
		if (auto* dungeon = getDungeonFromId(level.dungeons, *id)) {
			return *dungeon;
		}
	}
	return level.dungeons.front();
}

//
// Placement util
//
std::vector<DoorPair> transformed(const std::vector<DoorPair>& doors, const TilePlacement& tp) {
	std::vector<DoorPair> out; out.reserve(doors.size());
	std::transform(doors.begin(), doors.end(), std::back_inserter(out), [transform = tp.blockPosTransform()](const DoorPair& it) {
		return DoorPair{ transformed(it.from, transform), transformed(it.to, transform) };
	});
	return out;
}

std::vector<DoorDef> initialDoors(const MetaTile& metaTile) {
	if (!metaTile.metadata.entryDoor.empty()) {
		const auto& doors = metaTile.tile().doors();
		const auto it = std::find_if(doors.begin(), doors.end(), RETLAMBDA(it.nameMatches(metaTile.metadata.entryDoor)));
		if (it != doors.end()) {
			return { *it };
		}
	}
	if (metaTile.tile().doors().size() >= 2) {
		return nonExitDoors(metaTile, metaTile.tile().doors());
	}
	return {};
}

PlaceResultVector initialPlacements(const MetaTile& metaTile) {
	// entryDoors here means something like: what doors (if any) could we
	// have come through if this wasn't the first tile (e.g. by teleport)
	const auto entryDoors = algo::map_vector(initialDoors(metaTile), RETLAMBDA((DoorPair{it, it})));

	PlaceResultVector placements;
	for (auto&& rotation : metaTile.metadata.rotations.get()) {
		const TilePlacement tilePlacement(metaTile, Placement{ BlockPos::ZERO, rotation });
		if (entryDoors.empty()) {
			placements.push_back({ tilePlacement });
		} else {
			for (const auto& door : entryDoors) {
				placements.push_back({ tilePlacement, transformed({door}, tilePlacement) });
			}
		}
	}
	return placements;
}

DoorVector freeDoors(const DoorVector& doors, const PlacedTiles& doorChecker) {
	return algo::copy_if(doors, RETLAMBDA(!doorChecker.isConnected(it)));
}

PlaceResultVector neighbourPlacements(const DoorVector& exitDoorCandidates, const MetaTile& metaTile, const DoorVector& entryDoorCandidates, int maxWidthDifference, const PlacedTiles& doorChecker) {
	const auto& metadata = metaTile.metadata;

	TOptional<DoorVector> freeExitDoors;

	PlaceResultVector placements;
	// @todo: Cartesian product?
	for (const auto& neighDoor : entryDoorCandidates) {
		if (!metadata.entryDoor.empty() && !neighDoor.nameMatches(metadata.entryDoor)) {
			continue;
		}
		if (!freeExitDoors) {
			freeExitDoors = freeDoors(exitDoorCandidates, doorChecker);
		}
		for (const auto& door : freeExitDoors.GetValue()) {
			if (auto placement = canBePlaced(door, neighDoor, metaTile, maxWidthDifference)) {
				placements.push_back(std::move(placement.GetValue()));
			}
		}
	}
	return placements;
}

PlaceResultVector removeIntersecting(const VolumesIntersector& intersector, PlaceResultVector candidates) {
	if (!intersector.isEmpty()) {
		candidates.erase(std::remove_if(begin(candidates), end(candidates), RETLAMBDA(intersector(it.tilePlacement.bounds()))), end(candidates));
	}
	return candidates;
}

TOptional<PlaceResult> canBePlaced(const DoorDef& base, const DoorDef& neighDoor, const MetaTile& metaTile, int maxWidthDifference/* = 9999*/) {
	if (std::abs(base.width() - neighDoor.width()) > maxWidthDifference) {
		return {};
	}
	const auto angle = angleBetween(neighDoor.facing(), base.neighbourFacing());
	if (!metaTile.metadata.rotations.has(angle)) {
		return {};
	}
	const BlockPos tmp = blockpostransform::create(metaTile.tile().size(), BlockRegionTransform::GetRotate(angle))(neighDoor.position());
	TilePlacement transformedNeighbourTile(metaTile, { base.neighbourPos() - tmp, angle });
	DoorDef transformedNeighbourDoor = ::transformed(neighDoor, transformedNeighbourTile.blockPosTransform());
	return PlaceResult{ transformedNeighbourTile, {DoorPair{base, transformedNeighbourDoor}} };
}

PlaceResultVector filterIfSome(const PlaceResultVector& original, const Pred<const TilePlacement&>& predicate) {
	return filterIfSome(original, [&](const PlaceResult& pr) { return predicate(pr.tilePlacement); });
}

PlaceResultVector filterIfSome(const PlaceResultVector& original, const Pred<const PlaceResult&>& predicate) {
	PlaceResultVector filtered;
	std::copy_if(original.begin(), original.end(), std::back_inserter(filtered), predicate);
	return filtered.empty() ? original : filtered;
}

bool matchesAny(const DoorDef& door, const std::vector<std::string>& names) {
	return algo::any_of(names, RETLAMBDA(door.nameMatches(it)));
}

bool isExitDoorFor(const DoorDef& door, const MetaTile& metaTile) {
	return matchesAny(door, metaTile.metadata.exitDoors);
}

DoorVector exitDoorCandidates(const MetaTile& metaTile, const DoorVector& doors) {
	if (metaTile.metadata.exitDoors.empty()) {
		return doors;
	}
	return algo::copy_if(doors, RETLAMBDA(isExitDoorFor(it, metaTile)));
}

DoorVector exitDoorCandidates(const TileGroup& tileGroup, const TilePlacement& tilePlacement) {
	if (auto metaTile = tileGroup.findById(tilePlacement.metaIdHACK())) {
		return util::exitDoorCandidates(*metaTile, tilePlacement.doors());
	}
	return tilePlacement.doors();
}

DoorVector nonExitDoors(const MetaTile& metaTile, const DoorVector& doors) {
	return algo::copy_if(doors, RETLAMBDA(!isExitDoorFor(it, metaTile)));
}

}}
