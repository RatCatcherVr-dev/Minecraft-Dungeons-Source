// Fill	out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "AnimNotifyState_TimedSoundEffect.h"

UAnimNotifyState_TimedSoundEffect::UAnimNotifyState_TimedSoundEffect(const FObjectInitializer&) {
}

void UAnimNotifyState_TimedSoundEffect::NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) {
	if (ValidateParameters(MeshComp)) {
		auto* audio = UGameplayStatics::SpawnSoundAttached(SoundCue, MeshComp, SocketName, LocationOffset, EAttachLocation::KeepRelativeOffset, true);
		AudioComps.Add(MeshComp, audio);
	}
	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_TimedSoundEffect::NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) {	
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_TimedSoundEffect::NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) {	
	TWeakObjectPtr<UAudioComponent> audioComponent;
	if (AudioComps.RemoveAndCopyValue(MeshComp, audioComponent))
	{
		if (audioComponent.IsValid())
			audioComponent->Stop();
	}

	Received_NotifyEnd(MeshComp, Animation);
}

bool UAnimNotifyState_TimedSoundEffect::ValidateParameters(const USkeletalMeshComponent* MeshComp) const {
	bool bValid = true;

	if (!SoundCue)
	{
		bValid = false;
	}
	else if (!SocketName.IsNone() && !MeshComp->DoesSocketExist(SocketName) && MeshComp->GetBoneIndex(SocketName) == INDEX_NONE)
	{
		bValid = false;
	}

	return bValid;
}

FString UAnimNotifyState_TimedSoundEffect::GetNotifyName_Implementation() const {
	FString name;
	if (SoundCue) {
		SoundCue->GetName(name);
	}

	return name;
}