// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/actor/item/BaseProjectile.h"
#include "GrowingArrowComponent.h"
#include "Assets/DungeonsAssetManager.h"
#include "DungeonsGameInstance.h"

UGrowingArrowComponent::UGrowingArrowComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

 
void UGrowingArrowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto owner = Cast<ABaseProjectile>(GetOwner());

	FVector newLocation = owner->GetActorLocation();

	DistanceTraveled += FVector::Dist(newLocation, LastLocation);
	LastLocation = newLocation;
	auto currentAlpha = GetCurrentAlpha();
	auto scale = FMath::Lerp(1.f, MaxScale, currentAlpha);
	owner->SetActorRelativeScale3D(FVector(scale));

	owner->SetDamageFactorMultiplier(1.f + (currentAlpha * Multiplier));

	if (scale == MaxScale) {
		SetComponentTickEnabled(false);
	}
}

void UGrowingArrowComponent::Activate(bool reset) {
	Super::Activate(reset);

	LastLocation = GetOwner()->GetActorLocation();
	DistanceTraveled = 0.f;
	SetComponentTickEnabled(true);
}

void UGrowingArrowComponent::BeginPlay() 
{
	Super::BeginPlay();
}

void UGrowingArrowComponent::SetMultiplier(float multiplier) {
	Multiplier = multiplier;
}

float UGrowingArrowComponent::GetCurrentAlpha() const {
	return Math::clamp(DistanceTraveled / MaxDistance, 0.0f, 1.0f);
}