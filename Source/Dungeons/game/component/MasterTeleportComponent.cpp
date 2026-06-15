#include "Dungeons.h"
#include "game/actor/character/BaseCharacter.h"
#include "MasterTeleportComponent.h"
#include "Dungeons/DungeonsGameInstance.h"

UMasterTeleportComponent::UMasterTeleportComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	PrimaryComponentTick.TickInterval = 1.f;
}

void UMasterTeleportComponent::TickComponent(float, ELevelTick, FActorComponentTickFunction*) {
	const auto owner = Cast<ABaseCharacter>(GetOwner());
	if (const auto master = owner->GetMaster()) {
		const auto masterLocation = master->GetActorLocation();

		if (
			master->GetWorldState() == ECharacterWorldState::InWorld &&
			master->GetHealthComponent()->IsAlive() &&
			!master->GetMovementComponent()->IsFalling() &&
			FVector::DistSquared2D(owner->GetActorLocation(), masterLocation) > FMath::Square(2500.f)
			) {
			owner->SetActorLocation(masterLocation, false, nullptr, ETeleportType::ResetPhysics);
		}
	}
}