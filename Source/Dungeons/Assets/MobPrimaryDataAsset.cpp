
#include "Dungeons.h"
#include "MobPrimaryDataAsset.h"
#include "LogMacros.h"
#include <PackageName.h>
#include <UObjectBaseUtility.h>
#include <Engine/AssetManager.h>
#include "game/GameTypes.h"
#include <AssetBundleData.h>
#include <vector>


const FName UMobPrimaryDataAsset::MobsBundle(TEXT("MobsBundle"));

UMobPrimaryDataAsset::UMobPrimaryDataAsset() {
	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA
template<typename T>
static TArray<T> asArray(std::vector<T> in) {
	TArray<T> out;
	for (T item : in) {
		out.Add(item);
	}
	return out;
}


void UMobPrimaryDataAsset::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		return;
	}

	// get the types:
	TArray<FSoftObjectPath> NewPaths;
	NewPaths.Append(asArray(game::TypeMap::singleton().Entities()).FilterByPredicate([](const FSoftObjectPath& path) {
		if (!path.IsValid()) {
			UE_LOG(LogDungeons, Log, TEXT("Trying to add asset for cook: %s, but path returns 'InValid()' and will not be cooked!"), *path.GetAssetName());
		}
			return path.IsValid();
		}));
	
	UAssetManager& Manager = UAssetManager::Get();

	AssetBundleData.SetBundleAssets(MobsBundle, MoveTemp(NewPaths));

	// Update rules
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}
#endif