#include "Dungeons.h"
#include "ThrowableItemInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"

AThrowableItemInstance::AThrowableItemInstance() {
}


FVector AThrowableItemInstance::CalculateVelocityAmountToLoc(FVector &location, float maxHeight) {
	auto player = Cast<APlayerCharacter>(GetOwner());
	FVector playerPos = player->GetActorLocation();
	FVector distance = location - playerPos;
	auto offsetHeight = location.Z - playerPos.Z;
	if (offsetHeight > maxHeight)
		maxHeight = offsetHeight + 10.0f;
	float gravity = player->GetWorld()->GetDefaultGravityZ() * -1;
	auto verticalSpeed = FMath::Sqrt(2 * gravity * maxHeight);
	auto TravelTime = FMath::Sqrt(2 * (maxHeight - offsetHeight) / gravity) + FMath::Sqrt(2 * maxHeight / gravity);
	auto horizontalSpeed = distance / TravelTime;

	FVector finalSpeed = horizontalSpeed;
	finalSpeed.Z = verticalSpeed;
	return finalSpeed;
}

void AThrowableItemInstance::TargetingEnded(FVector target, bool outcome) {
	if (CurrentTargetingActor.IsValid()) {
		CurrentTargetingActor->Destroy();
		CurrentTargetingActor = nullptr;
	}

	IsAiming = false;
	
	auto player = Cast<APlayerCharacter>(GetOwner());
	player->SetAttackState(EAttackState::None);
	if (outcome) {
		Target = target;
		SetTargetServer(target);
		Super::TryActivate();
	}
	else {
		OnItemActivationCompleted.Broadcast(false);
	}
}

void AThrowableItemInstance::ResetInstance() {
	Super::ResetInstance();

	TargetingEnded(FVector::ZeroVector, false);
}

bool AThrowableItemInstance::TryActivate() {
	if (IsAiming) {
		TargetingEnded(FVector::ZeroVector, false);
		return false;
	}
	else {
		auto player = Cast<APlayerCharacter>(GetOwner());

		FActorSpawnParameters sp;
		sp.Instigator = player;
		sp.Owner = player->GetController();
		sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector spawnLocation = FVector::ZeroVector;
		FRotator spawnRotation = FRotator::ZeroRotator;
		CurrentTargetingActor = Cast<ATargetingActor>(GetWorld()->SpawnActor(TargetingActorClass, &spawnLocation, &spawnRotation, sp));

		CurrentTargetingActor->OnTargetingEnded.BindUObject(this, &AThrowableItemInstance::TargetingEnded);
		player->SetAttackState(EAttackState::Targeting);
		return true;
	}
}

void AThrowableItemInstance::SetTargetServer_Implementation(FVector target) {
	Target = target;
}

bool AThrowableItemInstance::SetTargetServer_Validate(FVector target) {
	return true;
}
