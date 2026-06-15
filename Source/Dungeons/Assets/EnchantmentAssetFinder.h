#pragma once

#include "AssetFinder.h"
#include "game/Enchantments/EnchantmentType.h"
#include "EnchantmentAssetFinder.Generated.h"

UCLASS(config = Game)
class UEnchantmentAssetFinder : public UAssetFinder {
	GENERATED_BODY()
public:
	UEnchantmentAssetFinder();

	TOptional<FSoftObjectPath> GetClassPath(EEnchantmentTypeID) const;
	TOptional<FSoftObjectPath> GetIconPath(EEnchantmentTypeID) const;
	TOptional<FSoftObjectPath> GetMaterialPath(EEnchantmentTypeID) const;

	TArray<FSoftObjectPath> GetEnchantmentAssets(TFunction<bool(EEnchantmentTypeID)> pred);
private:
	TArray<FString> AssetLocations() const override { return EnchantmentAssetPaths; }

	UPROPERTY(Config)
	TArray<FString> EnchantmentAssetPaths;

	static FName IdToName(EEnchantmentTypeID);
};