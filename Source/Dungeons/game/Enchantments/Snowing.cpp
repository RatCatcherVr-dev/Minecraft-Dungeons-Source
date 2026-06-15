// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Snowing.h"
#include "util/Algo.hpp"
#include "util/CharacterQuery.h"
#include <UnrealNetwork.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include <Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h>
#include "../actor/character/player/PlayerCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

ASnowBall::ASnowBall() {
	SetReplicates(true);
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	Movement->SetIsReplicated(true);
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->InitialSpeed = 3000.f;
	Movement->MaxSpeed = 3000.f;
	Movement->ProjectileGravityScale = 0.f;
}

void ASnowBall::BeginPlay() {
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &ASnowBall::OnOverlapBegin);
}

void ASnowBall::SetThrowerOwner(class ASnowBallThrower* owner) {
	ThrowerOwner = owner;
}

void ASnowBall::SetStunDuration(float stunDuration) {
	StunDuration = stunDuration;
}

void ASnowBall::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor) {
	if (const auto* otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
		if (auto characterOwner = Cast<ABaseCharacter>(GetOwner())) {
			const auto hostile = characterquery::is::hostile(characterOwner);
			if (hostile(otherCharacter)) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
				auto targetAbilitySystem = otherCharacter->GetAbilitySystemComponent();

				FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(ownerAbilitySystem, Effect));
				spec.GetContext().AddOrigin(GetActorLocation());
				spec.SetSetByCallerMagnitude(effects::DurationName, StunDuration);
				ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);

				FGameplayCueParameters params;
				params.Location = GetActorLocation();
				ownerAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Snowing"), params);
				Destroy();
			}
		}
	}
}

ASnowBallThrower::ASnowBallThrower() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASnowBallThrower::BeginPlay() {
	Super::BeginPlay();
}

void ASnowBallThrower::Tick(float DeltaTime) {
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

void ASnowBallThrower::ShootSnowBallAtTarget(AActor* target, float SnowBallStunDuration) {
	FTransform trans;
	trans.SetLocation(GetActorLocation());
	auto snowball = GetWorld()->SpawnActorDeferred<ASnowBall>(SnowBallClass, trans, CenterCharacter.Get(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	snowball->SetStunDuration(SnowBallStunDuration);
	snowball->SetThrowerOwner(this);
	if (auto movement = snowball->FindComponentByClass<UProjectileMovementComponent>()) {
		const FVector delta = target->GetActorLocation() - GetActorLocation();
		FVector dir = delta;
		dir.Normalize();

		movement->Velocity = dir * 2000.0f;
	}
	UGameplayStatics::FinishSpawningActor(snowball, trans);
}

void ASnowBallThrower::SetCenterCharacter(ABaseCharacter* character) {
	CenterCharacter = character;
}

TWeakObjectPtr<ABaseCharacter> ASnowBallThrower::GetCenterCharacter() const {
	return CenterCharacter;
}

void ASnowBallThrower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASnowBallThrower, CenterCharacter)
}

USnowing::USnowing() {
	TypeId = EEnchantmentTypeID::Snowing;
	LevelMultiplier = [this](int level) {
		return Math::max(MinInterval, BaseInterval + (float)(level - 1) * IntervalPerLevel);
	};
	MultiplierFormatter = valueformat::asEveryRoundedSecond;
}

void USnowing::OnStart() {
	Super::OnStart();
	if (GetOwner()->HasAuthority() && !SnowballThrower.IsValid()) {
		FTransform trans;
		trans.SetLocation(GetOwner()->GetActorLocation());
		SnowballThrower = GetWorld()->SpawnActorDeferred<ASnowBallThrower>(SnowBallThrowerClass, trans, GetOwner());
		SnowballThrower->SetCenterCharacter(GetCharacterOwner());
		UGameplayStatics::FinishSpawningActor(SnowballThrower.Get(), trans);
	}
}

void USnowing::OnEnd() {
	Super::OnEnd();
	if (GetOwner()->HasAuthority()) {
		if (SnowballThrower.IsValid()) {
			SnowballThrower->Destroy();
		}
	}
}

void USnowing::Execution() {
	if (GetOwner()->HasAuthority() && SnowballThrower.IsValid()) {
		auto characterOwner = GetCharacterOwner();
		const auto hostile = characterquery::is::hostile(characterOwner);
		const auto stunned = [](const ABaseCharacter* character) { 
			auto abilitySystem = character->GetAbilitySystemComponent();
			FGameplayEffectQuery query;
			query.EffectDefinition = UStaggerGameplayEffect::StaticClass();
			auto activeEffects = abilitySystem->GetActiveEffects(query);
			return activeEffects.Num() > 0;
		};
		const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && !stunned(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

		TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);
		if (targets.Num() > 0) {
			auto target = targets[FMath::RandRange(0, targets.Num() - 1)];
			SnowballThrower->ShootSnowBallAtTarget(target, target->IsA(AMobCharacter::StaticClass()) ? MobStunDuration : PlayerStunDuration);
		}
	}
}

void USnowing::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USnowing, SnowballThrower);
}

float USnowing::GetExecutionInterval() const{
	return LevelMultiplier(Level);
}
