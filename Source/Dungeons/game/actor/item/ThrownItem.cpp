#include "Dungeons.h"
#include "ThrownItem.h"
#include <Sound/SoundCue.h>
#include "UnrealNetwork.h"

AThrownItem::AThrownItem(const class FObjectInitializer& OI) : Super(OI) {
	//splashDamage = 50;
	bReplicates = true;
	bReplicateMovement = true;
}

void AThrownItem::BeginPlay() {
	Super::BeginPlay();

	auto collisionMesh = FindComponentByClass<USphereComponent>();
	if (collisionMesh) {
		collisionMesh->SetGenerateOverlapEvents(true);
		collisionMesh->SetMobility(EComponentMobility::Movable);
		collisionMesh->SetSimulatePhysics(true);
		collisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		collisionMesh->SetEnableGravity(true);
		collisionMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		collisionMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
		auto staticMesh = FindComponentByClass<UMeshComponent>();
		staticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		collisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AThrownItem::OnOverlapBegin);
	}
}

void AThrownItem::OnHitObject_Implementation(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FVector actorLocation = GetActorLocation();
	if (ImpactEffect != nullptr)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, actorLocation);
	if (ImpactSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, actorLocation);

	/*if (Role == ROLE_SimulatedProxy)
		return;

	auto owner = GetOwner();
	for (auto actor : actorquery::getNearbyActors(this, 600, AMobCharacter::StaticClass())) {
		auto hc = actor->FindComponentByClass<UHealthComponent>();
		if (hc == nullptr)
			continue;
		hc->ApplyDamage(splashDamage, owner, owner, owner->GetActorLocation(), actor->GetActorLocation());
	}
	Destroy();*/
}

void AThrownItem::OnOverlapBegin(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherComp->GetCollisionObjectType() != ECC_WorldStatic)
		return;
	if (OtherActor->Role == ROLE_SimulatedProxy) return;
	OnHitObject(ThisComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AThrownItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


