#include "Dungeons.h"
#include "FishHookItem.h"

#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include <Engine.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

AFishHookItem::AFishHookItem(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
}

float GetRadius(USceneComponent* sceneComp) {
	if (auto capsule = Cast<UCapsuleComponent>(sceneComp)) {
		return capsule->GetScaledCapsuleRadius();
	}
	else if (auto sphere = Cast<USphereComponent>(sceneComp)) {
		return sphere->GetScaledSphereRadius();
	}
	return 0.f;
}

float GetRadiusSum(AActor* actor1, AActor* actor2) {
	return GetRadius(actor1->GetRootComponent()) + GetRadius(actor2->GetRootComponent());
}

void AFishHookItem::BeginPlay() {
	Super::BeginPlay();
	if (auto sphere = Cast<USphereComponent>(RootComponent)) {
		sphere->SetSimulatePhysics(false);
		sphere->SetMobility(EComponentMobility::Movable);
		sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		sphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Overlap);
	}

	if (TargetActor) {
		if (const auto targetActorCollisionMesh = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent())) {
			targetActorCollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	InitializeOwner();
}

void AFishHookItem::OnRep_Owner() {
	Super::OnRep_Owner();

	InitializeOwner();
}

void AFishHookItem::InitializeOwner() {
	if (GetOwner()) {
		if (!CachedPlayerForwardDirectionOnSpawn) {
			CachedPlayerForwardDirectionOnSpawn = GetOwner()->GetActorForwardVector();
		}

		if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
			if (!StopPullbackDistance) {
				if (TargetActor) {
					StopPullbackDistance = 100.f + GetRadiusSum(playerOwner, TargetActor);
				}
			}

			if (const auto skeleton = playerOwner->GetMesh()) {
				PlayerSkeleton = skeleton;
			}
		}
	}
}

void AFishHookItem::Tick(float DeltaSeconds) {
	const FVector& hookLocation = GetActorLocation();
	const auto owner = GetOwner();
	const FVector& playerLocation = owner->GetActorLocation();
	const auto& weaponSocketTransform = PlayerSkeleton->GetSocketTransform(FName("J_R_Weapon_Socket"));

	auto fishLineBeamEffect = FindComponentByClass<UParticleSystemComponent>();
	
	FRotator socketRotation = weaponSocketTransform.Rotator();
	//socketRotation += FRotator(-90.0f, 0.0f, -90.0f);
	fishLineBeamEffect->SetBeamSourcePoint(1, weaponSocketTransform.GetLocation() /*- (socketRotation.Vector() * 100.0f)*/, 0);
	
	if (HookAcc < 100000.0f)
		HookAcc += 100000.0f * DeltaSeconds;
	if (HookSpeed < 2000.0f)
		HookSpeed += HookAcc * DeltaSeconds;

	const FVector playerToHookDistance = hookLocation - playerLocation;

	FVector rotateVector;
	if (!ShouldGoback) {

		if ((playerToHookDistance.Size() > MaxReach)) {
			ShouldGoback = true;
			MissedActor = true;
			HookSpeed = 0.0f;
			HookAcc = 0.0f;
			CachePullbackDirection();
			OnHookonDenied.ExecuteIfBound(PredictionKey);
			return;
		}
		if (TargetActor) {
			const FVector& targetActorLocation = TargetActor->GetActorLocation();
			const FVector hookToActorDistance = targetActorLocation - hookLocation;
			
			if ((hookToActorDistance.Size() < StopPullbackDistance.Get(0.f))) {
				HookSpeed = 0.0f;
				HookAcc = 0.0f;
				OnHitObject(this, TargetActor);
				
				return;
			}

			FVector hookToActorDirection = hookToActorDistance;
			hookToActorDirection.Normalize();
			hookToActorDirection *= HookSpeed * DeltaSeconds * 2.0f;
			rotateVector = hookToActorDirection;
			SetActorRotation(rotateVector.Rotation());
			AddActorWorldOffset(hookToActorDirection);

			if (bFirstTick) {
				bFirstTick = false;
			}
		}
		else {
			FVector moveDirection = CachedPlayerForwardDirectionOnSpawn.Get(FVector::ZeroVector);
			moveDirection.Normalize();
			moveDirection *= HookSpeed * DeltaSeconds * 2.0f;
			AddActorWorldOffset(moveDirection);

			if (bFirstTick) {
				bFirstTick = false;
			}
		}
	}
	else {
		if (owner) {
			const FVector hookToPlayerDistance = playerLocation - hookLocation;

			FVector hookToPlayerDirection = hookToPlayerDistance;
			hookToPlayerDirection.Normalize();
			hookToPlayerDirection *= HookSpeed * DeltaSeconds * 4.0f;
			rotateVector = -hookToPlayerDirection;

			SetActorRotation(rotateVector.Rotation());
			AddActorWorldOffset(hookToPlayerDirection);
			if (!MissedActor && TargetActor) {
				TargetActor->AddActorWorldOffset(hookToPlayerDirection);
			}

			const auto newPlayerToHookDistance = GetActorLocation() - owner->GetActorLocation();
			auto playerToHookDir = newPlayerToHookDistance;
			playerToHookDir.Normalize();

			const auto isBehindPlayer = FVector::DotProduct(CachedPullbackDirection, playerToHookDir) < 0.0f;

			if (!isBehindPlayer && newPlayerToHookDistance.Size() > 0) {
				LastValidPlayerToHookDirection = newPlayerToHookDistance;
				LastValidPlayerToHookDirection.Normalize();
			}

			if ((newPlayerToHookDistance.Size() < StopPullbackDistance.Get(0.f)) || isBehindPlayer) {
				OnHitObject(this, owner);
			}
		}
	}
}

