// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ResurrectSurroundingMobs.h"
#include "game/actor/character/mob/MobCharacter.h"

UResurrectSurroundingMobs::UResurrectSurroundingMobs() {
	TypeId = EEnchantmentTypeID::ResurrectSurroundingMobs;
	ServerOnlyExecution = true;
	PrimaryComponentTick.bCanEverTick = bDebugVisuals;
}

void UResurrectSurroundingMobs::OnStart() {
	Super::OnStart();
	Area = NewObject<USphereComponent>(GetOwner());
	Area->SetGenerateOverlapEvents(true);
	Area->AttachTo(GetOwner()->GetRootComponent());
	Area->SetSphereRadius(ResurrectRadius);
	Area->SetCollisionObjectType(ECC_WorldDynamic);
	Area->SetCollisionResponseToAllChannels(ECR_Ignore);
	Area->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Area->OnComponentBeginOverlap.AddDynamic(this, &UResurrectSurroundingMobs::OnAreaOverlapBegin);
	Area->OnComponentEndOverlap.AddDynamic(this, &UResurrectSurroundingMobs::OnAreaOverlapEnd);
	Area->RegisterComponent();

	Area->UpdateOverlaps();
}

void UResurrectSurroundingMobs::OnEnd() {
	Super::OnEnd();
	if (Area) {
		TArray<AActor*> OverlappingMobs;
		Area->GetOverlappingActors(OverlappingMobs, AMobCharacter::StaticClass());
		for (auto actor : OverlappingMobs) {
			if (auto mob = Cast<AMobCharacter>(actor)) {
				mob->SetForceResurrection(false);
				mob->CancelResurrection();
			}
		}
		Area->DestroyComponent();
	}
}

void UResurrectSurroundingMobs::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), ResurrectRadius, 50, FColor::Red);
	SetComponentTickEnabled(bDebugVisuals);
}

void UResurrectSurroundingMobs::OnAreaOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherComp->IsA(UCapsuleComponent::StaticClass()) || OtherActor == GetOwner()) {
		return;
	}
	if (auto mob = Cast<AMobCharacter>(OtherActor)) {
		if (mob->IsAlive() && mob->IsTargetable() && !mob->IsFriendlyTowardsPlayers()) {
			if (bAlwaysTrigger || (FloatRange(1.f).random() <= ResurrectChance)) {
				mob->SetForceResurrection(true);
				mob->SetResurrectTime(ResurrectTime);
			}
		}
	}
}

void UResurrectSurroundingMobs::OnAreaOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!OtherComp->IsA(UCapsuleComponent::StaticClass())) {
		return;
	}
	if (auto mob = Cast<AMobCharacter>(OtherActor)) {
		if (mob->IsAlive() && mob->IsTargetable() && !mob->IsFriendlyTowardsPlayers()) {
			mob->SetForceResurrection(false);
			mob->SetResurrectTime(MobDefaultResurrectTime);
		}
	}
}
