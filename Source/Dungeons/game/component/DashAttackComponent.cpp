#include "Dungeons.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "NavigationPath.h"
#include "DashAttackComponent.h"
#include "util/FloatRange.h"
#include <NavigationSystem.h>

UDashAttackComponent::UDashAttackComponent() {
}

void UDashAttackComponent::BeginPlay() {
	Super::BeginPlay();
}

bool UDashAttackComponent::CanAttack(AActor* attackTarget) const {
	return Super::CanAttack(attackTarget) && IsWithinRange(attackTarget);
}

bool UDashAttackComponent::IsAtLocation(AActor* attackTarget) {
	if (attackTarget == nullptr) return true;
	return FVector::Distance(GetOwner()->GetActorLocation(), attackTarget->GetActorLocation()) <= AcceptableAtLocationRadius;
}

bool UDashAttackComponent::IsWithinRange(AActor* attackTarget) const {
	if (attackTarget == nullptr) {
		return false;
	}
	
	const auto from = GetOwner()->GetActorLocation();
	const auto to = attackTarget->GetActorLocation();
	auto distance = FVector::Distance(from, to);

	if (!AttackRange().inRange(distance)) {
		return false;
	}

	UNavigationSystemV1& NavSys = *UNavigationSystemV1::GetCurrent(GetWorld());
	UNavigationPath* path = NavSys.FindPathToLocationSynchronously(GetWorld(), GetOwner()->GetActorLocation(), attackTarget->GetActorLocation(), GetOwner());
	if (path == nullptr || !path->IsValid()) {
		return false;
	}
	if (path->GetPathLength() > maxAttackRangeRangeUnits) {
		return false;
	}

	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f, attackTarget->GetActorLocation(), (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly)) {
		if (hitResult.GetActor() != attackTarget) {
			return false;
		}
	}
	return true;
}

float UDashAttackComponent::GetAttackRange() const {
	return maxAttackRangeRangeUnits;
}

void UDashAttackComponent::SetMovmementComponentSpeedParams() {
	auto owner = Cast<ABaseCharacter>(GetOwner());
	auto characterMovementComponent = owner->GetCharacterMovement();
	characterMovementComponent->bForceMaxAccel = true;
	characterMovementComponent->MaxWalkSpeed = DashSpeed;
	characterMovementComponent->BrakingDecelerationWalking = 0.f;
}

void UDashAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	if (attackTarget == nullptr) {
		return;
	}
	auto owner = Cast<ABaseCharacter>(GetOwner());
	if (owner == nullptr) {
		return;
	}
	if (!IsWithinRange(attackTarget)) {
		return;
	}
	auto playerController = Cast<ABasePlayerController>(owner->GetController());
	if (playerController == nullptr) {
		return;
	}

	SetMovmementComponentSpeedParams();

	auto result = playerController->MoveToActor(attackTarget, AcceptableAtLocationRadius, true, false, false);

	AttackCounter++;
	SuccessfulAttackCounter++;
}

FloatRange UDashAttackComponent::AttackRange() const {
	return FloatRange(minAttackRangeRangeUnits, maxAttackRangeRangeUnits);
}
