// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "IncreasedArrowBundleSize.h"



UIncreasedArrowBundleSize::UIncreasedArrowBundleSize() {
	TypeID = EArmorPropertyID::IncreasedArrowBundleSize;
}

void UIncreasedArrowBundleSize::BeginPlay() {
	Super::BeginPlay();
	const auto equipment = GetOwner()->FindComponentByClass<UEquipmentComponent>();
	// possibly find better way to include all SlotTypes to be affected by increaseBundleSize.
	const auto burningArrowSlots = equipment->GetSlotsOfType(ESlotType::BurningArrow);
	const auto arrowSlots = equipment->GetSlotsOfType(ESlotType::Arrow);
	const auto heavyHarpoonSlots = equipment->GetSlotsOfType(ESlotType::HeavyHarpoon);

	// bind will be gc:d
	burningArrowSlots[0]->OnPickedUp.BindUObject(this, &UIncreasedArrowBundleSize::OnArrowsPickedUp);
	arrowSlots[0]->OnPickedUp.BindUObject(this, &UIncreasedArrowBundleSize::OnArrowsPickedUp);
	heavyHarpoonSlots[0]->OnPickedUp.BindUObject(this, &UIncreasedArrowBundleSize::OnArrowsPickedUp);
}

void UIncreasedArrowBundleSize::OnArrowsPickedUp(UItemSlot* slot) {
	if (GetOwner()->HasAuthority()) {
		const auto arrowSlot = Cast<UArrowItemSlot>(slot);
		arrowSlot->EquipFixedAmount(FInventoryItemData(slot->GetItem()->GetItemId(), 1.f), ArrowIncreaseAmount);
	}
}
