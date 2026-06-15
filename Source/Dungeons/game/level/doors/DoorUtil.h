#pragma once
#include "lovika/tile/DoorDef.h"

class AActor;
class UWorld;
class UDoorComponent;
struct BlockCuboid;
struct FStreamableHandle;

//D11.PS D11.TODO - look at this
//typedef int_fast8_t FacingID;
typedef signed char FacingID;

namespace generator { namespace graph { struct Graph; }}

namespace game { namespace door {
extern const FString DEFULT_DOOR_PREFAB_PATH;
extern const FString DEFAULT_TRAVEL_DOOR_PREFAB_PATH;

FString generatedPrefabPathForWidth(const FString& doorPath, int width);
TArray<FString> generatePreferredPrefabPaths(const DoorDef&, FString prefabPath = "");
	
DoorDef door(const BlockCuboid&, FacingID);
DoorDef possibleDoor(const BlockCuboid&);
DoorDef probableDoor(const BlockCuboid& door, const BlockCuboid& tileBounds);
Facing::Name probableFacing(const BlockCuboid& door, const BlockCuboid& tileBounds);

DoorDef withPrefab(DoorDef, TOptional<FString> prefab);


AActor* placeDoorLike(UWorld&, const DoorDef&, UClass* pDoorClass = nullptr);
AActor* placeDoorLike(UWorld&, const DoorDef&, FString prefabPath = "");

UDoorComponent* placeDoor(UWorld&, const DoorDef&, FString prefabPath = "");

void placeTeleportDoors(UWorld&, const DoorDef& src, const DoorDef& dst, bool bidirectional);
void placeTeleportDoors(UWorld&, const generator::graph::Graph&);

TSharedPtr<FStreamableHandle> preloadTeleportDoors(const generator::graph::Graph& graph, std::function< void() > inPostLoadFunc);

}}
