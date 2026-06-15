#pragma once

#include "lovika/tile/TileDef.h"
#include <Engine/AssetManager.h>

enum class ELevelNames : uint8;
enum class ELoadingScreenType : uint8;

namespace levelgen { namespace sourcedata { struct SourceData; } }

namespace missionasset {

TSet<int> GetDoorSizesForTiles(const MetaTileVector&);

TSet<FSoftObjectPath> ExtractValidAssets(const TArray<FAssetData>& potentialAssets, const UAssetManager&);

TSet<FSoftObjectPath> ValidAssetsFromPackageNames(TSet<FName>&& packageNames, IAssetRegistry&, const UAssetManager&);

TSet<FName> GetIntroAsset(FString filename, const levelgen::sourcedata::SourceData&);

TSet<FName> GetDoorPackageNamesForObjectAndSizes(const TSet<FString>& objects, const TSet<int>& doorSizes);

TSet<FSoftObjectPath> GetDoorPathsForObjectAndSizes(const TSet<FString>& objects, const TSet<int>& doorSizes, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetTravelDoorPrefabsForTiles(const MetaTileVector tiles, const TSet<int>& doorSizes, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetDoorPrefabsForTiles(const MetaTileVector&, IAssetRegistry&, const UAssetManager&);

TOptional<FString> GetGateForObjective(const io::Objective&);

TSet<FName> GetObjectPathsForObjective(const io::Objective&);

TSet<FSoftObjectPath> GetAllMatchingDoors(FString AssetNameBase, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetPrefabsForObjective(const io::Objective&, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetPrefabsForObjectives(const std::vector<io::Objective>&, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetPrefabsForChallengesForTiles(const MetaTileVector&, IAssetRegistry&, const UAssetManager&);

TSet<FSoftObjectPath> GetSublevelsForTiles(const TArray<FString>& potentialPaths, IAssetRegistry&, const UAssetManager&, const MetaTileVector&);

TSet<FSoftObjectPath> GetLoadingscreenAssets(ELoadingScreenType, ELevelNames);

TSet<FSoftObjectPath> GetEnvironmentalAssetsForMission(const FString& missionId, IAssetRegistry&, const UAssetManager&);

bool IsBuildSystemInitialized();

}
