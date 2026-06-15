// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Soul.h"
#include "game/component/SoulComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/component/MoveToTargetMovementComponent.h"
#include "game/actor/ProjectileManager.h"


ASoul::ASoul() {
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(false);
	SetReplicateMovement(false);
}

void ASoul::LaunchSoul() {
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);

	OnLaunchSoul();
}

void ASoul::StopSoul(bool PushToStore)
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	OnStopSoul();

	if (PushToStore)
	{
		//Push the soul back to the pool
		AProjectileActorManager::Push_Soul(this);
	}
}

void ASoul::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	float elapsed = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTime;
	float alpha = elapsed / TravelTime;
	if (alpha < 1.f) {
		auto riseAlpha = elapsed / RiseTime;
		auto lerpedRise = (riseAlpha <= 1.f) ? FMath::Lerp(SpawnPosition, RiseToPosition, riseAlpha) : RiseToPosition;

		SetActorLocation(FMath::Lerp(lerpedRise, GetOwner()->GetActorLocation(), FMath::InterpEaseIn(0.f, 1.f, alpha, 5)));
	}
	else {
		OnShouldAbsorb();
		StopSoul();
	}
}

void ASoul::Initialize(FVector spawnPos, FVector risePos, float travelTime, float riseTime) {
	StartTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	SpawnPosition = std::move(spawnPos);
	RiseToPosition = risePos;
	TravelTime = travelTime;
	RiseTime = riseTime;
}