#include "ItemAssetFinder.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/AssetManager.h"
#include "game/actor/item/StorableItem.h"
#include "game/item/instance/AItemInstance.h"
#include "MessageDialog.h"
#include "MessageLog.h"
#include "ui/PostGameWidget.h"
#include "AssetFinder.h"

UItemAssetFinder::UItemAssetFinder() {
	for(const auto* type : GetItemRegistry().GetValues()) {
		const auto id = type->getId().GetBackingType();
		
		RequestAsset(AItemInstance::StaticClass(), type->getRelativeInstanceObjectPath(), id);
		RequestAsset(AStorableItem::StaticClass(), type->getRelativeStorableObjectPath(), id);
		RequestAsset(UTexture2D::StaticClass(), type->getRelativeIconPath(), id);
		RequestAsset(UTexture2D::StaticClass(), type->getRelativeIconPathHigh(), id);
		RequestAsset(UTexture2D::StaticClass(), type->getRelativeGearIconPath(), id);
		RequestAsset(UTexture2D::StaticClass(), type->getRelativeAmmoIconSmallPath(), id);
	}
}

TOptional<FSoftObjectPath> UItemAssetFinder::InstancePath(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeInstanceObjectPath());
}

TOptional<FSoftObjectPath> UItemAssetFinder::StorablePath(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeStorableObjectPath());
}

TOptional<FSoftObjectPath> UItemAssetFinder::IconPath(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeIconPath());
}

TOptional<FSoftObjectPath> UItemAssetFinder::IconPathHigh(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeIconPathHigh());
}

TOptional<FSoftObjectPath> UItemAssetFinder::GearIconPath(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeGearIconPath());
}

TOptional<FSoftObjectPath> UItemAssetFinder::AmmoIconPathSmall(const FItemId & id) const
{
	return GetPath(id.GetBackingType(), GetItemRegistry().Get(id).getRelativeAmmoIconSmallPath());
}

TArray<FSoftObjectPath> UItemAssetFinder::GetItemAssets(TFunction<bool(const FItemId&)> Predicate) const {
	if (Predicate) {
		return GetAssets([Predicate](const FAssetEntry& asset) {
			if (const auto id = GetItemRegistry().Request(asset.Group)) {
				return Predicate(id.GetValue());
			}
			return false;
		});
	} 

	return GetAssets();
}

void UItemAssetFinder::ReprocessItem(const FItemId & id) {
#if WITH_EDITOR
	ReprocessGroup(id.GetBackingType());
#endif
}

TArray<FString> UItemAssetFinder::AssetLocations() const {
	return ItemAssetPaths;
}
