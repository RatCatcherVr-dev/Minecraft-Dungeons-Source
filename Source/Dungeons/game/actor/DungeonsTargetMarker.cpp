// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "DungeonsTargetMarker.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "character/BaseCharacter.h"
#include "UnrealNetwork.h"
#include "game/component/AoeAttackComponent.h"
#include "game/component/AoeMarkerComponent.h"

// Sets default values
ADungeonsTargetMarker::ADungeonsTargetMarker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComp;
	FollowTargetVFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FollowTargetVFX"));
	FollowTargetVFX->SetupAttachment(RootComponent);
	FollowTargetVFX->SetRelativeLocation(FVector(.0f, .0f, .0f));
	AttackVFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AttackVFX"));
	AttackVFX->SetupAttachment(RootComponent);
	AttackVFX->SetRelativeLocation(FVector(.0f, .0f, .0f));

	bReplicates = true;
}

// Called when the game starts or when spawned
void ADungeonsTargetMarker::BeginPlay()
{
	Super::BeginPlay();

	FollowTargetVFX->SetRelativeLocation(FVector(.0f, .0f, .0f));
	AttackVFX->SetRelativeLocation(FVector(.0f, .0f, .0f));

	SetActorTickEnabled(MarkerSpeed > 0);
}

// Called every frame
void ADungeonsTargetMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTargeting && CurrentTarget.IsValid())
	{
		if (finalTarget.IsSet())
		{
			FVector newLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), finalTarget.Get(CurrentTarget->GetActorLocation()), DeltaTime, MarkerSpeed);
			SetActorLocation(newLocation);
		}
		if (CurrentTarget.IsValid())
		{
			// Move the vfx marker to follow our target
			FVector newLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), CurrentTarget->GetActorLocation(), DeltaTime, MarkerSpeed);
			SetActorLocation(newLocation);
		}
	}
	else
	{
		if (!bIsTargeting && CurrentTarget.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Target is valid but isn't currently targeting")));
		}
		else if (bIsTargeting && !CurrentTarget.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Targeting but has no Target")));
		}
	}
}

void ADungeonsTargetMarker::BeginAttack(UAoeMarkerComponent* ownerAttackComponent, AActor* newTarget, UAoeTargetProvider* targetProvider, float lockDelay)
{
	ResetLockedMulticast();
	SetOwnerComponent(ownerAttackComponent);
	SetTargetAndProvider(newTarget, targetProvider);
	OnAttackBegan(lockDelay);
}

void ADungeonsTargetMarker::OnAttackBegan(float lockDelay)
{
	SetActorHiddenInGame(false);
	targetLockedDelay = lockDelay;
	GetWorld()->GetTimerManager().ClearTimer(targetLockedTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(targetLockedTimerHandle, FTimerDelegate::CreateUObject(this, &ADungeonsTargetMarker::OnTargetLocked), targetLockedDelay, false);

	SetupAttackBeganParticles();

	bIsTargeting = true;

	OnAttackBeganMulticast(lockDelay);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("ATTACK BEGIN")));
}

void ADungeonsTargetMarker::OnAttackEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(targetLockedTimerHandle);
	AttackVFX->SetHiddenInGame(true);
	FollowTargetVFX->SetHiddenInGame(true);

	bIsTargeting = false;

	OnAttackEndedMulticast();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("ATTACK ENDED")));
}

void ADungeonsTargetMarker::SetOwnerComponent(UAoeMarkerComponent* componentOwner)
{
	MarkerOwner = componentOwner;
}

void ADungeonsTargetMarker::SetTargetAndProvider(AActor* newTarget, UAoeTargetProvider* targetProvider)
{
	CurrentTarget = newTarget;
	MarkerTargetProvider = targetProvider;
}

void ADungeonsTargetMarker::OnCharacterDeath()
{
	Destroy();
}

void ADungeonsTargetMarker::SetupTargetLockedParticles_Implementation()
{
	FollowTargetVFX->SetHiddenInGame(true);

	AttackVFX->SetHiddenInGame(false);
	AttackVFX->ResetParticles();
	AttackVFX->Activate(true);

	bIsTargeting = false;
}

void ADungeonsTargetMarker::SetupAttackBeganParticles_Implementation()
{
	AttackVFX->SetHiddenInGame(true);

	FollowTargetVFX->SetHiddenInGame(false);
	FollowTargetVFX->ResetParticles();
	FollowTargetVFX->Activate(true);
}

void ADungeonsTargetMarker::ResetLockedMulticast_Implementation()
{
	finalTarget.Reset();
}

void ADungeonsTargetMarker::OnTargetLockedMulticast_Implementation(FVector endTarget)
{
	finalTarget = endTarget;
	OnTargetLockedBP();
}

void ADungeonsTargetMarker::OnTargetLocked()
{
	SetupTargetLockedParticles();

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("TARGET LOCKED")));

	OnTargetLockedMulticast(GetActorLocation());

	MarkerOwner->SetLockedMarker(this);
	MarkerOwner->ApplyDamage(FSharedPredictionContext());
}

void ADungeonsTargetMarker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADungeonsTargetMarker, CurrentTarget);
	DOREPLIFETIME(ADungeonsTargetMarker, bIsTargeting);
}

void ADungeonsTargetMarker::OnAttackBeganMulticast_Implementation(float LockDelay) {
	OnAttackBeganBP(LockDelay);
}

void ADungeonsTargetMarker::OnAttackEndedMulticast_Implementation() {
	OnAttackEndedBP();
}