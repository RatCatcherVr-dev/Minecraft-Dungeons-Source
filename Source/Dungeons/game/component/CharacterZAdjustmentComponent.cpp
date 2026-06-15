#include "Dungeons.h"
#include "CharacterZAdjustmentComponent.h"

#include "game/component/TeleportComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "Components/CapsuleComponent.h"
#include "game/actor/character/BaseCharacter.h"

static constexpr float halfTimeToDtScalar(float halftime) {
	return 0.5f / halftime;
}

UCharacterZAdjustmentComponent::UCharacterZAdjustmentComponent()
	: previousPosition(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterZAdjustmentComponent::BeginPlay() {
	Super::BeginPlay();

	if (const auto teleportComponent = GetOwner()->FindComponentByClass<UTeleportComponent>()) {
		teleportComponent->OnTeleported.AddUObject(this, &UCharacterZAdjustmentComponent::ResetPosition);
	}

	if (const auto ragdollComponent = GetOwner()->FindComponentByClass<URagdollOnDeathComponent>()) {
		ragdollComponent->OnRagdollChanged.AddUObject(this, &UCharacterZAdjustmentComponent::ResetPosition);
	}

	if (const auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->OnWorldStateChanged.AddUObject(this, &UCharacterZAdjustmentComponent::ResetPosition);
	}

	ResetPosition();
}

void UCharacterZAdjustmentComponent::ResetPosition() {
	SetMeshRelativeZ(GetMeshRelativeZ());
}

float UCharacterZAdjustmentComponent::GetMeshRelativeZ() const {
	return -GetCapsule()->GetScaledCapsuleHalfHeight();
}

void UCharacterZAdjustmentComponent::SetMeshRelativeZ(float RelativeZ) {
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, RelativeZ));
	previousZ = GetMesh()->GetComponentLocation().Z;
}

void UCharacterZAdjustmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto* character = Cast<ABaseCharacter>(GetOwner());

	const auto currentPosition = character->GetActorLocation();
	const auto tmpLastPosition = previousPosition;
	previousPosition = currentPosition;

	// If we've moved too much, we've probably teleported and should reset the Z-smoothing.
	if (FVector::DistSquared2D(currentPosition, tmpLastPosition) > 1000 * 1000) {
		ResetPosition();
		return;
	}
	
	const bool shouldTick = [&]() {
		const bool probablyNot = character->GetRemoteRole() == ENetRole::ROLE_AutonomousProxy && IsNetMode(NM_ListenServer);
		const bool inWorld = character->GetWorldState() == ECharacterWorldState::InWorld;

		return inWorld && probablyNot && (!character || (character && !character->IsLocallyControlled()));
	}();

	if (shouldTick) {
		return;
	}
	if (character->GetCharacterMovement()->IsMovingOnGround()) {
		const float targetWorldZ = GetCapsule()->GetComponentLocation().Z + GetMeshRelativeZ();
		const float deltaZ = previousZ - targetWorldZ;

		if (!FMath::IsNearlyZero(deltaZ, 0.0001f)) {
			//Old position is changing too much for us to allow the jitter!
			//Retain old position and fade it out over time.
			const float ZSmoothing = deltaZ * (1.f - FMath::Min(1.f, DeltaTime * halfTimeToDtScalar(halfTimeSeconds)));
			const float RelativeZ = ZSmoothing + GetMeshRelativeZ();
			SetMeshRelativeZ(RelativeZ);
		}
	}
	previousZ = GetMesh()->GetComponentLocation().Z;
}

// note: reason for not caching in BeginPlay is because the client version (ResetPosition) 
// can be called before BeginPlay (via APlayerCharacter::ClientOnPlayerTeleported_Implementation)
UCapsuleComponent* UCharacterZAdjustmentComponent::GetCapsule() const {
	if (capsule == nullptr) {
		capsule = GetOwner()->FindComponentByClass<UCapsuleComponent>();
		check(capsule != nullptr && "Capsule component cannot be null.");
	}
	return capsule;
}

USkeletalMeshComponent* UCharacterZAdjustmentComponent::GetMesh() const {
	if (mesh == nullptr) {
		mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
		check(mesh != nullptr && "Mesh component cannot be null");
	}
	return mesh;
}
