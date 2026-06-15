#include "Dungeons.h"
#include "GlobalHealthBarComponent.h"
#include "game/component/HealthBarComponent.h"
#include "HealthComponent.h"
#include <UnrealNetwork.h>
#include <LogMacros.h>

UGlobalHealthBarComponent::UGlobalHealthBarComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UGlobalHealthBarComponent::BeginPlay() {
	Super::BeginPlay();

	HealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	if (HealthComponent->IsAlive())
	{
		OnAlive();
	}

	OnDeathHandle = HealthComponent->OnDeath.AddUObject(this, &UGlobalHealthBarComponent::OnDeath_Internal);
	OnHealthFractionChangedHandle = HealthComponent->OnHealthFractionChanged.AddUObject(this, &UGlobalHealthBarComponent::OnHealtFractionChanged);
	
	OnValueChanged.Broadcast(HealthComponent->GetCurrentHealthPercentage());

	if (!GetOwner()->HasAuthority()){
		PrimaryComponentTick.bCanEverTick = false;
	}

	if (auto HealthBar = GetOwner()->FindComponentByClass<UHealthBarComponent>()) {
		HealthBar->SetDisplayedGlobally(true);
	}
}

void UGlobalHealthBarComponent::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	HealthComponent->OnDeath.Remove(OnDeathHandle);
	HealthComponent->OnHealthFractionChanged.Remove(OnHealthFractionChangedHandle);
	OnDeath_Internal();
}

void UGlobalHealthBarComponent::OnDeath_Internal() {
	OnDeath();
	if (auto HealthBar = GetOwner()->FindComponentByClass<UHealthBarComponent>()) {
		HealthBar->SetDisplayedGlobally(false);
	}
}

void UGlobalHealthBarComponent::OnHealtFractionChanged(float newFraction, float oldFraction) {
	if (newFraction != oldFraction) {
		OnValueChanged.Broadcast(newFraction);
	}
}
