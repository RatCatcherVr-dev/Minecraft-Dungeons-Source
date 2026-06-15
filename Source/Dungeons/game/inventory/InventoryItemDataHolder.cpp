// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "InventoryItemDataHolder.h"
#include "AssetRegistryModule.h"
#include "Assets/AssetFinder.h"
#include "Engine/AssetManager.h"

void UInventoryItemDataHolder::Initialize() {
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> data;
	
	AssetRegistry.GetAssetsByClass(UItemInstanceInventoryDataLookup::StaticClass()->GetFName(),data);

	ensureMsgf(data.Num() == 1, TEXT("There should only ever be one ItemInstanceInventoryDataLookup asset packaged. Found %i"), data.Num());

	if(data.Num() > 0) {
		UAssetManager::GetStreamableManager().RequestAsyncLoad(UAssetFinder::GetPathForAsset(data[0]), FStreamableDelegate::CreateUObject(this, &UInventoryItemDataHolder::OnLookupLoaded, data[0]), FStreamableManager::AsyncLoadHighPriority);
	}
}

USoundCue* UInventoryItemDataHolder::GetInventoryDropSound(const FItemId& id) const {
	if(!DataLookup) return nullptr;

	return DataLookup->GetInventoryDropSound(id);
}
const TArray<FItemStatsEntry> UInventoryItemDataHolder::GetStats(const FItemId& id) const {
	if(!DataLookup) return {};

	return DataLookup->GetStats(id);
}

void UInventoryItemDataHolder::OnLookupLoaded(FAssetData asset) {
	DataLookup = Cast<UItemInstanceInventoryDataLookup>(asset.GetAsset());
}
