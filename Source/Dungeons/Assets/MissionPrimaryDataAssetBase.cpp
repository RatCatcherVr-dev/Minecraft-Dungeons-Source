

#include "Dungeons.h"
#include "MissionPrimaryDataAssetBase.h"
#include "MissionPrimaryAssetUtil.h"
#include <Engine/AssetManager.h>
#include "LogMacros.h"
#include "game/level/TileDecor.h"
#include "util/EnumUtil.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "lovika/world/level/levelgen/TileGroup.h"
#include "util/AssetUtil.h"
#include "game/GameTypes.h"
#include "game/level/doors/DoorUtil.h"
#include "util/StringUtil.h"
#include "world/level/block/Block.h"
#include "lovika/builder/LovikaBaseBuilder.h"
#include "LoadingScreen/LoadingScreenBuilder.h"
#include "game/objective/ChallengeSystem.h"
#include "world/level/block/Block.h"
#include "world/level/material/Material.h"


const FName UMissionPrimaryDataAssetBase::BundleName(TEXT("MissionBundle"));

FPrimaryAssetId UMissionPrimaryDataAssetBase::GetPrimaryAssetId() const
{
	FPrimaryAssetId id = Super::GetPrimaryAssetId();

	id.PrimaryAssetType = *UMissionPrimaryDataAssetBase::StaticClass()->GetName();

	return id;
}

UMissionPrimaryDataAssetBase::UMissionPrimaryDataAssetBase() {
	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA
void UMissionPrimaryDataAssetBase::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		return;
	}

	TSet<FSoftObjectPath> NewPaths;

	UAssetManager& Manager = UAssetManager::Get();
	IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();

	if(GetShouldPackageAssets()){

		UE_LOG(LogDungeons, Log, TEXT("Updating Asset Bundle data for level '%s'"), *GetFilename());
		if (!missionasset::IsBuildSystemInitialized())
		{
			UE_LOG(LogDungeons, Warning, TEXT("ULovikaBaseBuilder not initialized, creating..."))
			Material::teardownMaterials();
			Block::teardownBlocks();

			Material::initMaterials();
			Block::initBlocks();
		}

		if (auto sourceData = levelgen::sourcedata::fromFileSystem(GetFilename())) {
		
			//Intro
			NewPaths = missionasset::ValidAssetsFromPackageNames(missionasset::GetIntroAsset(GetFilename(), *sourceData), AssetRegistry, Manager);

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

			//Loading screen
			NewPaths.Append(missionasset::GetLoadingscreenAssets(GetLoadingScreenType(), GetLevelName()));

		}
	}

	AssetBundleData.SetBundleAssets(BundleName, NewPaths.Array());
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	checkf(Rules.ChunkId < 100, TEXT("Nintendo Switch can only categorize chunks < 100, please select a different ChunkId"));
	ChunkIDTag = Rules.ChunkId; // Copy for surface in Asset registry!
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}


#endif
