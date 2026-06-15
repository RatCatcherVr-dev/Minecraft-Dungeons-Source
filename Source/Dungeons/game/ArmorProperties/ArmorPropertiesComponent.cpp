#include "Dungeons.h"
#include "ArmorProperty.h"
#include <GameplayPrediction.h>
#include "Net/UnrealNetwork.h"
#include "game/actor/character/BaseCharacter.h"
#include "ArmorPropertyType.h"
#include "ArmorPropertyTypeDefs.h"
#include "ArmorPropertiesComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "NoExportTypes.h"
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "util/Algo.hpp"


UArmorPropertiesComponent::UArmorPropertiesComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = false;
}

FLinearColor UArmorPropertiesComponent::DetermineArmorColor() {
	FLinearColor color;
	auto lastHighestPrio = std::numeric_limits<float>::max();

	for (auto property : Properties) {
		auto& colorPrio = property->GetPropertyColorPrio();

		if (colorPrio.Prio < lastHighestPrio) {
			color = colorPrio.getMultipliedColor();
			lastHighestPrio = colorPrio.Prio;
		}
	}

	return color;
}

void UArmorPropertiesComponent::ServerRemoveAllProperties_Implementation() {
	RemoveAllProperties();
}

bool UArmorPropertiesComponent::ServerRemoveAllProperties_Validate() {
	return true;
}

void UArmorPropertiesComponent::ServerAddProperties_Implementation(const TArray<FArmorPropertyData>& propertyData, float itemPower) {
	AddProperties(propertyData, itemPower);
}
bool UArmorPropertiesComponent::ServerAddProperties_Validate(const TArray<FArmorPropertyData>& propertyData, float itemPower) {
	return true;
}

void UArmorPropertiesComponent::ServerActivateProperties_Implementation(const TArray<FArmorPropertyData>& propertyData, float itemPower) {
	RemoveAllProperties();
	AddProperties(propertyData, itemPower);
}
bool UArmorPropertiesComponent::ServerActivateProperties_Validate(const TArray<FArmorPropertyData>& propertyData, float itemPower) {
	return true;
}

void UArmorPropertiesComponent::ServerDeactivateProperties_Implementation() {
	for (auto i = (Properties.Num() - 1); i >= 0; i--) {
		auto prop = Properties[i];
		if (prop->ShouldDeactivate()) {
			Properties.Remove(prop);
			prop->DestroyComponent();
		}
	}
}

bool UArmorPropertiesComponent::ServerDeactivateProperties_Validate() {
	return true;
}

void UArmorPropertiesComponent::RemoveAllProperties() {
	while (Properties.Num() > 0) {
		auto property = Properties.Last();
		Properties.Remove(property);
		property->DestroyComponent();
	}	
}

void UArmorPropertiesComponent::AddProperties(const TArray<FArmorPropertyData>& propertyData, const float itemPower) {
	for (const auto data : propertyData) {
		const auto& armorPropertyType = game::armorproperties::type::getArmorPropertyType(data.ID);
		const auto armorPropertyClass = armorPropertyType.GetClass();
		auto* armorProperty = NewObject<UArmorProperty>(GetOwner(), armorPropertyClass);
		armorProperty->SetItemPower(itemPower);
		armorProperty->RegisterComponent();
		Properties.Emplace(armorProperty);
	}
}

void UArmorPropertiesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UArmorPropertiesComponent, Properties);
}

UAbilitySystemComponent* UArmorPropertiesComponent::GetAbilitySystem() const {
	if (const auto owner = Cast<ABaseCharacter>(GetOwner())) {
		return owner->GetAbilitySystemComponent();
	}
	return nullptr;
}

TArray<UGearUtil*> UArmorPropertiesComponent::GetValidGearUtils() const {
	return algo::copy_if_map_tarray(Properties, RETLAMBDA(IsValid(it)), RETLAMBDA(Cast<UGearUtil>(it)));	
}
