#include "ItemPrimaryDataAsset.h"
#include "LogMacros.h"
#include <Engine/AssetManager.h>
#include "game/item/ItemTypeDefs.h"
#include <AssetBundleData.h>
#include "ui/PostGameWidget.h"
#include "ItemAssetFinder.h"

namespace game { namespace item { namespace asset {
	bool shouldPackageAssets(const FItemId& id) {
		const auto& itemType = GetItemRegistry().Get(id);

		if (itemType.isWorkInProgress()) {
			return false;
		}

		const auto& restricted = itemType.getRestrictedMissionDifficulties();
				
		auto disabled = 0;
		for (auto& restriction : restricted) {
			if(!missions::shouldPackageAssets(restriction.Key)){
				disabled++;
			}
		}

		return restricted.Num() <= 0 || disabled != restricted.Num();
	}
}}}


const FName UItemPrimaryDataAsset::BundleName(TEXT("ItemsBundle"));

UItemPrimaryDataAsset::UItemPrimaryDataAsset() {
	Rules.bApplyRecursively = false;
	Rules.Priority = 0;
	bIsRuntimeLabel = false;
}

#if WITH_EDITORONLY_DATA
void UItemPrimaryDataAsset::UpdateAssetBundleData() {
	Super::UpdateAssetBundleData();

	if (!UAssetManager::IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid UAssetManager for ItemPrimaryDataAsset!"));
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	const auto& items = GetItemRegistry().GetValues();
	check(items.Num() > 0);

	auto* assetFinder = IDungeonsModule::Get().GetItemAssetFinder();
	
	TArray<FSoftObjectPath> NewPaths = assetFinder->GetItemAssets([](const FItemId& id) {
		if (game::item::asset::shouldPackageAssets(id)) {
			return true;
		} else {
			UE_LOG(LogTemp, Warning, TEXT("excluding item from packaging %s"), *GetItemRegistry().Get(id).getName());
			return false;
		}
	});

	check(NewPaths.Num() > 0);
	AssetBundleData.SetBundleAssets(BundleName, MoveTemp(NewPaths));

	// Update rules
	FPrimaryAssetId PrimaryAssetId = GetPrimaryAssetId();
	Manager.SetPrimaryAssetRules(PrimaryAssetId, Rules);
}
#endif