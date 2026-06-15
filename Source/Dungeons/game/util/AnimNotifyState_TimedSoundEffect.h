// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include <Sound/SoundCue.h>
#include "AnimNotifyState_TimedSoundEffect.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAnimNotifyState_TimedSoundEffect : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()
public:
	// The particle system template to use when spawning the particle component
	UPROPERTY(EditAnywhere, Category = Audio, meta = (ToolTip = "The sound cue to spawn for the notify state"))
	USoundCue* SoundCue = nullptr;

	// The socket within our mesh component to attach to when we spawn the particle component
	UPROPERTY(EditAnywhere, Category = Audio, meta = (ToolTip = "The socket or bone to attach the system to"))
	FName SocketName;

	// Offset from the socket / bone location
	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (ToolTip = "Offset from the socket or bone to place the particle system"))
	FVector LocationOffset;

	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;

	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;
private:
	TMap<class USkeletalMeshComponent *, TWeakObjectPtr<UAudioComponent>> AudioComps;

	bool ValidateParameters(const USkeletalMeshComponent* MeshComp) const;
};
