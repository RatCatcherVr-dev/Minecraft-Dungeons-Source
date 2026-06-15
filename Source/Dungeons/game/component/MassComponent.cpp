#include "Dungeons.h"
#include "MassComponent.h"

UMassComponent::UMassComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

bool UMassComponent::IsImmovable() const {
	return immovable;
}

float UMassComponent::GetPushMultiplier() const {
	return pushMultiplier * additionalPushMultiplier;
}

void UMassComponent::SetAdditionalMultiplier(float multiplier)
{
	additionalPushMultiplier = multiplier;
}
