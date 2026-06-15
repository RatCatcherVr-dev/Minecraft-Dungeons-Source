
#include "ActorShake.h"
#include "Engine/Engine.h"
#include "Components/MeshComponent.h"

DECLARE_CYCLE_STAT(TEXT("ActorShakePlayShake"), STAT_PlayShake, STATGROUP_Game);

UActorShake::UActorShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSingleInstance = true;

	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.5f;
	
	LocOscillation.X.Amplitude = 15.f;
	LocOscillation.X.Frequency = 101.f;
	LocOscillation.Y.Amplitude = 15.f;
	LocOscillation.Y.Frequency = 101.f;
	LocOscillation.Z.Amplitude = 15.f;
	LocOscillation.Z.Frequency = 101.f;
}

void UActorShake::StopShake(bool bImmediately)
{
	if (bImmediately)
	{
		// stop oscillation
		OscillatorTimeRemaining = 0.f;
	}
	else
	{
		// advance to the blend out time
		if (OscillatorTimeRemaining > 0.0f)
		{
			OscillatorTimeRemaining = FMath::Min(OscillatorTimeRemaining, OscillationBlendOutTime);
		}
		else
		{
			OscillatorTimeRemaining = OscillationBlendOutTime;
		}
	}
}

void UActorShake::PlayShake(AActor* newActorToShake, float Scale, FRotator UserPlaySpaceRot)
{
	SCOPE_CYCLE_COUNTER(STAT_PlayShake);

	ShakeScale = Scale;
	ActorToShake = newActorToShake;
	ActorMeshComponent = ActorToShake->FindComponentByClass<UMeshComponent>();

	if (!ActorMeshComponent) {
		ActorToShake = nullptr;
		return;
	}

	// init oscillations
	if (OscillationDuration != 0.f)
	{

		if (OscillatorTimeRemaining > 0.f)
		{
			ActorMeshComponent->SetRelativeLocation(ActorStartingPosition);

			// this shake was already playing
			OscillatorTimeRemaining = OscillationDuration;

			if (bBlendingOut)
			{
				bBlendingOut = false;
				CurrentBlendOutTime = 0.f;

				// stop any blendout and reverse it to a blendin
				if (OscillationBlendInTime > 0.f)
				{
					bBlendingIn = true;
					CurrentBlendInTime = OscillationBlendInTime * (1.f - CurrentBlendOutTime / OscillationBlendOutTime);
				}
				else
				{
					bBlendingIn = false;
					CurrentBlendInTime = 0.f;
				}
			}
		}
		else
		{

			ActorStartingPosition = ActorMeshComponent->GetRelativeTransform().GetLocation();

			LocSinOffset.X = FFOscillator::GetInitialOffset(LocOscillation.X);
			LocSinOffset.Y = FFOscillator::GetInitialOffset(LocOscillation.Y);
			LocSinOffset.Z = FFOscillator::GetInitialOffset(LocOscillation.Z);

			InitialLocSinOffset = LocSinOffset;

			OscillatorTimeRemaining = OscillationDuration;

			if (OscillationBlendInTime > 0.f)
			{
				bBlendingIn = true;
				CurrentBlendInTime = 0.f;
			}
		}
	}
}

void UActorShake::UpdateAndApplyActorShake(float DeltaTime, float Alpha)
{
	// this is the base scale for the whole shake, anim and oscillation alike
	float const BaseShakeScale = FMath::Max<float>(Alpha * ShakeScale, 0.0f);

	// update oscillation times
	if (OscillatorTimeRemaining > 0.f)
	{
		OscillatorTimeRemaining -= DeltaTime;
		OscillatorTimeRemaining = FMath::Max(0.f, OscillatorTimeRemaining);
	}
	if (bBlendingIn)
	{
		CurrentBlendInTime += DeltaTime;
	}
	if (bBlendingOut)
	{
		CurrentBlendOutTime += DeltaTime;
	}

	// see if we've crossed any important time thresholds and deal appropriately
	bool bOscillationFinished = false;

	if (OscillatorTimeRemaining == 0.f)
	{
		// finished!
		bOscillationFinished = true;
		ActorMeshComponent->SetRelativeLocation(ActorStartingPosition);
	}
	else if (OscillatorTimeRemaining < 0.0f)
	{
		// indefinite shaking
	}
	else if (OscillatorTimeRemaining < OscillationBlendOutTime)
	{
		// start blending out
		bBlendingOut = true;
		CurrentBlendOutTime = OscillationBlendOutTime - OscillatorTimeRemaining;
	}

	if (bBlendingIn)
	{
		if (CurrentBlendInTime > OscillationBlendInTime)
		{
			// done blending in!
			bBlendingIn = false;
		}
	}
	if (bBlendingOut)
	{
		if (CurrentBlendOutTime > OscillationBlendOutTime)
		{
			// done!!
			CurrentBlendOutTime = OscillationBlendOutTime;
			bOscillationFinished = true;
		}
	}

	// Do not update oscillation further if finished
	if (bOscillationFinished == false)
	{
		// calculate blend weight. calculating separately and taking the minimum handles overlapping blends nicely.
		float const BlendInWeight = (bBlendingIn) ? (CurrentBlendInTime / OscillationBlendInTime) : 1.f;
		float const BlendOutWeight = (bBlendingOut) ? (1.f - CurrentBlendOutTime / OscillationBlendOutTime) : 1.f;
		float const CurrentBlendWeight = FMath::Min(BlendInWeight, BlendOutWeight);

		// this is the oscillation scale, which includes oscillation fading
		float const OscillationScale = BaseShakeScale * CurrentBlendWeight;

		if (OscillationScale > 0.f)
		{
			// View location offset, compute sin wave value for each component
			FVector	LocOffset = FVector(0);
			LocOffset.X = FFOscillator::UpdateOffset(LocOscillation.X, LocSinOffset.X, DeltaTime);
			LocOffset.Y = FFOscillator::UpdateOffset(LocOscillation.Y, LocSinOffset.Y, DeltaTime);
			LocOffset.Z = FFOscillator::UpdateOffset(LocOscillation.Z, LocSinOffset.Z, DeltaTime);
			LocOffset *= OscillationScale;

			{
				// find desired space
				FMatrix const PlaySpaceToWorld = FMatrix::Identity;

				// apply loc offset relative to world space
				ActorMeshComponent->SetWorldLocation(ActorMeshComponent->GetComponentLocation() += PlaySpaceToWorld.TransformVector(LocOffset));
			}
		}
	}
}

bool UActorShake::IsFinished() const
{
	return OscillatorTimeRemaining <= 0.f && IsLooping() == false;
}

bool UActorShake::IsLooping() const
{
	return OscillationDuration < 0.0f;
}

void UActorShake::SetCurrentTimeAndApplyShake(float NewTime)
{
	// reset to start and advance to desired point
	LocSinOffset = InitialLocSinOffset;
	RotSinOffset = InitialRotSinOffset;
	FOVSinOffset = InitialFOVSinOffset;

	OscillatorTimeRemaining = OscillationDuration;

	if (OscillationBlendInTime > 0.f)
	{
		bBlendingIn = true;
		CurrentBlendInTime = 0.f;
	}

	if (OscillationDuration > 0.f)
	{
		if ((OscillationBlendOutTime > 0.f) && (NewTime > OscillationBlendOutTime))
		{
			bBlendingOut = true;
			CurrentBlendOutTime = OscillationBlendOutTime - (OscillationDuration - NewTime);
		}
	}

	UpdateAndApplyActorShake(NewTime, 1.f);
}

AActor* UActorShake::GetActorToShake() const
{
	return ActorToShake;
}
