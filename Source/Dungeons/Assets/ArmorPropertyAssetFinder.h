#pragma once

#include "AssetFinder.h"
#include "game/ArmorProperties/ArmorPropertyEnumTypes.h"
#include "ArmorPropertyAssetFinder.Generated.h"

UCLASS(config = Game)
class UArmorPropertyAssetFinder : public UAssetFinder {
	GENERATED_BODY()
public:
	UArmorPropertyAssetFinder();

	TOptional<FSoftObjectPath> GetClassPath(EArmorPropertyID) const;
	TOptional<FSoftObjectPath> GetIconPath(EArmorPropertyID) const;
	TOptional<FSoftObjectPath> GetMaterialPath(EArmorPropertyID) const;

	TArray<FSoftObjectPath> GetArmorPropertyAssets(TFunction<bool(EArmorPropertyID)> pred);
private:
	TArray<FString> AssetLocations() const override { return ArmorPropertyAssetPaths; }

	UPROPERTY(Config)
	TArray<FString> ArmorPropertyAssetPaths;

	static FName IdToName(EArmorPropertyID);
	static TOptional<EArmorPropertyID> NameToId(const FName&);
};