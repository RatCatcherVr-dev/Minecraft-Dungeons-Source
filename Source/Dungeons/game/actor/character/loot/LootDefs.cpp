#include "Dungeons.h"
#include "LootDefs.h"
#include "game/item/ItemType.h"
#include "Assets/ItemAssetFinder.h"

FItemMetaData::FItemMetaData()
	: Duration(0)
	, DefaultCooldown(0)
	, IsGear(false) {
}

FItemMetaData::FItemMetaData(const FItemId&  type) {
	const auto& ItemType = GetItemRegistry().Get(type);
	Name = *ItemType.getName();
	Description = ItemType.getDescription();
	Duration = ItemType.getDurationSeconds();
	DefaultCooldown = ItemType.getCoolDownSeconds();
	IsGear = ItemType.isGear();

	if (const auto path = IDungeonsModule::Get().GetItemAssetFinder()->IconPathHigh(type)) {
		Image = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *path->ToString()));
	}
}
