// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "FireworkArrow.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include <game/component/EquipmentComponent.h>

int AFireworkArrow::GetDisplayCount() const {
	return 0;
}


void AFireworkArrow::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetOwner();
	
	if (!owner)
		return;

	auto equipmentComponent = owner->FindComponentByClass<UEquipmentComponent>();
	if (!equipmentComponent)
		return;

	TArray<UItemSlot*> fireworksArrowSlot(equipmentComponent->GetSlotsOfType(ESlotType::FireworksArrow));

	if (fireworksArrowSlot.Num()) {
		auto& slot = fireworksArrowSlot[0];
		if (slot->GetCount() == 0) {
			slot->EquipItem(FInventoryItemData(game::item::type::Arrow.getId(), ItemPower));
		}
	}

	Super::Activate(predictionKey);
}


void AFireworkArrow::OnArrowFired() {
	// Finish Activation

	Super::TryActivate();
}
