#include "OxygenBubbleActor.h"
#include "ProjectileManager.h"

AOxygenBubbleActor::AOxygenBubbleActor() : Super() {
	PrimaryActorTick.bCanEverTick = true;
	mParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Oxygen Particles"));
	RootComponent = mParticles.Get();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> s_MainParticlesTemplate(TEXT("ParticleSystem'/Game/Content_DLC5/Decor/Prefabs/_HiddenDepths/Oxygen_Collection/P_Oxygen_Collection_Bubble.P_Oxygen_Collection_Bubble'"));
	check(s_MainParticlesTemplate.Object); //need a default template
	if (s_MainParticlesTemplate.Object != nullptr)
	{
		mParticles->SetTemplate(Cast<UParticleSystem>(s_MainParticlesTemplate.Object));
	}

	mParticles->SecondsBeforeInactive = 0.0f;
}

void AOxygenBubbleActor::Tick(float DeltaSecs) {
	Super::Tick(DeltaSecs);

	if (mTarget.IsValid()) {
		if (mWaitTime > 0) {
			mWaitTime -= DeltaSecs;
			return;
		}

		const FVector MyLocation = GetActorLocation();
		const FVector TargetLocation = mTarget->GetActorLocation();
		const FVector TargetVector = TargetLocation - MyLocation;

		if (TargetVector.SizeSquared() <= mMinGoalCollectionRadiusSquared)
		{
			Disable(true);
			return;
		}

		//not collected, lets move
		mFlightTime += DeltaSecs;

		const FVector GoalDirection = TargetVector.GetSafeNormal();
		const FVector NormVelocity = mVelocity.GetSafeNormal();

		const float fVelocityLerp = FMath::Clamp((mFlightTime * mTurnAroundTimeInv), 0.0f, 1.0f);

		const FVector LerpedVelocity = NormVelocity + fVelocityLerp * (GoalDirection - NormVelocity);

		const float fNewSpeed = fVelocityLerp * mMaxSpeed * DeltaSecs;

		mVelocity = LerpedVelocity * fNewSpeed;

		SetActorLocation(MyLocation + mVelocity, false, nullptr, ETeleportType::ResetPhysics);

	}
	else {
		Disable();
	}
}

void AOxygenBubbleActor::BeginPlay() {
	Super::BeginPlay();
	SetLifeSpan(0.0f);
	Disable();
}

void AOxygenBubbleActor::LaunchBubble(FVector pSource, APlayerCharacter* pTarget) {
	if (pTarget)
	{
		mTarget = pTarget;

		const FVector StartVelocity(0, 0, FMath::RandRange(0.0f, 3.0f));
		mVelocity = StartVelocity.GetSafeNormal();

		mTurnAroundTimeInv = 1.0f / FMath::RandRange(3.0f, 5.0f);

		mMaxSpeed = FMath::RandRange(mMinRandSpeed, mMaxRandSpeed);

		mFlightTime = 0.0f;
		mWaitTime = 0.5f;

		mGoalDirection = FVector::ZeroVector;

		SetActorLocation(pSource, false, nullptr, ETeleportType::ResetPhysics);

		mParticles->Activate();
		mParticles->SetComponentTickEnabled(true);
	}
	else
	{
		Disable();
	}
}

void AOxygenBubbleActor::Disable(bool AtPlayer /*= false*/) {
	mTarget.Reset();

	mGoalDirection = FVector::ZeroVector;
	mVelocity = FVector::ZeroVector;

	mTurnAroundTimeInv = 0.0f;
	mMaxSpeed = 0.0f;
	mMinGoalCollectionRadiusSquared = 10000.0f;
	mFlightTime = 0.0f;

	mParticles->Activate(true);
	mParticles->ResetParticles(true);
	mParticles->Deactivate();
	mParticles->SetComponentTickEnabled(false);

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}
