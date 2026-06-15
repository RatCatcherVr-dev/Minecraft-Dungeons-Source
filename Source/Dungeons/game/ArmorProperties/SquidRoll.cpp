#include "Dungeons.h"
#include "SquidRoll.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"

USquidRollQuick::USquidRollQuick() {
	TypeID = EArmorPropertyID::SquidRollQuick;
}

void USquidRollQuick::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;

	if (CurrentActiveClouds < MaxCloudsAtSameTime) {
		ABlindCloud* newCloud = UBlindCloudSpawnHelper::SpawnBlindCloud(BlindCloudClass, BlindGameplayEffect, GetOwner(), GetOwner()->GetActorLocation(), Duration, true);
		newCloud->OnDestroyed.AddDynamic(this, &USquidRollQuick::OnCloudDestroyed);
		CurrentActiveClouds++;
	}
}

void USquidRollQuick::OnCloudDestroyed(AActor* DestroyedActor) {
	CurrentActiveClouds--;
}

void USquidRollQuick::BeginPlay() {
	Super::BeginPlay();
}

void USquidRollQuick::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
}


USquidRollLimited::USquidRollLimited() {
	TypeID = EArmorPropertyID::SquidRollLimited;
}

void USquidRollLimited::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority) return;

	if (CurrentActiveClouds < MaxCloudsAtSameTime) {
		ABlindCloud* newCloud = UBlindCloudSpawnHelper::SpawnBlindCloud(BlindCloudClass, BlindGameplayEffect, GetOwner(), GetOwner()->GetActorLocation(), Duration, true);
		newCloud->OnDestroyed.AddDynamic(this, &USquidRollLimited::OnCloudDestroyed);
		CurrentActiveClouds++;
	}
}

void USquidRollLimited::OnCloudDestroyed(AActor* DestroyedActor) {
	CurrentActiveClouds--;
}

void USquidRollLimited::BeginPlay() {
	Super::BeginPlay();
}

void USquidRollLimited::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
}