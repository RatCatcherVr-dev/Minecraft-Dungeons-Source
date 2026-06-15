// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemTypeDefs.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "game/actor/item/GearActor.h"
#include "EquipmentSlotActorAnimNotifyState.generated.h"

UCLASS()
class DUNGEONS_API UEquipmentSlotActorAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	UFUNCTION(BlueprintImplementableEvent)
	bool Received_SlotActorNotifyBegin(AGearActor* GearSlotActor, UAnimSequenceBase * Animation, float TotalDuration) const;

	UFUNCTION(BlueprintImplementableEvent)
	bool Received_SlotActorNotifyTick(AGearActor* GearSlotActor, UAnimSequenceBase * Animation, float FrameDeltaTime) const;

	UFUNCTION(BlueprintImplementableEvent)
	bool Received_SlotActorNotifyEnd(AGearActor* GearSlotActor, UAnimSequenceBase * Animation) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	ESlotType EquipmentSlotType = ESlotType::None;
private:

	TWeakObjectPtr<AGearActor> GearActorCache;
	AGearActor* GetEquipmentSlotGearActor(USkeletalMeshComponent * MeshComp) ;
};


