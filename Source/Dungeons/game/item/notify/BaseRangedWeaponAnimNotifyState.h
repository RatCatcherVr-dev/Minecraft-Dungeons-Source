// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BaseRangedWeaponAnimNotifyState.generated.h"

UCLASS()
class DUNGEONS_API UBaseRangedWeaponAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
private:
	class USoundCue* GetSoundForRangedWeapon(USkeletalMeshComponent* MeshComp);
};
