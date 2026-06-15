// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Infinity.h"
#include "game/item/ArrowItemSlot.h"
#include "game/component/EquipmentComponent.h"

UInfinity::UInfinity() {
	TypeId = EEnchantmentTypeID::Infinity;
	ServerOnlyExecution = true;
	LevelMultiplier = [this](int level) -> float {
		return 0.16f * level;
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

void UInfinity::OnAfterAmmoConsumed(TSubclassOf<ABaseProjectile> projectileClass) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;

	if ((FMath::FRand() < LevelMultiplier(Level)) || bAlwaysTrigger) {
		if (const auto equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
			const auto type = projectileClass.GetDefaultObject()->GetProjectileItemType();
			const ItemType& itemType = GetItemRegistry().Get(type.Get(game::item::type::Arrow.getId()));

			const auto arrowSlots = equipmentComponent->GetSlotsOfType(itemType.slotType());
			if (const auto slot = static_cast<UArrowItemSlot*>(arrowSlots[0])) {
				BroadcastEnchantmentTriggeredEvent();
				slot->EquipFixedAmount(FInventoryItemData(itemType.getId(), 1.0f), 1);
			}
		}
	}
}
