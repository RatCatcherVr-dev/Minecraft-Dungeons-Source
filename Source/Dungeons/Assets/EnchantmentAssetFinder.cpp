#include "EnchantmentAssetFinder.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/Enchantments/Enchantment.h"
#include "util/EnumUtil.h"
#include <Materials/MaterialInstanceConstant.h>

UEnchantmentAssetFinder::UEnchantmentAssetFinder() {
	for (const auto& enchantment : game::enchantment::type::getAvailableEnchantments()) {
		FName id = IdToName(enchantment.getEnchantmentTypeID());
		RequestAsset(UEnchantment::StaticClass(), enchantment.getRelativeClassPath(), id);
		RequestAsset(UTexture2D::StaticClass(), enchantment.getRelativeIconPath(), id);
		RequestAsset(UMaterialInstanceConstant::StaticClass(), enchantment.getRelativeMaterialPath(), id);
	}
}

TOptional<FSoftObjectPath> UEnchantmentAssetFinder::GetClassPath(EEnchantmentTypeID id) const {
	return GetPath(IdToName(id), game::enchantment::type::getEnchantmentType(id).getRelativeClassPath());
}

TOptional<FSoftObjectPath> UEnchantmentAssetFinder::GetIconPath(EEnchantmentTypeID id) const {
	return GetPath(IdToName(id), game::enchantment::type::getEnchantmentType(id).getRelativeIconPath());
}

TOptional<FSoftObjectPath> UEnchantmentAssetFinder::GetMaterialPath(EEnchantmentTypeID id) const{
	return GetPath(IdToName(id), game::enchantment::type::getEnchantmentType(id).getRelativeMaterialPath());
}

TArray<FSoftObjectPath> UEnchantmentAssetFinder::GetEnchantmentAssets(TFunction<bool(EEnchantmentTypeID)> pred)
{
	return GetAssets([pred](const FAssetEntry& entry) {
		if (const auto id = EnumValueFromString(EEnchantmentTypeID, entry.Group.ToString())) {
			return pred(id.GetValue());
		}
		return false;
	});
}

FName UEnchantmentAssetFinder::IdToName(EEnchantmentTypeID id) {
	return FName(*GetEnumValueToStringStripped(id));
}
