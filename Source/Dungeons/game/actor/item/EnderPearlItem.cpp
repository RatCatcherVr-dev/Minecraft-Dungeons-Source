#include "Dungeons.h"
#include "EnderPearlItem.h"

void AEnderPearlItem::BeginPlay() {
	Super::BeginPlay();
}

void AEnderPearlItem::OnHitObject(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnHitObject(ThisComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (Role == ROLE_SimulatedProxy)
		return;
	if (OtherComp->GetCollisionObjectType() != ECC_WorldStatic)
		return;
	if (OtherActor->Role == ROLE_SimulatedProxy) return;

	auto owner = GetOwner();
	FVector teleportLocation = GetActorLocation();
	teleportLocation.Z += 100.0f;
	owner->SetActorLocation(teleportLocation);
	Destroy();
}

