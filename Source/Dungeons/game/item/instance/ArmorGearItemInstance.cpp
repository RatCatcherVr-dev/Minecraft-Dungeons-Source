#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "ArmorGearItemInstance.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"

AArmorGearItemInstance::AArmorGearItemInstance() {
	PowerEffects = { UMaxHealthIncrease::StaticClass(), UArmorDamageIncrease::StaticClass() };
}

void AArmorGearItemInstance::SetArmorProperties(const TArray<FArmorPropertyData>& properties) {
	armorProperties = properties;
}

void AArmorGearItemInstance::ApplyEquippedEffects() {
	Super::ApplyEquippedEffects();

	if (auto propertiesComponent = GetOwner()->FindComponentByClass<UArmorPropertiesComponent>()) {
		propertiesComponent->ServerAddProperties(armorProperties, ItemPower);
	}
}

void AArmorGearItemInstance::RemoveEquippedEffects() {
	Super::RemoveEquippedEffects();

	auto* owner = GetOwner();
	if (owner) {
		if (auto propertiesComponent = owner->FindComponentByClass<UArmorPropertiesComponent>()) {
			propertiesComponent->ServerRemoveAllProperties();
		}
	}
}

void AArmorGearItemInstance::ActivateEquippedEffects() {
	Super::ActivateEquippedEffects();

	if (auto propertiesComponent = GetOwner()->FindComponentByClass<UArmorPropertiesComponent>()) {
		propertiesComponent->ServerActivateProperties(armorProperties, ItemPower);
	}
}

void AArmorGearItemInstance::DeactivateEquippedEffects() {
	Super::DeactivateEquippedEffects();

	if (auto propertiesComponent = GetOwner()->FindComponentByClass<UArmorPropertiesComponent>()) {
		propertiesComponent->ServerDeactivateProperties();
	}
}
