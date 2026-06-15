#include "CosmeticsPrimaryDataAsset.h"
#include "LogMacros.h"
#include <Engine/AssetManager.h>
#include <AssetBundleData.h>

const FName UCosmeticsPrimaryDataAsset::BundleName(TEXT("CosmeticsBundle"));

UCosmeticsPrimaryDataAsset::UCosmeticsPrimaryDataAsset() {
	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA
void UCosmeticsPrimaryDataAsset::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid UAssetManager for CosmeticsPrimaryDataAsset!"));
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();

	TArray<FSoftObjectPath> NewPaths;
	for (auto cosmeticsSet : CosmeticsCollection) {
		cosmeticsSet->ForeachRow<FCosmeticsEntryBase>("Cosmetics Set",
			[&NewPaths](const FName name, const auto& dataEntry) {
				if (dataEntry.IsReleased)
					NewPaths.Append(dataEntry.LinkedAssets());
			});
	}

	check(NewPaths.Num() > 0);
	AssetBundleData.SetBundleAssets(BundleName, MoveTemp(NewPaths));

	// Update rules
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}
#endif