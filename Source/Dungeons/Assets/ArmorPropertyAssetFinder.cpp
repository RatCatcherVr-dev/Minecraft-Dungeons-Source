#include "ArmorPropertyAssetFinder.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <Materials/MaterialInstanceConstant.h>
#include "util/EnumUtil.h"

UArmorPropertyAssetFinder::UArmorPropertyAssetFinder() {
	for (const auto& armorproperty : game::armorproperties::type::getArmorProperties()) {
		FName id = IdToName(armorproperty.getId());
		RequestAsset(UArmorProperty::StaticClass(), armorproperty.getRelativeClassPath(), id);
		RequestAsset(UTexture2D::StaticClass(), armorproperty.getRelativeIconPath(), id);
		RequestAsset(UMaterialInstanceConstant::StaticClass(), armorproperty.getRelativeMaterialPath(), id);
	}
}

TOptional<FSoftObjectPath> UArmorPropertyAssetFinder::GetClassPath(EArmorPropertyID id) const
{
	return GetPath(IdToName(id), game::armorproperties::type::getArmorPropertyType(id).getRelativeClassPath());
}

TOptional<FSoftObjectPath> UArmorPropertyAssetFinder::GetIconPath(EArmorPropertyID id) const
{
	return GetPath(IdToName(id), game::armorproperties::type::getArmorPropertyType(id).getRelativeIconPath());
}

TOptional<FSoftObjectPath> UArmorPropertyAssetFinder::GetMaterialPath(EArmorPropertyID id) const
{
	return GetPath(IdToName(id), game::armorproperties::type::getArmorPropertyType(id).getRelativeMaterialPath());
}

TArray<FSoftObjectPath> UArmorPropertyAssetFinder::GetArmorPropertyAssets(TFunction<bool(EArmorPropertyID)> pred)
{
	return GetAssets([pred](const FAssetEntry& entry) {
		if (const auto id = NameToId(entry.Group)) {
			return pred(id.GetValue());
		}
		return false;
	});
}

FName UArmorPropertyAssetFinder::IdToName(EArmorPropertyID id) {
	return FName(*GetEnumValueToStringStripped(id));
}

TOptional<EArmorPropertyID> UArmorPropertyAssetFinder::NameToId(const FName& name)
{
	return EnumValueFromString(EArmorPropertyID, name.ToString());
}
