// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PlentifulQuiverInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "UnrealNetwork.h"

int APlentifulQuiverInstance::GetDisplayCount() const {
	return 0;
}

bool APlentifulQuiverInstance::CanActivate() const {
	return false;
}

void APlentifulQuiverInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetOwner();
	if (!HasAuthority())
		return;
	Super::Activate(predictionKey);
}

void APlentifulQuiverInstance::OnSetCanAddArrow_Implementation() {
	bCanAddArrow = true;
}

void APlentifulQuiverInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}



