#include "Dungeons.h"
#include "DoorUtil.h"
#include "InstantTravelActor.h"
#include "game/Conversion.h"
#include "game/component/DoorComponent.h"
#include "game/objective/ObjectiveUtil.h"
#include "lovika/BlockCuboid.h"
#include "lovika/QuadrantAngle.h"
#include "lovika/world/level/levelgen/generator/Graph.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "util/AssetUtil.h"
#include <LogMacros.h>
#include "game/GameTypes.h"

namespace game { namespace door {
const FString DEFAULT_DOOR_PREFAB_PATH(TEXT("Decor/Prefabs/Door/BP_Door"));
const FString DEFAULT_TRAVEL_DOOR_PREFAB_PATH(TEXT("Decor/Prefabs/DoorTravel/GenericDoor/BP_GenericTravelDoor"));
	
DoorDef door(const BlockCuboid& bounds, FacingID facing) {
	auto doorPos = (bounds.minInclusive + bounds.maxExclusive) / 2;
	auto doorSize = bounds.size();
	return DoorDef(doorPos, facing, std::max(doorSize.x, doorSize.z), "", "");
}

DoorDef possibleDoor(const BlockCuboid& bounds) {
	auto dim = bounds.size();
	auto facing = dim.x > dim.z ? Facing::SOUTH : Facing::EAST;
	return door(bounds, facing);
}

DoorDef probableDoor(const BlockCuboid& door, const BlockCuboid& tileBounds) {
	return door::door(door, probableFacing(door, tileBounds));
}

Facing::Name probableFacing(const BlockCuboid& door, const BlockCuboid& tileBounds) {
	const auto tileCenter = (tileBounds.minInclusive + tileBounds.maxExclusive) / 2;
	const auto& location = door.minInclusive;

	if (door.maxExclusive.x - door.minInclusive.x > door.maxExclusive.z - door.minInclusive.z) {
		return door.minInclusive.z <= tileCenter.z ? Facing::Name::NORTH : Facing::Name::SOUTH;
	} else {
		return door.minInclusive.x <= tileCenter.x ? Facing::Name::WEST : Facing::Name::EAST;
	}
}

DoorDef withPrefab(DoorDef door, TOptional<FString> prefab) {
	door.prefab = prefab;
	return door;
}

UDoorComponent* placeDoor(UWorld& world, const DoorDef& door, FString prefabPath/*= ""*/) {
	if (auto actor = placeDoorLike(world, door, prefabPath)) {
		return actor->FindComponentByClass<UDoorComponent>();
	}
	return nullptr;
}

FString generatedPrefabPathForWidth(const FString& doorPath, int width)
{
	return doorPath + "_" + FString::FromInt(width);
}

	
TArray<FString> generatePreferredPrefabPaths(const DoorDef& door, FString prefabPath)
{
	TArray<FString> preferredPaths;

	if(prefabPath.IsEmpty() && door.prefab.IsSet())
	{
		prefabPath = door.prefab.GetValue();
	}

	if(!prefabPath.IsEmpty())
	{
		preferredPaths.Add(game::PrefabPath(prefabPath, door.width()));
		preferredPaths.Add(game::PrefabPath(prefabPath));
	}

	preferredPaths.Add(game::PrefabPath(DEFAULT_DOOR_PREFAB_PATH, door.width()));

	return preferredPaths;
}


AActor* placeDoorLike(UWorld& world, const DoorDef& door, UClass* pDoorClass) {
	
	if (!pDoorClass)
	{
#if !UE_BUILD_SHIPPING		
		UE_LOG(LogDungeons, Warning, TEXT("Failed spawning door"));
#endif
		return nullptr;
	}

	const auto ueDegrees = conversion::degreesToUe(quadrantToDegrees(opposite(quadrantFromFacing(door.facing()))));
	return objective::placeObject(world, *pDoorClass, Vec3(door.position()) + Vec3(0.5f, 0, 0.5f), ueDegrees);

}

AActor* placeDoorLike(UWorld& world, const DoorDef& door, FString prefabPath/*= ""*/) {
	TArray<FString> preferedPrefabPaths = generatePreferredPrefabPaths(door, prefabPath);
	UClass* cls = nullptr;
	
	for(int i = 0; i < preferedPrefabPaths.Num() && cls == nullptr; ++i)
	{
		cls = ConstructorHelpersInternal::FindOrLoadClass(preferedPrefabPaths[i], AActor::StaticClass());
	}

	return placeDoorLike(world, door, cls);
}


TArray<FSoftObjectPath> GetPreferredPrefabSoftPaths(const DoorDef& src)
{
	TArray<FSoftObjectPath> SrcNames;
	TArray<FString> SrcPreferedPrefabPaths = generatePreferredPrefabPaths(src, src.prefab.Get(DEFAULT_TRAVEL_DOOR_PREFAB_PATH));

	for (auto& preFabPath : SrcPreferedPrefabPaths)
	{
		int index = INDEX_NONE;
		if (preFabPath.FindLastChar('/', index))
		{
			FString BaseAssetName = preFabPath.RightChop(index + 1);
			FString ClassName = preFabPath + TEXT(".") + BaseAssetName + TEXT("_C");
			SrcNames.Add(ClassName);
		}
	}

	return SrcNames;
}


//
// Teleport doors
//
void placeTeleportDoors(UWorld& world, const DoorDef& src, const DoorDef& dst, bool bidirectional) {
	auto a = Cast<AInstantTravelActor>(placeDoorLike(world, src, src.prefab.Get(DEFAULT_TRAVEL_DOOR_PREFAB_PATH)));
	auto b = Cast<AInstantTravelActor>(placeDoorLike(world, dst, dst.prefab.Get(DEFAULT_TRAVEL_DOOR_PREFAB_PATH)));

	if (a && b)
	{
		a->SetDestinationLocation(b->GetTravelToMeLocation());
		a->SetDestinationRotation(b->GetTravelToMeRotation());
		if (bidirectional) {
			b->SetDestinationLocation(a->GetTravelToMeLocation());
			b->SetDestinationRotation(a->GetTravelToMeRotation());
		}
	}
	else
	{
		UE_LOG(LogDungeons, Warning, TEXT("\n################## Failure in placeTeleportDoors ##################\n"));
		UE_LOG(LogDungeons, Warning, TEXT(" a (%s) ##################\n"), (a) ? *a->GetFullName() : TEXT("null"));
		UE_LOG(LogDungeons, Warning, TEXT(" b (%s) ##################\n"), (b) ? *b->GetFullName() : TEXT("null"));
	}

}


void placeTeleportDoors(UWorld& world, const generator::graph::Graph& graph) {
	for (auto& edge : graph.edges()) {
		if (edge.src.door.isSpatiallyAdjacent(edge.dst.door)) {
			continue;
		}
		placeTeleportDoors(world, edge.src.door, edge.dst.door, true);
	}
}

TSharedPtr<FStreamableHandle> preloadTeleportDoors(const generator::graph::Graph& graph, std::function< void() > inPostLoadFunc)
{
	TArray<FSoftObjectPath> AllNames;
	AllNames.Reserve(128);

	//generate a list of soft paths for level door assets
	for (auto& edge : graph.edges())
	{
		if (edge.src.door.isSpatiallyAdjacent(edge.dst.door)) {
			continue;
		}
				
		TArray<FSoftObjectPath> SrcNames = GetPreferredPrefabSoftPaths(edge.src.door);
		TArray<FSoftObjectPath> DstNames = GetPreferredPrefabSoftPaths(edge.dst.door);

		for (const auto& name : SrcNames)
		{
			AllNames.AddUnique(name);
		}

		for (const auto& name : DstNames)
		{
			AllNames.AddUnique(name);
		}
	}

	TSharedPtr<FStreamableHandle> streamHandle;
	if (AllNames.Num())
	{
		//async loads assets
		streamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(AllNames, [AllNames, inPostLoadFunc]() {

			//source door classes
			for (const auto& name : AllNames) {
				if (auto object = Cast<UClass>(name.ResolveObject())) {
					object->AddToRoot();
				}
			}

			inPostLoadFunc();
		});
	}
	else
	{
		//just call callback
		inPostLoadFunc();
	}

	return streamHandle;
}

}}
