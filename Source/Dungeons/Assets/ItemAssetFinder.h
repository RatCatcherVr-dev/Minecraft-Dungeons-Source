#pragma once


#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"
#include "AssetFinder.h"
#include "ItemAssetFinder.Generated.h"


UCLASS(Config = Game)
class DUNGEONS_API UItemAssetFinder : public UAssetFinder {
	GENERATED_BODY()
public:
	UItemAssetFinder();

	TOptional<FSoftObjectPath> InstancePath(const FItemId&) const;
	TOptional<FSoftObjectPath> StorablePath(const FItemId&) const;
	TOptional<FSoftObjectPath> IconPath(const FItemId&) const;
	TOptional<FSoftObjectPath> IconPathHigh(const FItemId&) const;
	TOptional<FSoftObjectPath> GearIconPath(const FItemId&) const;
	TOptional<FSoftObjectPath> AmmoIconPathSmall(const FItemId&) const;

	//Returns all found assets, taking an optional predicate to filter on itemid.
	TArray<FSoftObjectPath> GetItemAssets(TFunction<bool(const FItemId&)> Predicate = nullptr) const;
	void ReprocessItem(const FItemId&);
	
private:
	TArray<FString> AssetLocations() const override;
	
	UPROPERTY(Config)
	TArray<FString> ItemAssetPaths;
}
;