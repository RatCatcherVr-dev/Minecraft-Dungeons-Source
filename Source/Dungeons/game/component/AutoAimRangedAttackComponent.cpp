#include "Dungeons.h"
#include "AutoAimRangedAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "util/CharacterQuery.h"

AActor* UAutoAimRangedAttackComponent::GetAutoAimTarget(FVector& location) const {
	const auto attacker = Cast<ABaseCharacter>(GetOwner());

	const auto autoAimAngleRadians = FMath::DegreesToRadians(autoAimAngleDegrees);

	const auto hostile = characterquery::is::hostile(attacker);
	const auto predicate = [&](const AActor* v) {
		if (auto character = Cast<ABaseCharacter>(v)) {
			return hostile(character) && characterquery::is::targetable(character) && actorquery::is::alive(character);
		}
		return false;
	};

	const auto target = actorquery::getClosestActorInCone2D(attacker, ABaseCharacter::StaticClass(), autoAimRangeUnits, autoAimAngleRadians, true, predicate);

	if (target == nullptr) return nullptr;

	auto coneDirection = target->GetActorLocation() - attacker->GetActorLocation();
	coneDirection.Normalize();
	location = target->GetActorLocation();

	if (autoAimDebug) {
		DrawDebugCone(attacker->GetWorld(), attacker->GetActorLocation(), coneDirection, autoAimRangeUnits, autoAimAngleRadians, autoAimAngleRadians, 12, FColor::Emerald, false, 2.f);
		UE_LOG(LogTemp, Warning, TEXT("closest actor: %s"), target == nullptr ? *FString("NULL") : *target->GetName());
		DrawDebugLine(attacker->GetWorld(), attacker->GetActorLocation(), location, FColor::Red, true, 2.f);
		DrawDebugPoint(GetWorld(), attacker->GetActorLocation(), 3.f, FColor::White, false, 2.f);
		DrawDebugPoint(GetWorld(), location, 3.f, FColor::White, false, 2.f);
	}

	return target;
}

FVector UAutoAimRangedAttackComponent::CalculateAttackLocation(ABaseCharacter* attacker, const FVector& emitLocation, const FVector& emitDirection, AActor* attackTarget, const FRangedAttackProjectileSpawnDescription& definition, const FItemId& ammoType) const {
	if (attackTarget) {
		//If we have a target - try to intercept.
		const auto targetLocation = attackTarget->GetActorLocation();

		const auto calculateAimAtLocation = [targetLocation, emitLocation, distanceToTarget = FVector::Dist(targetLocation, emitLocation), targetVelocity = attackTarget->GetVelocity(), projectileSpeed = GetEstimatedProjectileSpeed(ammoType)]
			(FVector approximateAimAtLocation)
		{
			const auto approximateAimDirection = (approximateAimAtLocation - emitLocation).GetSafeNormal();
			const auto approximateProjectileVelocity = approximateAimDirection * projectileSpeed;
			const auto relativeSpeed = (approximateProjectileVelocity - targetVelocity).Size();
			const auto dt = FMath::IsNearlyZero(relativeSpeed)? 0 : distanceToTarget / relativeSpeed;
			return targetLocation + dt * targetVelocity;
		};

		auto estimatedTargetLocation = targetLocation;
		// Number of refinement/"integration" steps (of diminishing returns). A lower
		// iteration count would also benefit cases where the mob decreases its "sideway"
		// speed, as the estimation would be angled closer to the current target pos.
		//   The fixed point of this iterated function is the target position (where the
		// mob and projectile exactly meet), given the entities keeps their velocities.
		for (int i = 0; i < 4; ++i) {
			estimatedTargetLocation = calculateAimAtLocation(estimatedTargetLocation);
		}

		if (const auto mob = Cast<ACharacter>(attackTarget)) {
			const auto playerOffset = attacker->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			const auto mobOffset = mob->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			//Move target position so that player always shoots straight along the ground
			//If player and target is on the same height.
			//Use max halfmobcapsule * 2 so that we don't aim above very short enemies.
			estimatedTargetLocation.Z = estimatedTargetLocation.Z - mobOffset + Math::min(mobOffset*2.0f, playerOffset);
		}

		return estimatedTargetLocation;
	}
	//Otherwise, use auto aim to find target - but only use the default ranged attack logic.
	FVector outFoundLocation;
	attackTarget = GetAutoAimTarget(outFoundLocation);
	return Super::CalculateAttackLocation(attacker, emitLocation, emitDirection, attackTarget, definition, ammoType);
}

FVector UAutoAimRangedAttackComponent::GetAttackerBaseEmitVector(ABaseCharacter* attacker) const {
	if (const auto controller = attacker->GetController()) {
		return controller->GetControlRotation().Vector();
	}
	return Super::GetAttackerBaseEmitVector(attacker);
}

bool UAutoAimRangedAttackComponent::IsAutoAimEnabled() const {
	return autoAim;
}