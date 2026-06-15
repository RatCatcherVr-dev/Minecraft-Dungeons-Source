#include "XPOrb.h"
#include "character/player/PlayerCharacter.h"
#include "ProjectileManager.h"

AXPOrb::AXPOrb():
Super()
{
	PrimaryActorTick.bCanEverTick = true;
	mParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("XP Particles"));
	RootComponent = mParticles.Get();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> s_MainParticlesTemplate(TEXT("ParticleSystem'/Game/Effects/Blueprints/Experience/PS_ExperienceOrb.PS_ExperienceOrb'"));
	check(s_MainParticlesTemplate.Object); //need a default template
	if (s_MainParticlesTemplate.Object != nullptr)
	{
		mParticles->SetTemplate(Cast<UParticleSystem>(s_MainParticlesTemplate.Object));
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> s_HitParticlesTemplate(TEXT("ParticleSystem'/Game/Effects/Blueprints/Experience/PS_ExperienceImpact.PS_ExperienceImpact'"));
	check(s_HitParticlesTemplate.Object); //need a default template
	if (s_HitParticlesTemplate.Object != nullptr)
	{
		HitParticleSystemTemplate = Cast<UParticleSystem>(s_HitParticlesTemplate.Object);
	}

	mParticles->SecondsBeforeInactive = 0.0f;

}

void AXPOrb::Tick(float DeltaSecs)
{
	Super::Tick(DeltaSecs);

	if (mTarget.IsValid())
	{
		const FVector MyLocation = GetActorLocation();
		const FVector TargetLocation = mTarget->GetActorLocation();
		const FVector TargetVector = TargetLocation - MyLocation;

		if (TargetVector.SizeSquared() <= mMinGoalCollectionRadiusSquared)
		{
			DisableOrb(true);
			return;
		}

		//not collected, lets move
		mFlightTime += DeltaSecs;

		const FVector GoalDirection = TargetVector.GetSafeNormal();
		const FVector NormVelocity = mVelocity.GetSafeNormal();

		const float fVelocityLerp = FMath::Clamp((mFlightTime * mTurnAroundTimeInv), 0.0f, 1.0f);

		const FVector LerpedVelocity =  NormVelocity + fVelocityLerp * (GoalDirection - NormVelocity);

		const float fNewSpeed = fVelocityLerp * mMaxSpeed * DeltaSecs;

		mVelocity = LerpedVelocity * fNewSpeed;

		SetActorLocation(MyLocation + mVelocity, false, nullptr, ETeleportType::ResetPhysics);

	}
	else
	{
		DisableOrb();
	}
}

void AXPOrb::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(0.0f);
	DisableOrb();
}

void AXPOrb::LaunchOrb(AActor* pSource, APlayerCharacter* pTarget)
{
	if (pSource && pTarget)
	{
		mTarget = pTarget;

		const FVector StartVelocity(FMath::RandRange(-2.0f, 1.0f), FMath::RandRange(-2.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));
		mVelocity = StartVelocity.GetSafeNormal();

		mTurnAroundTimeInv = 1.0f / FMath::RandRange(3.0f, 14.0f);

		mMaxSpeed = FMath::RandRange(4800.0f, 9600.0f);

		const float fScale = FMath::RandRange(0.9f, 1.2f);
		const FVector StartScale(fScale, fScale, fScale);
		SetActorScale3D(StartScale);

		mFlightTime = 0.0f;

		mGoalDirection = FVector::ZeroVector;

		SetActorLocation(pSource->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);

		mParticles->Activate();
		mParticles->SetComponentTickEnabled(true);

	}
	else
	{
		DisableOrb();
	}
}

void AXPOrb::DisableOrb(bool AtPlayer)
{
	if (AtPlayer)
	{
		//do collect effects
		DoOrbCollect();
	}

	mTarget.Reset();

	mGoalDirection = FVector::ZeroVector;
	mVelocity = FVector::ZeroVector;

	mTurnAroundTimeInv = 0.0f;
	mMaxSpeed = 0.0f;
	mMinGoalCollectionRadiusSquared = 10000.0f;
	mFlightTime = 0.0f;

	mParticles->Activate(true); //reset then deactivate
	mParticles->Deactivate();
	mParticles->SetComponentTickEnabled(false);

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void AXPOrb::DoOrbCollect()
{
	const FVector MyLocation = GetActorLocation();
	const auto world = GetWorld();
	AProjectileActorManager::SpawnHitParticleEffect(world, HitParticleSystemTemplate, MyLocation, FRotator::ZeroRotator);
	mTarget->PlayXPPickupAudioComponent();
}
