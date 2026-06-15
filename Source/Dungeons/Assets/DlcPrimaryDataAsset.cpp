#include "DlcPrimaryDataAsset.h"
#include "LogMacros.h"

#include <PackageName.h>
#include <UObjectBaseUtility.h>
#include <Engine/AssetManager.h>
#include "game/dlc/DLCDef.h"
#include "game/dlc/DLCDefs.h"
#include <AssetBundleData.h>

UDLCPrimaryDataAsset::UDLCPrimaryDataAsset() {
	Rules.bApplyRecursively = true;
	Rules.Priority = 101;
	Rules.ChunkId = 1000;
	bIsRuntimeLabel = false;
}

const FName UDLCPrimaryDataAsset::BundleName(TEXT("DLCBundle"));

#if WITH_EDITORONLY_DATA
void UDLCPrimaryDataAsset::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid UAssetManager for ItemPrimaryDataAsset!"));
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();

	if (auto dlcdef = dlc::getChecked(dlcname)) {
		AssetBundleData.Reset();

		TArray<FSoftObjectPath> NewPaths;

		if(auto path = dlcdef->GetInspectorTexturePath()){
			NewPaths.Add(path.GetValue());
		}

		if (auto path = dlcdef->GetLogoTexturePath()) {
			NewPaths.Add(path.GetValue());
		}

		AssetBundleData.SetBundleAssets(BundleName, MoveTemp(NewPaths));
	}	

	// Update rules
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}
#endif