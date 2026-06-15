#include "Dungeons.h"
#include "MissionPrimaryDataAssetBase2.h"
#include "MissionPrimaryAssetUtil.h"
#include <Engine/AssetManager.h>
#include "LogMacros.h"
#include "game/level/TileDecor.h"
#include "util/EnumUtil.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "util/AssetUtil.h"
#include "util/StringUtil.h"
#include "world/level/block/Block.h"
#include "lovika/builder/LovikaBaseBuilder.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"


const FName UMissionPrimaryDataAssetBase2::BundleName(TEXT("HypermissionsBundle"));

FPrimaryAssetId UMissionPrimaryDataAssetBase2::GetPrimaryAssetId() const
{
	FPrimaryAssetId id = Super::GetPrimaryAssetId();

	id.PrimaryAssetType = *UMissionPrimaryDataAssetBase2::StaticClass()->GetName();

	return id;
}

UMissionPrimaryDataAssetBase2::UMissionPrimaryDataAssetBase2() {
	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA
void UMissionPrimaryDataAssetBase2::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		return;
	}

	TSet<FSoftObjectPath> NewPaths;

	UAssetManager& Manager = UAssetManager::Get();
	IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();

	if(GetShouldPackageAssets()){

		const auto filenames = GetFilenames();

		UE_LOG(LogDungeons, Log, TEXT("Updating Asset Bundle data for level '%s' (filenames: %s)"),
			*GetEnumValueToStringStripped(GetLevelName()),
			*FString::Join(filenames, TEXT(", ")));

		if (!missionasset::IsBuildSystemInitialized())
		{
			UE_LOG(LogDungeons, Warning, TEXT("ULovikaBaseBuilder not initialized, creating..."))
			Material::teardownMaterials();
			Block::teardownBlocks();

			Material::initMaterials();
			Block::initBlocks();
		}

		for (const auto& filename : filenames) {

			if (auto sourceData = levelgen::sourcedata::fromFileSystem(filename)) {

				//Intro
				NewPaths.Append(missionasset::ValidAssetsFromPackageNames(missionasset::GetIntroAsset(filename, *sourceData), AssetRegistry, Manager));

				TArray<FString> sublevelPaths;
				FString missionId(sourceData->level.id.c_str());

				for (const auto& group : sourceData->level.objectGroups) {
					sublevelPaths.Add(decor::pathForSublevels(missionId, group.name.id.c_str()));
				}

				//Get unique door sizes since we need this for gates and travel doors.
				TSet<int> doorSizes = missionasset::GetDoorSizesForTiles(sourceData->tiles.tiles());
				doorSizes.Append(missionasset::GetDoorSizesForTiles(sourceData->props.tiles()));

				//Sublevels
				NewPaths.Append(missionasset::GetSublevelsForTiles(sublevelPaths, AssetRegistry, Manager, sourceData->tiles.tiles()));
				NewPaths.Append(missionasset::GetSublevelsForTiles(sublevelPaths, AssetRegistry, Manager, sourceData->props.tiles()));

				//Environmental
				NewPaths.Append(missionasset::GetEnvironmentalAssetsForMission(missionId, AssetRegistry, Manager));

				//Doors
				NewPaths.Append(missionasset::GetDoorPrefabsForTiles(sourceData->tiles.tiles(), AssetRegistry, Manager));
				NewPaths.Append(missionasset::GetDoorPrefabsForTiles(sourceData->props.tiles(), AssetRegistry, Manager));
				//Travel Doors
				NewPaths.Append(missionasset::GetTravelDoorPrefabsForTiles(sourceData->tiles.tiles(), doorSizes, AssetRegistry, Manager));
				NewPaths.Append(missionasset::GetTravelDoorPrefabsForTiles(sourceData->props.tiles(), doorSizes, AssetRegistry, Manager));
				//Challanges
				NewPaths.Append(missionasset::GetPrefabsForChallengesForTiles(sourceData->tiles.tiles(), AssetRegistry, Manager));
				NewPaths.Append(missionasset::GetPrefabsForChallengesForTiles(sourceData->props.tiles(), AssetRegistry, Manager));
				//Objectives
				NewPaths.Append(missionasset::GetPrefabsForObjectives(sourceData->level.passThrough.objectives, AssetRegistry, Manager));
			}
		}
		//Loading screen
		NewPaths.Append(missionasset::GetLoadingscreenAssets(GetLoadingScreenType(), GetLevelName()));
	}

	AssetBundleData.SetBundleAssets(BundleName, NewPaths.Array());
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	checkf(Rules.ChunkId < 100, TEXT("Nintendo Switch can only categorize chunks < 100, please select a different ChunkId"));

	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}


#endif
