#include "Dungeons.h"
#include "GearPowerCounterWidget.h"
#include "game/component/EquipmentComponent.h"

TOptional<int> UGearPowerCounterWidget::FetchBoundValue(const AActor&) const {
	if (mEquipment) {
		return mEquipment->GetTotalEquippedDisplayItemPower();
	}
	return {};
}

void UGearPowerCounterWidget::BindTo(AActor& actor) {
	if (auto* equipment = actor.FindComponentByClass<UEquipmentComponent>()) {
		equipment->OnTotalEquippedItemPowerChangedInternal.AddUObject(this, &UGearPowerCounterWidget::Refresh);
		mEquipment = equipment;
	}
}

void UGearPowerCounterWidget::UnbindFrom(AActor&) {
	if (mEquipment) {
		mEquipment->OnTotalEquippedItemPowerChangedInternal.RemoveAll(this);
		mEquipment = nullptr;
	}
}

