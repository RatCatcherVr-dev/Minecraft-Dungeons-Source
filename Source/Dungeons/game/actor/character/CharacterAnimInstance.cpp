// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "CharacterAnimInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "util/Random.h"

ABaseCharacter* UCharacterAnimInstance::GetBaseCharacter() const
{
	USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent();
	return Cast<ABaseCharacter>(OwnerComponent->GetOwner());
}

AMobCharacter* UCharacterAnimInstance::GetMobCharacter() const 
{
	USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent();
	return Cast<AMobCharacter>(OwnerComponent->GetOwner());
}

APlayerCharacter* UCharacterAnimInstance::GetPlayerCharacter() const 
{
	USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent();
	return Cast<APlayerCharacter>(OwnerComponent->GetOwner());
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	static Random rnd;
	LocomotionAnimSpeedMultiplier = rnd.nextFloat(0.95f, 1.05f);
}

void UCharacterAnimInstance::NativeUpdateAnimation(float deltaSeconds) {
	const bool shouldLookAt = [&] {
		if (!LookAtActor.IsValid()) {
			return false;
		}

		const auto owner = GetOwningActor();

		return FVector::DotProduct(
			owner->GetActorForwardVector(),
			LookAtActor->GetActorLocation() - owner->GetActorLocation()
		) > 0.f;
	}();

	if (shouldLookAt) {
		const auto lookAtMultiplier { 2.f };
		StateMachineConditions.LookAtPoint = LookAtActor->GetActorLocation();
		StateMachineConditions.LookAtAlpha = FMath::Min(1.f, StateMachineConditions.LookAtAlpha + deltaSeconds * lookAtMultiplier);
	} else {
		const auto lookAwayMultiplier { .8f };
		StateMachineConditions.LookAtAlpha = FMath::Max(0.f, StateMachineConditions.LookAtAlpha - deltaSeconds * lookAwayMultiplier);
	}
}

void UCharacterAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();

	if (auto character = GetBaseCharacter())
	{
		FVector scale = character->GetActorScale();
		auto velocity = character->GetVelocity();
		float animSpeed = character->getIntendedAnimationSpeed();
		float maxSpeed = character->IsMoving() ? animSpeed : 0.f;

		StateMachineConditions.LocomotionVelocity = velocity;
		StateMachineConditions.LocomotionWalkRatio = maxSpeed / character->GetAnimationWalkSpeed();
		StateMachineConditions.RelativeMoveSpeed = maxSpeed / character->GetMaxSpeed();
		StateMachineConditions.LocomotionWalkVariedPlayRate = LocomotionAnimSpeedMultiplier * StateMachineConditions.LocomotionWalkRatio / scale.X;

		if (character->GetMovementComponent())
		{
			auto player = Cast<APlayerCharacter>(character);
			if (player && (player->GetAliveState() == EAliveState::Down || player->GetAliveState() == EAliveState::Dead)) {
				StateMachineConditions.FallBackLocomotionBlend = EAnimLocomotion::DownedDead;
			}
			else if (player && player->GetPlayerCharacterMovementComponent()->IsGliding()) {
				StateMachineConditions.FallBackLocomotionBlend = EAnimLocomotion::Gliding;
			}
			else if (player && player->GetPlayerCharacterMovementComponent()->IsDiving()) {
				StateMachineConditions.FallBackLocomotionBlend = EAnimLocomotion::Diving;
			}
			else if (character->GetMovementComponent()->IsFalling()) {
				StateMachineConditions.FallBackLocomotionBlend = EAnimLocomotion::Falling;
			}
			else if (character->GetMovementComponent()->IsFlying()) {
				StateMachineConditions.FallBackLocomotionBlend = EAnimLocomotion::Flying;
			} else {
				StateMachineConditions.FallBackLocomotionBlend = (StateMachineConditions.LocomotionWalkRatio > 0.02f) ? EAnimLocomotion::WalkRun : EAnimLocomotion::Idle;
			}
		}
		else
		{
			StateMachineConditions.FallBackLocomotionBlend = (StateMachineConditions.LocomotionWalkRatio > 0.02f) ? EAnimLocomotion::WalkRun : EAnimLocomotion::Idle;
		}
	}
}

void UCharacterAnimInstance::SetLookAtActor(AActor* target) {
	LookAtActor = target;
}

void UCharacterAnimInstance::ClearLookAtActor() {
	LookAtActor.Reset();
}
