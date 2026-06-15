#include "Dungeons.h"
#include "StaticMeshSpawner.h"
#include "game/Conversion.h"
#include "game/level/InstancedMeshBase.h"
#include "game/level/StaticMeshCollection.h"
#include "game/util/ActorQuery.h"
#include "lovika/RegionPredicates.h"

namespace game { namespace tile {

TAutoConsoleVariable<int32> CVarStaticMeshSpawnerSubRegionSize(
	TEXT("Dungeons.Instancing.StaticMeshSpawner.SubRegion"),
	32,
	TEXT("Sets StaticMeshSpawner instancing subRegion size, to stipulate areas as single actor is responsible for"),
	ECVF_Default
);

StaticMeshSpawner::StaticMeshSpawner(UWorld& world)
	: mWorld(world) {
}

void StaticMeshSpawner::operator()(TilePreparationState state) {
	auto spawnRegions = state.tile.tilePlacement().filterRegions(regionpredicates::isStaticMesh());

	int32 RegionSize = CVarStaticMeshSpawnerSubRegionSize.GetValueOnGameThread();

	for (auto& region : spawnRegions) {

		BlockPos MinBlockPos = region.area().minInclusive;

		FName subRegionName((region.name() + std::to_string(MinBlockPos.x/ RegionSize) + "_" + std::to_string(MinBlockPos.y/ RegionSize) + "_" + std::to_string(MinBlockPos.z/ RegionSize)).c_str());
					
		FName name(region.name().c_str());

		if (!mInstanceMap.Contains(subRegionName)) {
			auto meshCollection = actorquery::getFirstActor<AStaticMeshCollection>(&mWorld);
			ensureMsgf(meshCollection!=nullptr, TEXT("No Mesh Collection found, will not be able to spawn static meshes!"));
			if (meshCollection && meshCollection->StaticMeshMap.Contains(name)) {
				if (meshCollection->StaticMeshMap[name]) {
					mInstanceMap.Add(subRegionName, mWorld.SpawnActor<AInstancedMeshBase>(
						meshCollection->StaticMeshMap[name], FVector::ZeroVector, FRotator::ZeroRotator));
				}
			}
		}

		if (mInstanceMap.Contains(subRegionName)) {
			mInstanceMap[subRegionName]->Place(FTransform(conversion::blockCenterXZToUe(MinBlockPos)));
		}
	}
}

}}
