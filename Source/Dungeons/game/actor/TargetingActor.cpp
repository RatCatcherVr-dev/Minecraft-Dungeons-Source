
#include "Dungeons.h"
#include "TargetingActor.h"
#include <Engine.h>


void ATargetingActor::BeginPlay() {
	Super::BeginPlay();

	if (!TargetActor.IsValid()) {
		auto playerController = Cast<APlayerController>(GetOwner());
		playerController->InputComponent->BindAxis("MoveRightGamepad", this, &ATargetingActor::OnAxisX);
		playerController->InputComponent->BindAxis("MoveForwardGamepad", this, &ATargetingActor::OnAxisY);
	}
}

void ATargetingActor::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	auto playerController = Cast<APlayerController>(GetOwner());

	auto cursor = playerController->GetMouseCursor();

	if (TargetActor.IsValid()) {
		UpdateToTargetActorLocation();
	}
	else {
		if (cursor != EMouseCursor::None) {
			AdjustCursorToMouse(playerController);
		}
		else {
			TryOffsetFromGamePadAxes(playerController, deltaTime);
		}
	}
}

void ATargetingActor::SetTargetActor(AActor* target) {
	TargetActor = target;
}

TWeakObjectPtr<AActor> ATargetingActor::GetTargetActor() const {
	return TargetActor;
}

void ATargetingActor::AdjustCursorToMouse(APlayerController* playerController) {
	FHitResult hit;
	if (playerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery11, true, hit)) {
		SetActorLocation(hit.Location);
		CurrentTarget = hit.Location;
	}
}

void ATargetingActor::TargetSelected(FVector target) {
	if (OnTargetingEnded.IsBound()) {
		OnTargetingEnded.Execute(target, true);
	}
}

void ATargetingActor::TargetingCanceled() {
	if (OnTargetingEnded.IsBound()) {
		OnTargetingEnded.Execute(FVector::ZeroVector, false);
	}
}

void ATargetingActor::TryOffsetFromGamePadAxes(APlayerController* playerController, float deltaTime) {
	auto pawn = playerController->GetPawnOrSpectator();
	const auto& currentLocation = GetActorLocation();

	SetActorLocation(FVector(currentLocation.X, currentLocation.Y, pawn->GetActorLocation().Z + 1000.0f));

	FVector offset = GetCameraOrientedTiltDirection(pawn) * MoveSpeed * deltaTime;

	AddActorWorldOffset(offset);
	SetActorLocation(FindBelowGeometry());
}

void ATargetingActor::UpdateToTargetActorLocation() {
	const auto& targetActorLocation = TargetActor->GetActorLocation();
	SetActorLocation(FVector(targetActorLocation.X, targetActorLocation.Y, targetActorLocation.Z + 1000.0f));
	SetActorLocation(FindBelowGeometry());
}

FVector ATargetingActor::GetCameraOrientedTiltDirection(APawn* pawn) const {
	if (auto camera = pawn->FindComponentByClass<UCameraComponent>()) {
		auto cameraRight = camera->GetRightVector();
		auto cameraForward = camera->GetForwardVector();

		auto XDir = FVector(cameraRight.X, cameraRight.Y, 0) * AxisX;
		auto YDir = FVector(cameraForward.X, cameraForward.Y, 0) * AxisY;

		return XDir + YDir;
	}

	return FVector::ZeroVector;
}

FVector ATargetingActor::FindBelowGeometry() const {
	FHitResult hit;
	const FVector& location = GetActorLocation();
	if (GetWorld()->LineTraceSingleByChannel(hit, location, location - FVector(0, 0, 20000.0f), ECollisionChannel::ECC_WorldStatic)) {
		return hit.Location;
	}
	return FVector::ZeroVector;
}

void ATargetingActor::OnAxisX(float value) {
	AxisX = value;
	if (value > 0.0f || value < 0.0f) {
		TargetActor.Reset();
	}
}

void ATargetingActor::OnAxisY(float value) {
	AxisY = value;
	if (value > 0.0f || value < 0.0f) {
		TargetActor.Reset();
	}
}
