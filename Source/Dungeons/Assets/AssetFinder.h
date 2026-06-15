#pragma once

#include "CoreMinimal.h"
#include "AssetFinder.Generated.h"

class IAssetRegistry;

struct FAssetEntry {
	FAssetEntry(FName key, FSoftObjectPath path, const UClass* cls, FName group)
		:Key(key),Path(path),Class(cls),Group(group) {};
	
	FName Key;
	FSoftObjectPath Path;
	const UClass* Class;
	FName Group;
};

struct FAssetRequestEntry {
	FAssetRequestEntry(FName group, const UClass* cls) :Group(group), Class(cls) {}
	
	FName Group;
	const UClass* Class;
};

struct FAssetData;
UCLASS(Config=Game)
class DUNGEONS_API UAssetFinder : public UObject {
	GENERATED_BODY()
public:
	using AssetPred = TFunction<bool(const FAssetEntry&)>;

	virtual void Initialize(bool force);
	bool IsInitialized() const { return DiscoveredAssets.Num() > 0; }
	TArray<FSoftObjectPath> GetAssets(AssetPred pred = nullptr) const;

	static FSoftObjectPath GetPathForAsset(const FAssetData&);
	
#if WITH_EDITOR
	void ReprocessGroup(const FName&);
#endif

protected:
	FORCEINLINE const TArray<FAssetEntry>& GetDiscoveredAssets() const { return DiscoveredAssets; }
	FORCEINLINE TMap < FName, TMap<FName, FSoftObjectPath>> GetMappedAssets() const { return AssetMap; }
	TOptional<FSoftObjectPath> GetPath(const FName& group, const FName& assetId) const;
	
	//Adds an asset to look for of the given type. Key is the path (excluding asset root) that we look for.
	void RequestAsset(const UClass*, FName Key, FName Group = NAME_None);
private:
	virtual void OnInitialize(bool force) const {};
	virtual void OnInitializationComplete() {};
	virtual TArray<FString> AssetLocations() const { return {}; }
	TArray<FString> GeneratePaths() const;

	TMap <FName, TMap<FName, FSoftObjectPath>> GenerateRequestMap() const;

	TArray<FAssetEntry> ProcessAll(IAssetRegistry&, const TArray<FString>& paths, const TMap<FName, FAssetRequestEntry>&) const;
	TArray<FAssetEntry> ProcessSinglePath(IAssetRegistry&, const FString&, const TMap<FName, FAssetRequestEntry>&) const;
	static bool IsClass(const FAssetData&, const UClass*);
	//Asset types (such as Textures and Materials) we should look for
	TSet<FName> AssetsClassesToFind;
	//Map of request assets used to resolve discovered assets to actually requested assets.
	TMap<FName, FAssetRequestEntry> RequestedAssets;
	
	TArray<FAssetEntry> DiscoveredAssets;
	TMap <FName, TMap<FName, FSoftObjectPath>> AssetMap;

	UPROPERTY(Config)
	TArray<FString> AssetRoots;
};