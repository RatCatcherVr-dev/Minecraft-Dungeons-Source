#include "Dungeons.h"
#include "GearItemInstance.h"

void AGearItemInstance::CheckEquippedEffects() {
	if (bEffectsApplied) {
		RemoveEquippedEffects();
		ApplyEquippedEffects();
	}
}

void AGearItemInstance::BeginPlay() {
	Super::BeginPlay();
	OnItemPowerChanged.AddUObject(this, &AGearItemInstance::CheckEquippedEffects);
}
