// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseRangedWeaponAnimNotifyState.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "game/component/EquipmentComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"


void UBaseRangedWeaponAnimNotifyState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) {
	auto sound = GetSoundForRangedWeapon(MeshComp);
	if (sound) {
		UGameplayStatics::SpawnSoundAttached(sound, MeshComp);
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

USoundCue* UBaseRangedWeaponAnimNotifyState::GetSoundForRangedWeapon(USkeletalMeshComponent* MeshComp) {
	auto player = MeshComp ? MeshComp->GetOwner() : nullptr;
	auto equipment = player ? player->FindComponentByClass<UEquipmentComponent>() : nullptr;
	auto slots = equipment ? equipment->GetSlotsOfType(ESlotType::RangedWeapon) : TArray<UItemSlot*>();
	auto item = slots.Num() ? slots[0]->GetItem() : nullptr;
	auto rangedWeapon = item ? Cast<ARangedWeaponGearItemInstance>(item) : nullptr;

	return rangedWeapon ? rangedWeapon->GetAnimNotifyStateSound(GetNotifyName()) : nullptr;
}
