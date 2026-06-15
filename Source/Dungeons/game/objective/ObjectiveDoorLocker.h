#pragma once

#include "CommonTypes.h"
#include "game/component/DoorComponent.h"
#include "game/level/GameTile.h"
#include "lovika/tile/TileDef.h"
#include <Array.h>

class UWorld;

namespace io {
	using RegionLocator = std::string;
	struct ObjectiveGate;
}

namespace game {
class Game;
class Tiles;

namespace objective {

class DoorLocker {
public:
	DoorLocker(UWorld&); // The "null door locker"
	DoorLocker(UWorld&, const DoorVector&, const FString& prefabPath = "");
	DoorLocker(DoorLocker&&) = default;
	~DoorLocker();

	size_t count() const;
	bool isEmpty() const;
	bool isNull() const; // If this is the "null door locker" == nothing should be locked on purpose
	bool couldPlaceAllDoors() const;

	DoorLocker& lock(bool instant = false);
	DoorLocker& unlock(bool instant = false);

	TArray<FVector> getDoorLocations() const;
private:
	UWorld& mWorld;
	bool mIsNull;
	bool mIsUnlocked;
	bool mCouldPlaceAllDoors;
	TArray<TWeakObjectPtr<UDoorComponent>> mDoorComponents;
};

DoorVector doorsFromRegionLocator(const Tiles&, const io::RegionLocator&);
DoorVector doorsFromRegionLocators(const Tiles&, const std::vector<io::RegionLocator>&);
DoorVector doorsFromTileGates(const Tiles&, game::TileRef);
DoorVector doorsFromTileExits(const Tiles&, game::TileRef);
DoorVector doorsFromTileDoors(const Tiles&, game::TileRef);
DoorVector doorsFromTileDoorNames(const Tiles&, game::TileRef, const std::vector<std::string>&);
TOptional<DoorVector> doorsFromObjectiveGate(const Tiles&, const io::ObjectiveGate&, game::TilePtr = nullptr);
TOptional<DoorVector> doorsFromObjectiveGate(const Tiles&, const io::ObjectiveGate&, io::ObjectiveGate orDefault, game::TilePtr = nullptr);

Unique<DoorLocker> objectiveDoorLocker(Game&, const io::ObjectiveGate&, TOptional<DoorVector> defaultDoors, const io::ObjectiveGate& defaultRest, game::TilePtr = nullptr);
Unique<DoorLocker> objectiveDoorLocker(Game&, const io::ObjectiveGate&, const io::ObjectiveGate& orDefault, game::TilePtr = nullptr);

}}
