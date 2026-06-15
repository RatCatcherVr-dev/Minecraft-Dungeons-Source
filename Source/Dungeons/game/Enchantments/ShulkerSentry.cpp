// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ShulkerSentry.h"
#include "util/Algo.hpp"
#include "util/CharacterQuery.h"
#include <UnrealNetwork.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include <Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h>
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


AFriendlyShulkerBullet::AFriendlyShulkerBullet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFriendlyShulkerBullet::Tick(float DeltaTime)
{
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, 1000).FilterByPredicate(predicate);
	if (targets.Num() > 0) {
		auto target = targets[FMath::RandRange(0, targets.Num() - 1)];
		SetTarget(target);
	}
}

void AFriendlyShulkerBullet::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const APlayerCharacter* hitCharacter = Cast<APlayerCharacter>(OtherActor))
	{
		return;
	}
	Super::OnHitboxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

AShulkerBulletShooter::AShulkerBulletShooter() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AShulkerBulletShooter::BeginPlay() {
	Super::BeginPlay();
}

void AShulkerBulletShooter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (CenterCharacter.IsValid()) {
		const FVector& CenterCharacterLocation = CenterCharacter->GetActorLocation();
		float currentTime = UGameplayStatics::GetTimeSeconds(GetWorld());

		FVector NewLocation(
			CenterCharacterLocation.X + (CenterOffset * FMath::Cos(currentTime * CirculationSpeedScale)),
			CenterCharacterLocation.Y + (CenterOffset * FMath::Sin(currentTime * CirculationSpeedScale)),
			CenterCharacterLocation.Z);

		SetActorLocation(NewLocation);
	}
}

void AShulkerBulletShooter::FireBulletAtTarget(AActor* target) {
	FTransform trans;
	trans.SetLocation(GetActorLocation());
	auto bullet = GetWorld()->SpawnActorDeferred<AFriendlyShulkerBullet>(FriendlyShulkerBulletClass, trans, CenterCharacter.Get(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	bullet->SetOwner(GetOwner());
	UGameplayStatics::FinishSpawningActor(bullet, trans);
	bullet->SetTarget(target);
}

void AShulkerBulletShooter::SetCenterCharacter(ABaseCharacter* character) {
	CenterCharacter = character;
}

TWeakObjectPtr<ABaseCharacter> AShulkerBulletShooter::GetCenterCharacter() const {
	return CenterCharacter;
}

void AShulkerBulletShooter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShulkerBulletShooter, CenterCharacter)
}

UShulkerSentry::UShulkerSentry() {
	TypeId = EEnchantmentTypeID::ShulkerSentry;
	LevelMultiplier = [this](int level) {
		return Math::max(MinInterval, BaseInterval + (float)(level - 1) * IntervalPerLevel);
	};
	MultiplierFormatter = valueformat::asEveryRoundedSecond;
}

void UShulkerSentry::OnStart() {
	Super::OnStart();
	if (GetOwner()->HasAuthority() && !ShulkerBulletShooter.IsValid()) {
		FTransform trans;
		trans.SetLocation(GetOwner()->GetActorLocation());
		ShulkerBulletShooter = GetWorld()->SpawnActorDeferred<AShulkerBulletShooter>(ShulkerBulletShooterClass, trans, GetOwner());
		ShulkerBulletShooter->SetCenterCharacter(GetCharacterOwner());
		UGameplayStatics::FinishSpawningActor(ShulkerBulletShooter.Get(), trans);
	}
}

void UShulkerSentry::OnEnd() {
	Super::OnEnd();
	if (GetOwner()->HasAuthority() && ShulkerBulletShooter.IsValid()) {
		ShulkerBulletShooter->Destroy();
	}
}

void UShulkerSentry::Execution() {
	if (GetOwner()->HasAuthority() && ShulkerBulletShooter.IsValid()) {
		auto characterOwner = GetCharacterOwner();
		const auto hostile = characterquery::is::hostile(characterOwner);
		const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

		TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);
		if (targets.Num() > 0) {
			auto target = targets[FMath::RandRange(0, targets.Num() - 1)];
			ShulkerBulletShooter->FireBulletAtTarget(target);
		}
	}
}

void UShulkerSentry::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UShulkerSentry, ShulkerBulletShooter);
}

float UShulkerSentry::GetExecutionInterval() const{
	return LevelMultiplier(Level);
}
