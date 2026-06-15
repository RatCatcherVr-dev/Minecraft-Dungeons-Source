#include "Dungeons.h"
#include "EquipmentSlotActorAnimNotifyState.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"

void UEquipmentSlotActorAnimNotifyState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration){
	if (auto SlotActor = GetEquipmentSlotGearActor(MeshComp)) {
		Received_SlotActorNotifyBegin(SlotActor, Animation, TotalDuration);
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UEquipmentSlotActorAnimNotifyState::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime){
	if (auto SlotActor = GetEquipmentSlotGearActor(MeshComp)) {
		Received_SlotActorNotifyTick(SlotActor, Animation, FrameDeltaTime);
	}
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UEquipmentSlotActorAnimNotifyState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation){
	if (auto SlotActor = GetEquipmentSlotGearActor(MeshComp)) {
		Received_SlotActorNotifyEnd(SlotActor, Animation);
	}
	Super::NotifyEnd(MeshComp, Animation);
	GearActorCache.Reset();
}

AGearActor* UEquipmentSlotActorAnimNotifyState::GetEquipmentSlotGearActor(USkeletalMeshComponent * MeshComp) {
	if(!GearActorCache.IsValid()){
		if (auto player = MeshComp ? MeshComp->GetOwner() : nullptr) {
			if (auto equipment = player->FindComponentByClass<UEquipmentComponent>()) {
				if (auto equipmentDisplay = player->FindComponentByClass<UEquipmentDisplayComponent>()) {
					if (auto slot = equipment->GetFirstSlotOfType(EquipmentSlotType)) {
						GearActorCache = equipmentDisplay->GetItemSlotCurrentGearActor(slot);
					}
				}
			}
		}
	}
	return GearActorCache.Get();
}

