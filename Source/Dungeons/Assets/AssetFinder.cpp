#include "AssetFinder.h"
#include "Engine/AssetManager.h"
#include "BlueprintSupport.h"
#include "util/Algo.h"
#include "Engine/Blueprint.h"


void UAssetFinder::Initialize(bool force) {
	if (!force && IsInitialized()) return;
	AssetMap = GenerateRequestMap();

	OnInitialize(force);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FString> Paths = GeneratePaths();

	//Copied from UObjectLibrary::LoadBlueprintAssetDataFromPaths source
#if WITH_EDITOR
	if (GIsEditor && !IsRunningCommandlet())
	{
		// The calls into AssetRegistery require /Game/ instead of /Game.
		// The calls further below, when setting up the ARFilters, do not want the trailing /.
		// (note: this is only an annoying edge case with /Game. Subfolders will work in both cases without the trailing /".
		TArray<FString> LongFileNamePaths = Paths;
		for (FString& Str : LongFileNamePaths)
		{
			if (Str.EndsWith(TEXT("/")) == false)
			{
				Str += TEXT("/");
			}
		}
		AssetRegistry.ScanPathsSynchronous(LongFileNamePaths);
	}
#endif

	DiscoveredAssets = ProcessAll(AssetRegistry, Paths, RequestedAssets);

	for (const auto& entry : DiscoveredAssets) {
		AssetMap.FindOrAdd(entry.Group).Add(entry.Key) = entry.Path;
	}

	OnInitializationComplete();
}

TArray<FSoftObjectPath> UAssetFinder::GetAssets(AssetPred pred) const
{
	TArray<FSoftObjectPath> Paths;
	static const auto transform = [](const FAssetEntry& v) -> FSoftObjectPath { return v.Path; };

	if (pred) {
		algo::copy_if_map(DiscoveredAssets, pred, transform, Paths);
	} else {
		Paths = algo::map_tarray(DiscoveredAssets, transform);
	}

	return Paths;
}

#if WITH_EDITOR
void UAssetFinder::ReprocessGroup(const FName & Group)
{
	TMap<FName, FAssetRequestEntry> newRequest;
	for (const auto& entry : RequestedAssets) {
		if (entry.Value.Group == Group) {
			newRequest.Emplace(entry.Key, entry.Value);
		}
	}

	if (newRequest.Num() < 1) { 
		return;
	}
	
	//Remove current entries for this group
	DiscoveredAssets.RemoveAllSwap([&Group](const FAssetEntry& entry) { return entry.Group == Group; });
	AssetMap[Group].Empty();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetEntry> newAssets = ProcessAll(AssetRegistry, GeneratePaths(), newRequest);

	//Add back newly discovered assets for this group
	DiscoveredAssets.Append(newAssets);

	for (const auto& entry : newAssets) {
		AssetMap.FindOrAdd(entry.Group).Add(entry.Key) = entry.Path;
	}
}
#endif

TMap<FName, TMap<FName, FSoftObjectPath>> UAssetFinder::GenerateRequestMap() const {
	TMap<FName, TMap<FName, FSoftObjectPath>> map;
	for (const auto& request : RequestedAssets) {
		map.Add(request.Value.Group);
	}

	return map;
}

TOptional<FSoftObjectPath> UAssetFinder::GetPath(const FName& group, const FName& assetId) const {
	check(IsInitialized());

	if (const auto* entry = AssetMap[group].Find(assetId)) {
		return *entry;
	}

	return {};
}

void UAssetFinder::RequestAsset(const UClass* cls, FName Key, FName Group) {
	if(cls->IsChildOf<UActorComponent>() || cls->IsChildOf<AActor>()) {
		AssetsClassesToFind.Add(UBlueprint::StaticClass()->GetFName());
	} else {
		AssetsClassesToFind.Add(cls->GetFName());	
	}
	
	RequestedAssets.Emplace(Key, FAssetRequestEntry(Group, cls));
}

TArray<FString> UAssetFinder::GeneratePaths() const {
	TArray<FString> result;

	for (const auto& location : AssetRoots) {
		for (const auto& folder : AssetLocations()) {
			result.Add(location + folder);
		}
	}

	return result.Num() ? result : AssetRoots;
}

TArray<FAssetEntry> UAssetFinder::ProcessAll(IAssetRegistry& registry, const TArray<FString>& paths, const TMap<FName, FAssetRequestEntry>& filter) const {
	TArray<FAssetEntry> foundAssets;
	for (const auto& path : paths) {
		foundAssets.Append(ProcessSinglePath(registry, path, filter));
	}

	return foundAssets;
}

TArray<FAssetEntry> UAssetFinder::ProcessSinglePath(IAssetRegistry& registry, const FString& path, const TMap<FName, FAssetRequestEntry>& assetFilter) const {
		
	FARFilter ARFilter;
	ARFilter.ClassNames = AssetsClassesToFind.Array();
	ARFilter.PackagePaths.Add(*path);
	ARFilter.bRecursivePaths = true;
	ARFilter.bIncludeOnlyOnDiskAssets = false;

	TArray<FAssetData> assets;
	registry.GetAssets(ARFilter, assets);
	
	TArray<FAssetEntry> mappedAssets;
	for(const auto& asset : assets) {
		FName localPath(*asset.PackageName.ToString().RightChop(path.Len()));
		if(const auto type = assetFilter.Find(localPath)) {
			if (IsClass(asset, type->Class)) {
				mappedAssets.Emplace(localPath, GetPathForAsset(asset), type->Class, type->Group);
			}
		}
	}
	return mappedAssets;
}

FSoftObjectPath UAssetFinder::GetPathForAsset(const FAssetData& data) {

	if(data.AssetClass == UBlueprint::StaticClass()->GetFName()) {
		const FString GeneratedClassTag = data.GetTagValueRef<FString>(FBlueprintTags::GeneratedClassPath);
		if (!GeneratedClassTag.IsEmpty()) return FSoftObjectPath(FPackageName::ExportTextPathToObjectPath(*GeneratedClassTag));
	}

	return data.ToSoftObjectPath();
}

bool UAssetFinder::IsClass(const FAssetData & asset, const UClass* cls)
{
	if (asset.AssetClass == UBlueprint::StaticClass()->GetFName()) {
		if (UClass* nativeClass = FindObject<UClass>(nullptr, *asset.GetTagValueRef<FString>(FBlueprintTags::NativeParentClassPath))) {
			return nativeClass->IsChildOf(cls);
		}
	}

	return asset.AssetClass == cls->GetFName();
}
