#include "TileDecorator.h"
#include "AssetCleanup.h"
#include <AssetRegistryModule.h>
#include <Modules/ModuleManager.h>
#include <ObjectTools.h>
#include "TileDecoratorUtils.h"

ReferencedAssets partitionReferencedUnreferencedAssets(const TArray<FAssetData>& assets) {
	auto& assetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

	ReferencedAssets out;
	for (auto& assetData : assets) {
		Asset asset{ assetData.GetAsset() };
		assetRegistry.GetReferencers(asset.obj->GetOutermost()->GetFName(), asset.diskReferences);

		if (asset.isReferenced()) {
			out.referenced.Add(std::move(asset));
		} else {
			out.unreferenced.Add(std::move(asset));
		}
	}
	return out;
}

int deleteAssets(const TArray<Asset>& assets) {
	TArray<UObject*> objectsToDelete;
	for (auto& obj : assets) {
		objectsToDelete.Add(obj.obj);
	}
	return ObjectTools::DeleteObjects(objectsToDelete, false);
}

static std::string lastPathPart(const std::string& s) {
	const auto i = s.rfind('/');
	return i != std::string::npos ? s.substr(i + 1) : s;
}

static bool _remapAsset(IAssetRegistry& assetRegistry, const std::string& src, const std::string& dst, const FName& assetClass) {
	auto srcPath = src + '.' + lastPathPart(src);
	auto srcName = FName(srcPath.c_str());
	auto srcAsset = assetRegistry.GetAssetByObjectPath(srcName);

	auto dstPath = dst + '.' + lastPathPart(dst);
	auto dstName = FName(dstPath.c_str());
	auto dstAsset = assetRegistry.GetAssetByObjectPath(dstName);

	bool isCorrectTypeSrc = srcAsset.AssetClass == assetClass;
	bool isCorrectTypeDst = dstAsset.AssetClass == assetClass;
	bool bothValid = srcAsset.IsValid() && dstAsset.IsValid();

	if (!isCorrectTypeSrc || !isCorrectTypeDst || !bothValid) {
		return false;
	}
	Asset asset;
	assetRegistry.GetReferencers(FName(src.c_str()), asset.diskReferences);

	if (asset.diskReferences.Num() == 0) {
		return false;
	}
	TArray<UObject*> sourceAssetsToDestroy{ srcAsset.GetAsset() };
	auto result = ObjectTools::ConsolidateObjects(dstAsset.GetAsset(), sourceAssetsToDestroy, false);

	util::save::saveObject(srcAsset.GetAsset());
	util::save::saveObject(dstAsset.GetAsset());
	return result.DirtiedPackages.Num() > 0;
}

bool remapAsset(const std::string& srcPath, const std::string& dstPath, const FName& assetClass) {
	return _remapAsset(
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get(),
		srcPath,
		dstPath,
		assetClass
	);
}

int remapAssets(const TArray<Mapping>& mappings, const FName& assetClass) {
	auto& assetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

	int successCount = 0;
	for (auto& mapping : mappings) {
		if (_remapAsset(assetRegistry, mapping.src, mapping.dst, assetClass)) {
			successCount++;
		}
	}
	return successCount;
}
