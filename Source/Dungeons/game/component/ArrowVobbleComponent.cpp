// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ArrowVobbleComponent.h"
#include <Kismet/KismetMathLibrary.h>


UArrowVobbleComponent::UArrowVobbleComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UArrowVobbleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bCanVobble) {
		if (TargetComponent.IsValid()) {
			float PassedTimeClamped = Math::clamp(UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeStamp, 0.0f, 1.0f);
			float rotationMagnitude = (PassedTimeClamped - 1.0f) / VobbleTimeSeconds;

			float randomizedOffset = FMath::RandRange((VobbleScale * -1), VobbleScale);

			float rotation = rotationMagnitude * randomizedOffset;

			auto newRotation = UKismetMathLibrary::ComposeRotators(InitialRotation, FRotator(rotation, rotation, rotation));;
		
			TargetComponent->SetRelativeRotation(newRotation);
		}
	}
}

void UArrowVobbleComponent::BeginPlay() {
	Super::BeginPlay();
	if (auto rootComp = GetOwner()->GetRootComponent()) {
		TargetComponent = rootComp;
	}
}

void UArrowVobbleComponent::StartVobble() {
	bCanVobble = true;
	StartTimeStamp = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	if (TargetComponent.IsValid()) {
		InitialRotation = TargetComponent->RelativeRotation;
	}
}