void AFishHookItem::CachePullbackDirection() {
	CachedPullbackDirection = GetActorLocation() - GetOwner()->GetActorLocation();
	CachedPullbackDirection.Normalize();
}

void AFishHookItem::OnHitObject(AActor* OverlappedActor, AActor* OtherActor) {
	if (OtherActor == TargetActor) {
		if (!ShouldGoback) {
			ShouldGoback = true;
			CachePullbackDirection();
		}

		auto owner = Cast<ABaseCharacter>(GetOwner());
		auto abilitySystem = owner->FindComponentByClass<UAbilitySystemComponent>();
		check(abilitySystem && "Must have ability system.");

		EntityType mobEntityType = EntityType::Mob;
		if (auto mob = Cast<AMobCharacter>(OtherActor)) {
			mobEntityType = mob->EntityType;
			if (mob->HasAuthority()) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Location);
				mob->SetLastInjuredBy(owner);
				auto targetAbilitySystem = mob->GetAbilitySystemComponent();
				
				FGameplayEffectSpec spec(Cast<UGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), 1.0f);
				spec.GetContext().AddOrigin(GetActorLocation());
				spec.SetSetByCallerMagnitude(effects::DurationName, StunDuration);
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
			}
		}

		OnHookHitTarget.ExecuteIfBound(PredictionKey, mobEntityType);
	}
	if (auto player = Cast<APlayerCharacter>(OtherActor)) {
		if (ShouldGoback) {
			if (TargetActor) {
				if (const auto* playerCapsule = Cast<UCapsuleComponent>(player->GetRootComponent())) {
					if (auto targetCapsule = Cast<UCapsuleComponent>(TargetActor->GetRootComponent())) {
						const auto& playerLocation = player->GetActorLocation();
						FVector offsetDir = LastValidPlayerToHookDirection;
						if (offsetDir.Size() <= 0) {
							offsetDir = TargetActor->GetActorLocation() - player->GetActorLocation();
							offsetDir.Normalize();
						}
						const auto adjustedTargetActorLocation = playerLocation + (offsetDir * StopPullbackDistance.Get(0.f));
						TargetActor->SetActorLocation(adjustedTargetActorLocation, false, nullptr, ETeleportType::TeleportPhysics);

						targetCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					}
				}
			}
			OnHookHitPlayer.ExecuteIfBound(PredictionKey);
			Destroy();
		}
	}
}

void AFishHookItem::SetTargetActor(AActor *target) {
	TargetActor = target;
}

void AFishHookItem::SetMaxReach(float reach) {
	MaxReach = reach;
}

void AFishHookItem::SetStunDuration(float stunDuration) {
	StunDuration = stunDuration;
}

void AFishHookItem::SetPredictionKey(const FPredictionKey& predictionKey) {
	PredictionKey = predictionKey;
}
