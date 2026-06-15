#pragma once

#include <Containers/Array.h>
#include <AssetData.h>
#include <string>

struct Asset {
	UObject* obj;
	TArray<FName> diskReferences;

	bool isReferenced() const { return diskReferences.Num() != 0; }
};

struct ReferencedAssets {
	TArray<Asset> referenced;
	TArray<Asset> unreferenced;
};

ReferencedAssets partitionReferencedUnreferencedAssets(const TArray<FAssetData>&);
int deleteAssets(const TArray<Asset>&);

//
// Mapping
//
struct Mapping {
	std::string src;
	std::string dst;
};
bool remapAsset(const std::string& srcPath, const std::string& dstPath, const FName& assetClass);
int  remapAssets(const TArray<Mapping>&, const FName& assetClass);
