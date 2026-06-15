#include "Dungeons.h"
#include "InitInvincibleComponent.h"
#include "HealthComponent.h"

UInitInvincibleComponent::UInitInvincibleComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = delay;
}

void UInitInvincibleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (firstTick) {
		if (auto* healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			healthComponent->Invincible = true;
		}
		
		firstTick = false;
	} else {
		if (auto* healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			healthComponent->Invincible = false;
		}

		SetComponentTickEnabled(false);
	}
}
