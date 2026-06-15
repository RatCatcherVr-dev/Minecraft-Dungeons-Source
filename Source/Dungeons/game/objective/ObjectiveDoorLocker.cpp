#include "Dungeons.h"
#include "ObjectiveDoorLocker.h"
#include "ObjectiveUtil.h"
#include "RegionFinder.h"
#include "game/Game.h"
#include "game/level/GameTiles.h"
#include "game/level/doors/DoorUtil.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "lovika/Region.h"

namespace game { namespace objective {

DoorLocker::DoorLocker(UWorld& world)
	: DoorLocker(world, {})
{
	mIsNull = true;
}

DoorLocker::DoorLocker(UWorld& world, const DoorVector& doors, const FString& prefabPath/*= ""*/)
	: mWorld(world)
	, mIsUnlocked(false)
	, mIsNull(false)
{
	std::set<Pair<size_t, FacingID>> placed;
	for (auto&& door : doors) {
		if (!placed.emplace(door.position().hashCode(), door.facing()).second) {
			continue;
		}
		if (auto doorComponent = game::door::placeDoor(world, door, prefabPath)) {
			mDoorComponents.Add(doorComponent);
		}
	}
	mCouldPlaceAllDoors = (doors.size() == mDoorComponents.Num());
}

DoorLocker::~DoorLocker() {
	if (!mIsUnlocked) {
		unlock();
	}
}

bool DoorLocker::isEmpty() const {
	return count() == 0;
}

bool DoorLocker::isNull() const {
	return mIsNull;
}

size_t DoorLocker::count() const {
	return mDoorComponents.Num();
}

bool DoorLocker::couldPlaceAllDoors() const {
	return mCouldPlaceAllDoors;
}

DoorLocker& DoorLocker::lock(bool instant) {
	if (mIsUnlocked) {
		for (auto&& door : mDoorComponents) {
			if (door != nullptr) {
				door->Close(instant);
			}
		}
		mIsUnlocked = false;
	}
	return *this;
}

DoorLocker& DoorLocker::unlock(bool instant) {
	if (!mIsUnlocked) {
		for (auto&& door : mDoorComponents) {
			if (door != nullptr) {
				door->Open(instant);
			}
		}
		mIsUnlocked = true;
	}
	return *this;
}


TArray<FVector> DoorLocker::getDoorLocations() const {
	TArray<FVector> out;
	for (auto door : mDoorComponents) {
		out.Emplace(door->GetOwner()->GetActorLocation());
	}
	return out;
}

DoorVector doorsFromRegionLocator(const Tiles& tiles, const io::RegionLocator& locator) {
	return doorsFromRegionLocators(tiles, { locator });
}

DoorVector doorsFromRegionLocators(const Tiles& tiles, const std::vector<io::RegionLocator>& locators) {
	std::vector<DoorDef> doorDefs;
	for (auto& region : regionfinders::Default(tiles).get(locators)) {
		auto& bounds = region.area();
		auto tile = tiles.getTile(bounds.minInclusive);
		doorDefs.push_back(door::probableDoor(bounds, tile->bounds()));
	}
	return doorDefs;
}

io::RegionLocator locatorFromTile(TileRef tile, const std::string& name) {
	return tile.stretch().def.id + '.' + tile.tile().id() + '.' + name;
}

DoorVector doorsFromTileGates(const Tiles& tiles, TileRef tile) {
	return doorsFromRegionLocator(tiles, locatorFromTile(tile, "gate"));
}

DoorVector doorsFromTileDoorNames(const Tiles& tiles, TileRef tile, const std::vector<std::string>& names) {
	std::vector<io::RegionLocator> locators;
	std::transform(begin(names), end(names), std::back_inserter(locators), [&tile](auto&& name) { return locatorFromTile(tile, name); });
	return doorsFromRegionLocators(tiles, locators);
}

DoorVector doorsFromTileExits(const Tiles& tiles, TileRef tile) {
	auto doors = tile.tilePlacement().doors();

	if (auto prev = tiles.getPreviousDoor(tile)) {
		std::vector<DoorDef> exitDoors;
		std::copy_if(begin(doors), end(doors), std::back_inserter(exitDoors), [entry = prev->door](auto&& door) { return entry != door; });
		return exitDoors;
	}
	return doors;
}

DoorVector doorsFromTileDoors(const Tiles& tiles, TileRef tile) {
	return tile.tilePlacement().doors();
}

TOptional<DoorVector> doorsFromObjectiveGate(const Tiles& tiles, const io::ObjectiveGate& gate, TilePtr tile /*= null*/) {
	if (!gate.type) {
		return {};
	}

	DoorVector doors;
	auto addDoors = [&doors](const DoorVector& v) { doors.insert(doors.end(), v.begin(), v.end()); };

	const auto type = gate.type.Get(io::ObjectiveGate::Type(0));
	if (type & io::ObjectiveGate::TileExits) {
		check(tile);
		addDoors(doorsFromTileExits(tiles, *tile));
	}
	if (type & io::ObjectiveGate::Regions) {
		addDoors(doorsFromRegionLocators(tiles, gate.regions));
	}
	if (type & io::ObjectiveGate::Gates) {
		check(tile);
		addDoors(doorsFromTileGates(tiles, *tile));
	}
	return doors;
}

TOptional<DoorVector> doorsFromObjectiveGate(const Tiles& tiles, const io::ObjectiveGate& gate, io::ObjectiveGate orDefault, TilePtr tile /*= null*/) {
	const auto doors = doorsFromObjectiveGate(tiles, gate, tile);
	return doors? doors : doorsFromObjectiveGate(tiles, orDefault, tile);
}

Unique<DoorLocker> objectiveDoorLocker(Game& game, const io::ObjectiveGate& gate, TOptional<DoorVector> defaultDoors, const io::ObjectiveGate& orDefault, TilePtr tile) {
	if (gate.type.Get(orDefault.type.Get(io::ObjectiveGate::Gates)) == io::ObjectiveGate::Nothing) {
		return make_unique<DoorLocker>(game.world());
	}
	if (auto doors = doorsFromObjectiveGate(game.tiles(), gate, orDefault, tile)) {
		const auto prefab = gate.object.Get(orDefault.object.Get(""));
		auto doorLocker = make_unique<DoorLocker>(game.world(), doors.GetValue(), prefab);
		if (gate.startUnlocked.Get(orDefault.startUnlocked.Get(false))) {
			doorLocker->unlock(true);
		}
		return doorLocker;
	}
	return{};
}

Unique<DoorLocker> objectiveDoorLocker(Game& game, const io::ObjectiveGate& gate, const io::ObjectiveGate& orDefault, TilePtr tile) {
	return objectiveDoorLocker(game, gate, doorsFromObjectiveGate(game.tiles(), orDefault, tile).Get({}), orDefault, tile);
}

}}
