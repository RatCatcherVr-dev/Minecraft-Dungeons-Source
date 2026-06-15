#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "ActorShake.generated.h"

class AActor;

UCLASS(Blueprintable, editinlinenew)
class DUNGEONS_API UActorShake : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** 
	 *  If true to only allow a single instance of this shake class to play at any given time.
	 *  Subsequent attempts to play this shake will simply restart the timer.
	 */
	UPROPERTY(EditAnywhere, Category= ActorShake)
	uint32 bSingleInstance:1;

	/** Duration in seconds of current screen shake. Less than 0 means indefinite, 0 means no oscillation. */
	UPROPERTY(EditAnywhere, Category=Oscillation)
	float OscillationDuration;

	/** Duration of the blend-in, where the oscillation scales from 0 to 1. */
	UPROPERTY(EditAnywhere, Category=Oscillation, meta=(ClampMin = "0.0"))
	float OscillationBlendInTime;

	/** Duration of the blend-out, where the oscillation scales from 1 to 0. */
	UPROPERTY(EditAnywhere, Category=Oscillation, meta = (ClampMin = "0.0"))
	float OscillationBlendOutTime;

	/** Positional oscillation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Oscillation)
	struct FVOscillator LocOscillation;

protected:

	// INSTANCE DATA
	
	/** True if this shake is currently blending in. */
	uint16 bBlendingIn:1;

	/** True if this shake is currently blending out. */
	uint16 bBlendingOut:1;

	/** How long this instance has been blending in. */
	float CurrentBlendInTime;

	/** How long this instance has been blending out. */
	float CurrentBlendOutTime;

	/** Current location sinusoidal offset. */
	FVector LocSinOffset;

	/** Current rotational sinusoidal offset. */
	FVector RotSinOffset;

	/** Current FOV sinusoidal offset. */
	float FOVSinOffset;

	/** Initial offset (could have been assigned at random). */
	FVector InitialLocSinOffset;

	/** Initial offset (could have been assigned at random). */
	FVector InitialRotSinOffset;

	/** Initial offset (could have been assigned at random). */
	float InitialFOVSinOffset;

	/** Matrix defining the playspace, used when PlaySpace == CAPS_UserDefined */
	FMatrix UserPlaySpaceMatrix;

	/** Actor to use */
	AActor* ActorToShake;
	UMeshComponent* ActorMeshComponent;
	FVector ActorStartingPosition;

public:
	/** Overall intensity scale for this shake instance. */
	UPROPERTY(transient, BlueprintReadWrite, Category = ActorShake)
	float ShakeScale;

	/** Time remaining for oscillation shakes. Less than 0.f means shake infinitely. */
	UPROPERTY(transient, BlueprintReadOnly, Category = ActorShake)
	float OscillatorTimeRemaining;

public:

	// Native API
	virtual void UpdateAndApplyActorShake(float DeltaTime, float Alpha);
	virtual void PlayShake(AActor* newActorToShake, float Scale, FRotator UserPlaySpaceRot = FRotator::ZeroRotator);
	virtual bool IsFinished() const;

	/** 
	 * Stops this shake from playing. 
	 * @param bImmediatly		If true, shake stops right away regardless of blend out settings. If false, shake may blend out according to its settings.
	 */
	virtual void StopShake(bool bImmediately = true);

	// Returns true if this shake will loop forever
	bool IsLooping() const;

	/** Sets current playback time and applies the shake. */
	void SetCurrentTimeAndApplyShake(float NewTime);

	/* Returns the actor to shake */
	AActor* GetActorToShake() const;
};



