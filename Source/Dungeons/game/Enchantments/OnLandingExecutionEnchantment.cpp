// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "OnLandingExecutionEnchantment.h"


UOnLandingExecutionEnchantment::UOnLandingExecutionEnchantment() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UOnLandingExecutionEnchantment::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (const auto characterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		if (!characterMovement->IsFalling()) {
			if (bPlayerWasInAir) {
				OnLanded();
				bPlayerWasInAir = false;
			}
		}
		else {
			bPlayerWasInAir = true;
		}
	}
}
