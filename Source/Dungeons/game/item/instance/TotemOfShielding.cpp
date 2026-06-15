// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TotemOfShielding.h"
#include "util/CharacterQuery.h"
#include "game/util/ComponentUtils.h"
#include "Net/UnrealNetwork.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/power/ItemPowerEffectDefs.h"

ATotemOfShielding::ATotemOfShielding() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ATotemOfShielding::BeginPlay() {
	Super::BeginPlay();
	if (auto torusMesh = componentutils::GetComponentOfName<UStaticMeshComponent>("CollisionTorus", this)) {
		torusMesh->OnComponentBeginOverlap.AddDynamic(this, &ATotemOfShielding::OnBeginTorusOverlap);
	}

	if (auto roofCollider = FindComponentByClass<UBoxComponent>()) {
		roofCollider->OnComponentBeginOverlap.AddDynamic(this, &ATotemOfShielding::OnBeginTorusOverlap);

	}
	GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &ATotemOfShielding::TryStartDestroyCountdown, Duration);
}

void ATotemOfShielding::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	TryNotifyWeakenedVitality();
}


void ATotemOfShielding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATotemOfShielding, Duration);
}

void ATotemOfShielding::OnDestroyCountdownStarted_Internal() {
	Super::OnDestroyCountdownStarted_Internal();
	if (auto torusMesh = componentutils::GetComponentOfName<UStaticMeshComponent>("CollisionTorus", this)) {
		torusMesh->OnComponentBeginOverlap.RemoveDynamic(this, &ATotemOfShielding::OnBeginTorusOverlap);
		torusMesh->SetGenerateOverlapEvents(false);
	}

	if (auto roofCollider = FindComponentByClass<UBoxComponent>()) {
		roofCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ATotemOfShielding::OnBeginTorusOverlap);
		roofCollider->SetGenerateOverlapEvents(false);

	}
}

float ATotemOfShielding::GetHealthFraction() {
	if (DestructionTimerHandle.IsValid()) {
		const float timeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(DestructionTimerHandle);
		return timeRemaining / Duration;
	}
	return 1.0f;
}



bool ATotemOfShielding::CanBeDamagedBy(ABaseProjectile* projectile) {
	const auto* totemOwner = Cast<ABaseCharacter>(GetOwner());
	const auto* projectileOwner = Cast<ABaseCharacter>(projectile->GetInstigator());
	if (!projectileOwner) {
		return true;
	}
	if (totemOwner) {
		const auto hostile = characterquery::is::hostile(totemOwner);

		const auto predicate = [=](const ABaseCharacter* targetCandidate) {
			return  hostile(targetCandidate) &&
				characterquery::is::targetable(targetCandidate) &&
				actorquery::is::alive(targetCandidate);

			return false;
		};

		return predicate(projectileOwner);
	}
	return false;
}

void ATotemOfShielding::OnBeginTorusOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (auto projectile = Cast<ABaseProjectile>(OtherActor)) {
		if (CanBeDamagedBy(projectile)) {

			OnProjectileImpact_Internal(projectile);
			OnProjectileImpact(projectile, projectile->GetActorLocation());
		}
	}
}

ATotemOfShieldingInstance::ATotemOfShieldingInstance()
{
	PowerEffects = { UDurationIncrease::StaticClass() };
}


void ATotemOfShieldingInstance::PreTotemBeginPlay(ATotemBaseActor* totemActor) {
	Super::PreTotemBeginPlay(totemActor);
	const auto& itemType = GetItemType();
	if (auto shieldTotem = Cast<ATotemOfShielding>(totemActor)) {
		shieldTotem->Power = ItemPower;
		shieldTotem->Duration = itemType.getDurationSeconds() * GetPowerEffect()->GetMultiplier(ItemPower);
	}
}
