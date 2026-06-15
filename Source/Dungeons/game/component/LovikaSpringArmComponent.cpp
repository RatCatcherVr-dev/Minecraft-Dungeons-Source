// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "LovikaSpringArmComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "RagdollOnDeathComponent.h"
#include "TeleportComponent.h"

void ULovikaSpringArmComponent::BeginPlay() {
	Super::BeginPlay();

	if (const auto teleportComponent = GetOwner()->FindComponentByClass<UTeleportComponent>()) {
		teleportComponent->OnTeleported.AddUObject(this, &ULovikaSpringArmComponent::Reinitialize);
	}

	InternalTargetLength = SeekArmLength;
}
 
void ULovikaSpringArmComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (FMath::Abs(InternalTargetLength - TargetArmLength) > 0.05f) {
		TargetArmLength = FMath::FInterpTo(TargetArmLength, InternalTargetLength, DeltaTime, ArmLagSpeed);
	}

	UpdateSnapAmount(DeltaTime);
}

void ULovikaSpringArmComponent::UpdateSnapAmount(float DeltaTime) {
	if (XYSnapAmount <= 1.0f) {
		XYSnapAmount += FMath::Min(1.0f, DeltaTime / XYSnapRestoreDuration);
	}
}

void ULovikaSpringArmComponent::SetXYSnapAmount(float snapAmount, float snapRestoreDuration) {
	XYSnapAmount = snapAmount;
	XYSnapRestoreDuration = snapRestoreDuration;
}

void ULovikaSpringArmComponent::Reinitialize() {
	TargetArmLength = InternalTargetLength;
	PreviousDesiredLoc = GetComponentLocation() + TargetOffset;
	UpdateDesiredArmLocation(false, false, false, 0.0f);
}

void ULovikaSpringArmComponent::ResetDesiredArmLength() {
	InternalTargetLength = SeekArmLength;
}

void ULovikaSpringArmComponent::SetDesiredArmLegnth(float Length) {
	InternalTargetLength = Length;
}

void ULovikaSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) {
	FRotator DesiredRot = GetComponentRotation();

	// If inheriting rotation, check options for which components to inherit
	if (!bAbsoluteRotation) {
		if (!bInheritPitch) {
			DesiredRot.Pitch = RelativeRotation.Pitch;
		}

		if (!bInheritYaw) {
			DesiredRot.Yaw = RelativeRotation.Yaw;
		}

		if (!bInheritRoll) {
			DesiredRot.Roll = RelativeRotation.Roll;
		}
	}

	const float InverseCameraLagMaxTimeStep = (1.f / CameraLagMaxTimeStep);

	// Apply 'lag' to rotation if desired
	if (bDoRotationLag) {
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f) {
			const FRotator ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (CameraLagMaxTimeStep / DeltaTime);
			FRotator LerpTarget = PreviousDesiredRot;
			float RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER) {
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmRotStep * (LerpAmount * InverseCameraLagMaxTimeStep);
				RemainingTime -= LerpAmount;

				DesiredRot = FMath::RInterpTo(PreviousDesiredRot, LerpTarget, LerpAmount, CameraRotationLagSpeed);
				PreviousDesiredRot = DesiredRot;
			}
		} else {
			DesiredRot = FMath::RInterpTo(PreviousDesiredRot, DesiredRot, DeltaTime, CameraRotationLagSpeed);
		}
	}
	PreviousDesiredRot = DesiredRot;

	// Get the spring arm 'origin', the target we want to look at
	FVector ArmOrigin = GetComponentLocation() + TargetOffset;
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector DesiredLoc = ArmOrigin;
	if (bDoLocationLag) {
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraLagSpeed > 0.f) {
			const FVector ArmMovementStep = (ArmOrigin - PreviousArmOrigin) * (CameraLagMaxTimeStep / DeltaTime);
			FVector LerpTarget = PreviousArmOrigin;
			float RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER) {
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * (LerpAmount * InverseCameraLagMaxTimeStep);
				RemainingTime -= LerpAmount;

				DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, CameraLagSpeed);				
				DesiredLoc.X += (ArmOrigin.X - DesiredLoc.X) * XYSnapAmount;
				DesiredLoc.Y += (ArmOrigin.Y - DesiredLoc.Y) * XYSnapAmount;
				PreviousDesiredLoc = DesiredLoc;
			}
		} else {
			DesiredLoc = FMath::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
			DesiredLoc.X += (ArmOrigin.X - DesiredLoc.X) * XYSnapAmount;
			DesiredLoc.Y += (ArmOrigin.Y - DesiredLoc.Y) * XYSnapAmount;			
		}

		// Clamp distance if requested
		bool bClampedDist = false;
		if (CameraLagMaxDistance > 0.f) {
			const FVector FromOrigin = DesiredLoc - ArmOrigin;
			if (FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance)) {
				DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
				bClampedDist = true;
			}
		}
/*
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (bDrawDebugLagMarkers) {
			DrawDebugSphere(GetWorld(), ArmOrigin, 5.f, 8, FColor::Green);
			DrawDebugSphere(GetWorld(), DesiredLoc, 5.f, 8, FColor::Yellow);

			const FVector ToOrigin = ArmOrigin - DesiredLoc;
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc, DesiredLoc + ToOrigin * 0.5f, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), DesiredLoc + ToOrigin * 0.5f, ArmOrigin, 7.5f, bClampedDist ? FColor::Red : FColor::Green);
		}
#endif
*/
	}

	PreviousArmOrigin = ArmOrigin;
	PreviousDesiredLoc = DesiredLoc;

	// Now offset camera position back along our rotation
	DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
	// Add socket offset in local space
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);

	// Do a sweep to ensure we are not penetrating the world
	FVector ResultLoc;
	if (bDoTrace && (TargetArmLength != 0.0f)) {
		static FName TraceTagName(TEXT("SpringArm"));
		FCollisionQueryParams QueryParams(TraceTagName, false, GetOwner());

		FHitResult Result;
		GetWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);

		ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);
	} else {
		ResultLoc = DesiredLoc;
	}

	// Form a transform for new world transform for camera
	FTransform WorldCamTM(DesiredRot, ResultLoc);
	// Convert to relative to component
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

	// Update socket location/rotation
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();

	UpdateChildTransforms();
}