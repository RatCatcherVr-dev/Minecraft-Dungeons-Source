#include "PerfectFormAnimInstance.h"

#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MobCharacterMovementComponent.h"

void UPerfectFormAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	Super::NativeUpdateAnimation(deltaSeconds);

	if (ShouldUpdateTurnAngle()) {
		UpdateTurnAngle();
		UpdateRotationTime(deltaSeconds);
	}
}

void UPerfectFormAnimInstance::UpdateRotationTime(float deltaSeconds)
{
	if (!owningMobCharacter.IsValid()) {
		owningMobCharacter = Cast<AMobCharacter>(TryGetPawnOwner());
		// This update is also triggered when opening an AnimBP that utilises this class.
		// However at this point in editor the mob will still not be valid. <<Safe Guard>>
		if (!owningMobCharacter.IsValid())
			return;
	}
	if (!mobMovementComponent.IsValid()) {
		mobMovementComponent = owningMobCharacter->FindComponentByClass<UMobCharacterMovementComponent>();
	}

	float delta = mobMovementComponent->PathFollowingRotationRate.Yaw * deltaSeconds;
	rotationTime = delta > 0.0f ? (GetTurnAngleDelta() / delta) : 0.0f;
}

void UPerfectFormAnimInstance::UpdateTurnAngle()
{
	if (!owningMobCharacter.IsValid()) {
		owningMobCharacter = Cast<AMobCharacter>(TryGetPawnOwner());
		if (!owningMobCharacter.IsValid())
			return;
	}
	float yaw = owningMobCharacter->GetCapsuleComponent()->GetComponentRotation().Yaw;
	turnAngle = yaw - lastUpdateYaw;
	lastUpdateYaw = yaw;
}

float UPerfectFormAnimInstance::GetTurnAngleDelta() const
{
	return turnAngle < 0 ? -turnAngle : turnAngle;
}

bool UPerfectFormAnimInstance::ShouldUpdateTurnAngle() const
{
	return PerfectFormLocomotionBlend != EPerfectFormAnimLocomotion::Init;
}

bool UPerfectFormAnimInstance::IsTurning() const
{
	return !FMath::IsNearlyZero(turnAngle);
}
