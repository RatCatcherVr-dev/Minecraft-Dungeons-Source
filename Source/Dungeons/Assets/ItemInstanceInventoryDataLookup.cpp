// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "ItemInstanceInventoryDataLookup.h"
#include "game/item/instance/AItemInstance.h"
#include "ItemPrimaryDataAsset.h"

USoundCue* UItemInstanceInventoryDataLookup::GetInventoryDropSound(const FItemId& id) const {
	if(auto* entry = InventoryDropSounds.Find(id)) {
		return *entry;
	}

	return nullptr;
}
const TArray<FItemStatsEntry> UItemInstanceInventoryDataLookup::GetStats(const FItemId& id) const {
	if(auto* entry = Stats.Find(id)) {
		return entry->Stats;
	}

	return {};
}

void UItemInstanceInventoryDataLookup::PreSave(const ITargetPlatform* TargetPlatform) {
	Super::PreSave(TargetPlatform);

	InventoryDropSounds.Empty();
	Stats.Empty();
	
	for(auto& type : GetItemRegistry().GetValues()) {
		if(!game::item::asset::shouldPackageAssets(type->getId())) continue;
		
		if(AItemInstance* InstanceCDO =  type->getInstanceClass().GetDefaultObject()) {
			if(InstanceCDO->InventoryDropSound) {
				InventoryDropSounds.Add(type->getId(), InstanceCDO->InventoryDropSound);
			} else {
				UE_LOG(LogDungeons, Warning, TEXT("Item %s is missing an inventory drop sound."), *type->getId().GetBackingType().ToString());
			}

			Stats.Add(type->getId(), {UItemStatsUtil::CreateItemStats(type->getId())});	
		}
	}
}
